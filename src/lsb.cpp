// lsb.cpp
// Raw LSB encoding/decoding for BMP
#include "lsb.h"
#include <stdexcept>
#include <cstring>

size_t lsb_capacity(const BMPImage& img) {
    if (img.data.size() < 32) return 0;
    return (img.data.size() - 32) / 8;
}

void lsb_encode(BMPImage& img, const std::vector<uint8_t>& message) {
    size_t cap = lsb_capacity(img);
    if (message.size() > cap)
        throw std::runtime_error("Message too large for image (capacity: " + std::to_string(cap) + " bytes)");
    // Write message length (in bytes) as first 32 bits (big-endian)
    for (int i = 0; i < 32; ++i) {
        img.data[i] = (img.data[i] & 0xFE) | ((message.size() >> (31 - i)) & 1);
    }
    // Write message bits
    for (size_t i = 0; i < message.size(); ++i) {
        for (int b = 0; b < 8; ++b) {
            img.data[32 + i * 8 + b] = (img.data[32 + i * 8 + b] & 0xFE) | ((message[i] >> (7 - b)) & 1);
        }
    }
}

std::vector<uint8_t> lsb_decode(const BMPImage& img, size_t max_bytes) {
    // Read message length (first 32 bits, big-endian)
    size_t msg_len = 0;
    for (int i = 0; i < 32; ++i) {
        msg_len = (msg_len << 1) | (img.data[i] & 1);
    }
    if (msg_len > max_bytes) throw std::runtime_error("Message too large or corrupted");
    if (32 + msg_len * 8 > img.data.size()) throw std::runtime_error("Image too small or corrupted");
    std::vector<uint8_t> message(msg_len);
    for (size_t i = 0; i < msg_len; ++i) {
        uint8_t byte = 0;
        for (int b = 0; b < 8; ++b) {
            byte = (byte << 1) | (img.data[32 + i * 8 + b] & 1);
        }
        message[i] = byte;
    }
    return message;
}
