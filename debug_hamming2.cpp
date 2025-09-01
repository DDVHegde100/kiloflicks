// debug_hamming2.cpp
// Debug the Hamming syndrome calculation
#include "src/hamming.h"
#include <cstdint>
#include <iostream>
#include <vector>
#include <iomanip>
#include <bitset>

// Debug version of hamming74_decode_codeword
uint8_t debug_hamming74_decode_codeword(uint8_t codeword, bool& had_error) {
    had_error = false;
    std::cout << "  Input codeword: 0x" << std::hex << (int)codeword 
              << " (0b" << std::bitset<7>(codeword) << ")" << std::endl;
    
    // Extract bits
    uint8_t p0 = (codeword >> 6) & 1;
    uint8_t p1 = (codeword >> 5) & 1;
    uint8_t d3 = (codeword >> 4) & 1;
    uint8_t p2 = (codeword >> 3) & 1;
    uint8_t d2 = (codeword >> 2) & 1;
    uint8_t d1 = (codeword >> 1) & 1;
    uint8_t d0 = (codeword >> 0) & 1;
    
    std::cout << "  Bits: p0=" << (int)p0 << " p1=" << (int)p1 << " d3=" << (int)d3 
              << " p2=" << (int)p2 << " d2=" << (int)d2 << " d1=" << (int)d1 << " d0=" << (int)d0 << std::endl;
    
    // Syndrome
    uint8_t s0 = p0 ^ d3 ^ d2 ^ d0;
    uint8_t s1 = p1 ^ d3 ^ d1 ^ d0;
    uint8_t s2 = p2 ^ d2 ^ d1 ^ d0;
    uint8_t syndrome = (s0 << 2) | (s1 << 1) | s2;
    
    std::cout << "  Syndrome: s0=" << (int)s0 << " s1=" << (int)s1 << " s2=" << (int)s2 
              << " => " << (int)syndrome << std::endl;
    
    if (syndrome) {
        had_error = true;
        // Syndrome to bit position mapping for this Hamming(7,4) implementation
        static const int syndrome_to_bit[] = {-1, 3, 5, 1, 6, 2, 4, 0}; // index by syndrome
        int bit = syndrome_to_bit[syndrome];
        std::cout << "  Error detected, bit to flip: " << bit << std::endl;
        if (bit >= 0 && bit < 7) {
            codeword ^= (1 << bit);
            std::cout << "  Corrected codeword: 0x" << std::hex << (int)codeword 
                      << " (0b" << std::bitset<7>(codeword) << ")" << std::endl;
            // Re-extract after correction
            p0 = (codeword >> 6) & 1;
            p1 = (codeword >> 5) & 1;
            d3 = (codeword >> 4) & 1;
            p2 = (codeword >> 3) & 1;
            d2 = (codeword >> 2) & 1;
            d1 = (codeword >> 1) & 1;
            d0 = (codeword >> 0) & 1;
        }
    }
    uint8_t result = (d3 << 3) | (d2 << 2) | (d1 << 1) | d0;
    std::cout << "  Decoded nibble: 0x" << std::hex << (int)result << std::endl;
    return result;
}

void debug_specific_case() {
    // Test the failing case: codeword 0, bit 1 flipped
    // Original encoded: 0x25 (0b0100101)
    // Corrupted: 0x27 (0b0100111) - bit 1 flipped
    
    std::cout << "=== Debug specific failing case ===" << std::endl;
    uint8_t corrupted = 0x27;
    bool had_error = false;
    uint8_t result = debug_hamming74_decode_codeword(corrupted, had_error);
    std::cout << "Expected: 0x5, Got: 0x" << std::hex << (int)result << std::endl;
}

int main() {
    debug_specific_case();
    return 0;
}
