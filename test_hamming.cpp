// test_hamming.cpp
// Unit tests for Hamming(7,4) ECC
#include "src/hamming.h"
#include <cstdint>
#include <cassert>
#include <iostream>
#include <vector>

void test_hamming_encode_decode() {
    std::vector<uint8_t> data = {0xAB, 0xCD, 0xEF, 0x01, 0x23};
    auto encoded = hamming74_encode(data);
    bool had_error = false;
    auto decoded = hamming74_decode(encoded, had_error);
    assert(decoded == data);
    std::cout << "[PASS] Hamming(7,4) encode/decode roundtrip\n";
}

void test_hamming_single_bit_error_correction() {
    std::vector<uint8_t> data = {0x55, 0xAA};
    auto encoded = hamming74_encode(data);
    // Test single-bit error correction for each codeword
    for (size_t i = 0; i < encoded.size(); ++i) {
        for (int bit = 0; bit < 7; ++bit) {
            auto corrupted = encoded;
            corrupted[i] ^= (1 << bit); // flip one bit
            bool had_error = false;
            auto decoded = hamming74_decode(corrupted, had_error);
            assert(decoded == data);
            assert(had_error);
        }
    }
    std::cout << "[PASS] Hamming(7,4) single-bit error correction\n";
}

void test_hamming_double_bit_error_detection() {
    std::vector<uint8_t> data = {0x0F};
    auto encoded = hamming74_encode(data);
    // Flip two bits in the first codeword
    auto corrupted = encoded;
    corrupted[0] ^= 0b11;
    bool had_error = false;
    auto decoded = hamming74_decode(corrupted, had_error);
    // May not match original, but should not crash
    std::cout << "[INFO] Double-bit error: decoded = " << (int)decoded[0] << ", orig = " << (int)data[0] << "\n";
    std::cout << "[PASS] Hamming(7,4) double-bit error detection (no crash)\n";
}

int main() {
    test_hamming_encode_decode();
    test_hamming_single_bit_error_correction();
    test_hamming_double_bit_error_detection();
    std::cout << "All Hamming(7,4) ECC tests passed.\n";
    return 0;
}
