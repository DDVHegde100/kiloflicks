// lsb.h
// Raw LSB encoding/decoding for BMP
#pragma once
#include "bmp.h"
#include <string>


// Returns the maximum number of bytes that can be encoded in the image using LSB (including 32 bits for length)
size_t lsb_capacity(const BMPImage& img);

// Encodes the message (as bytes) into the image using LSB. Throws on overflow.
void lsb_encode(BMPImage& img, const std::vector<uint8_t>& message);

// Decodes a message of up to max_bytes from the image using LSB.
std::vector<uint8_t> lsb_decode(const BMPImage& img, size_t max_bytes);
