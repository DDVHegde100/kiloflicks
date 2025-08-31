// Decodes a single 7-bit Hamming codeword to 4 bits, corrects single-bit errors
uint8_t hamming74_decode_codeword(uint8_t codeword, bool& had_error);
// hamming.h
// Hamming(7,4) encode/decode for error correction
#pragma once
#include <vector>
#include <cstdint>

// Encodes 4-bit nibbles into 7-bit Hamming codewords
std::vector<uint8_t> hamming74_encode(const std::vector<uint8_t>& data);

// Decodes 7-bit Hamming codewords into 4-bit nibbles, corrects single-bit errors
std::vector<uint8_t> hamming74_decode(const std::vector<uint8_t>& codewords, bool& had_error);
