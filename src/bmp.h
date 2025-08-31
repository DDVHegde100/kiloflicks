// bmp.h
// Simple 24-bit uncompressed BMP loader/writer
#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct BMPImage {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> data; // BGRBGR...
};

// Loads a 24-bit uncompressed BMP file. Throws std::runtime_error on error.
BMPImage load_bmp(const std::string& filename);

// Writes a 24-bit uncompressed BMP file. Throws std::runtime_error on error.
void write_bmp(const std::string& filename, const BMPImage& image);
