// prng_permute.h
// Passphrase-based PRNG permutation for channel order
#pragma once
#include <string>
#include <vector>
#include <cstdint>

// Generates a permutation of indices [0, n) using a passphrase-based PRNG
std::vector<size_t> prng_permutation(size_t n, const std::string& passphrase);
