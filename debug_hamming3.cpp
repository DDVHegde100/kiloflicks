// debug_hamming3.cpp
// Debug the Hamming encoding to understand the syndrome table
#include "src/hamming.h"
#include <cstdint>
#include <iostream>
#include <vector>
#include <iomanip>
#include <bitset>

// Debug version that shows encoding process
uint8_t debug_hamming74_encode_nibble(uint8_t nibble) {
    std::cout << "Encoding nibble: 0x" << std::hex << (int)nibble 
              << " (0b" << std::bitset<4>(nibble) << ")" << std::endl;
    
    uint8_t d0 = (nibble >> 0) & 1;
    uint8_t d1 = (nibble >> 1) & 1;
    uint8_t d2 = (nibble >> 2) & 1;
    uint8_t d3 = (nibble >> 3) & 1;
    
    std::cout << "Data bits: d3=" << (int)d3 << " d2=" << (int)d2 
              << " d1=" << (int)d1 << " d0=" << (int)d0 << std::endl;
    
    uint8_t p0 = d3 ^ d2 ^ d0;
    uint8_t p1 = d3 ^ d1 ^ d0;
    uint8_t p2 = d2 ^ d1 ^ d0;
    
    std::cout << "Parity bits: p0=" << (int)p0 << " p1=" << (int)p1 
              << " p2=" << (int)p2 << std::endl;
    
    uint8_t codeword = (p0 << 6) | (p1 << 5) | (d3 << 4) | (p2 << 3) | (d2 << 2) | (d1 << 1) | d0;
    std::cout << "Codeword: 0x" << std::hex << (int)codeword 
              << " (0b" << std::bitset<7>(codeword) << ")" << std::endl;
    
    return codeword;
}

void test_syndrome_table() {
    std::cout << "=== Testing syndrome table ===" << std::endl;
    
    // Encode nibble 0x5 (original data)
    uint8_t original = debug_hamming74_encode_nibble(0x5);
    std::cout << "\n--- Original encoding complete ---\n" << std::endl;
    
    // Test what happens when we flip each bit
    for (int bit = 0; bit < 7; bit++) {
        std::cout << "Testing bit " << bit << " flip:" << std::endl;
        uint8_t corrupted = original ^ (1 << bit);
        std::cout << "Corrupted: 0x" << std::hex << (int)corrupted 
                  << " (0b" << std::bitset<7>(corrupted) << ")" << std::endl;
        
        // Calculate syndrome manually
        uint8_t p0 = (corrupted >> 6) & 1;
        uint8_t p1 = (corrupted >> 5) & 1;
        uint8_t d3 = (corrupted >> 4) & 1;
        uint8_t p2 = (corrupted >> 3) & 1;
        uint8_t d2 = (corrupted >> 2) & 1;
        uint8_t d1 = (corrupted >> 1) & 1;
        uint8_t d0 = (corrupted >> 0) & 1;
        
        uint8_t s0 = p0 ^ d3 ^ d2 ^ d0;
        uint8_t s1 = p1 ^ d3 ^ d1 ^ d0;
        uint8_t s2 = p2 ^ d2 ^ d1 ^ d0;
        uint8_t syndrome = (s0 << 2) | (s1 << 1) | s2;
        
        std::cout << "Syndrome: " << (int)syndrome << " (should point to bit " << bit << ")" << std::endl;
        std::cout << std::endl;
    }
}

int main() {
    test_syndrome_table();
    return 0;
}
