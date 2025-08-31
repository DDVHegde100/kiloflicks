// hamming.cpp
// Hamming(7,4) encode/decode for error correction
#include "hamming.h"
#include <stdexcept>

// Helper: encode a single 4-bit nibble to 7 bits
static uint8_t hamming74_encode_nibble(uint8_t nibble) {
    // Data bits: d3 d2 d1 d0 (high to low)
    uint8_t d0 = (nibble >> 0) & 1;
    uint8_t d1 = (nibble >> 1) & 1;
    uint8_t d2 = (nibble >> 2) & 1;
    uint8_t d3 = (nibble >> 3) & 1;
    // Parity bits
    uint8_t p0 = d3 ^ d2 ^ d0;
    uint8_t p1 = d3 ^ d1 ^ d0;
    uint8_t p2 = d2 ^ d1 ^ d0;
    // Codeword: p0 p1 d3 p2 d2 d1 d0
    return (p0 << 6) | (p1 << 5) | (d3 << 4) | (p2 << 3) | (d2 << 2) | (d1 << 1) | d0;
}

std::vector<uint8_t> hamming74_encode(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> out;
    for (uint8_t byte : data) {
        out.push_back(hamming74_encode_nibble(byte >> 4));
        out.push_back(hamming74_encode_nibble(byte & 0xF));
    }
    return out;
}

// Helper: decode a single 7-bit codeword to 4 bits, correct single-bit errors
static uint8_t hamming74_decode_codeword(uint8_t codeword, bool& had_error) {
    // Extract bits
    uint8_t p0 = (codeword >> 6) & 1;
    uint8_t p1 = (codeword >> 5) & 1;
    uint8_t d3 = (codeword >> 4) & 1;
    uint8_t p2 = (codeword >> 3) & 1;
    uint8_t d2 = (codeword >> 2) & 1;
    uint8_t d1 = (codeword >> 1) & 1;
    uint8_t d0 = (codeword >> 0) & 1;
    // Syndrome
    uint8_t s0 = p0 ^ d3 ^ d2 ^ d0;
    uint8_t s1 = p1 ^ d3 ^ d1 ^ d0;
    uint8_t s2 = p2 ^ d2 ^ d1 ^ d0;
    uint8_t syndrome = (s0 << 2) | (s1 << 1) | s2;
    if (syndrome) {
        // Correct single-bit error
        had_error = true;
        int bit = 7 - syndrome; // 1-based to 0-based
        codeword ^= (1 << bit);
        // Re-extract after correction
        p0 = (codeword >> 6) & 1;
        p1 = (codeword >> 5) & 1;
        d3 = (codeword >> 4) & 1;
        p2 = (codeword >> 3) & 1;
        d2 = (codeword >> 2) & 1;
        d1 = (codeword >> 1) & 1;
        d0 = (codeword >> 0) & 1;
    }
    return (d3 << 3) | (d2 << 2) | (d1 << 1) | d0;
}

std::vector<uint8_t> hamming74_decode(const std::vector<uint8_t>& codewords, bool& had_error) {
    std::vector<uint8_t> out;
    had_error = false;
    if (codewords.size() % 2 != 0) throw std::runtime_error("Hamming74: codeword length must be even");
    for (size_t i = 0; i < codewords.size(); i += 2) {
        uint8_t hi = hamming74_decode_codeword(codewords[i], had_error);
        uint8_t lo = hamming74_decode_codeword(codewords[i+1], had_error);
        out.push_back((hi << 4) | lo);
    }
    return out;
}
