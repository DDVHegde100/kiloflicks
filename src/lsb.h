// lsb.h
// Raw LSB encoding/decoding for BMP
#pragma once
#include "bmp.h"
#include <string>

// Encodes the message (as bytes) into the image using LSB. Throws on overflow.
void lsb_encode(BMPImage& img, const std::vector<uint8_t>& message);

// Decodes a message of up to max_bytes from the image using LSB.
std::vector<uint8_t> lsb_decode(const BMPImage& img, size_t max_bytes);
