// debug_hamming.cpp
// Debug the Hamming(7,4) ECC implementation
#include "src/hamming.h"
#include <cstdint>
#include <iostream>
#include <vector>
#include <iomanip>
#include <bitset>

void debug_single_bit_error() {
    std::vector<uint8_t> data = {0x55, 0xAA}; // Test case from failing test
    std::cout << "Original data: ";
    for (auto d : data) {
        std::cout << "0x" << std::hex << (int)d << " ";
    }
    std::cout << std::endl;
    
    auto encoded = hamming74_encode(data);
    std::cout << "Encoded codewords: ";
    for (auto c : encoded) {
        std::cout << "0x" << std::hex << (int)c << " (0b" << std::bitset<7>(c) << ") ";
    }
    std::cout << std::endl;
    
    // Test single-bit error correction for each codeword
    for (size_t i = 0; i < encoded.size(); ++i) {
        for (int bit = 0; bit < 7; ++bit) {
            auto corrupted = encoded;
            corrupted[i] ^= (1 << bit); // flip one bit
            std::cout << "Testing codeword " << i << ", bit " << bit 
                      << " - corrupted: 0x" << std::hex << (int)corrupted[i] << std::endl;
            bool had_error = false;
            auto decoded = hamming74_decode(corrupted, had_error);
            std::cout << "  Decoded: ";
            for (auto d : decoded) {
                std::cout << "0x" << std::hex << (int)d << " ";
            }
            std::cout << " error=" << had_error << " match=" << (decoded == data) << std::endl;
            if (!(decoded == data)) {
                std::cout << "  FAILURE at codeword " << i << ", bit " << bit << std::endl;
                return;
            }
        }
    }
    std::cout << "[PASS] All single-bit errors corrected successfully" << std::endl;
}

int main() {
    debug_single_bit_error();
    return 0;
}
