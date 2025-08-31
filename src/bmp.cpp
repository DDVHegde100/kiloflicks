// bmp.cpp
// Simple 24-bit uncompressed BMP loader/writer
#include "bmp.h"
#include <fstream>
#include <stdexcept>
#include <cstring>

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BMPInfoHeader {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

BMPImage load_bmp(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open BMP file: " + filename);

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    if (fileHeader.bfType != 0x4D42) throw std::runtime_error("Not a BMP file");
    if (infoHeader.biBitCount != 24 || infoHeader.biCompression != 0)
        throw std::runtime_error("Only 24-bit uncompressed BMP supported");

    int width = infoHeader.biWidth;
    int height = std::abs(infoHeader.biHeight);
    int row_padded = (width * 3 + 3) & (~3);
    std::vector<uint8_t> data(width * height * 3);

    file.seekg(fileHeader.bfOffBits, std::ios::beg);
    for (int y = 0; y < height; ++y) {
        int row = infoHeader.biHeight > 0 ? height - 1 - y : y;
        file.read(reinterpret_cast<char*>(&data[row * width * 3]), width * 3);
        file.ignore(row_padded - width * 3);
    }
    return BMPImage{width, height, std::move(data)};
}

void write_bmp(const std::string& filename, const BMPImage& image) {
    int width = image.width;
    int height = image.height;
    int row_padded = (width * 3 + 3) & (~3);
    int filesize = 54 + row_padded * height;

    BMPFileHeader fileHeader = {0x4D42, (uint32_t)filesize, 0, 0, 54};
    BMPInfoHeader infoHeader = {40, width, height, 1, 24, 0, 0, 0, 0, 0, 0};

    std::ofstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot write BMP file: " + filename);
    file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

    std::vector<uint8_t> row(row_padded);
    for (int y = 0; y < height; ++y) {
        std::memcpy(row.data(), &image.data[(height - 1 - y) * width * 3], width * 3);
        std::memset(row.data() + width * 3, 0, row_padded - width * 3);
        file.write(reinterpret_cast<const char*>(row.data()), row_padded);
    }
}
