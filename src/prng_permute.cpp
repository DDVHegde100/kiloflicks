// prng_permute.cpp
// Passphrase-based PRNG permutation for channel order
#include "prng_permute.h"
#include <random>
#include <algorithm>
#include <functional>

// Simple hash for passphrase to seed
static uint32_t hash_passphrase(const std::string& pass) {
    uint32_t h = 2166136261u;
    for (char c : pass) {
        h ^= (uint8_t)c;
        h *= 16777619u;
    }
    return h;
}

std::vector<size_t> prng_permutation(size_t n, const std::string& passphrase) {
    std::vector<size_t> perm(n);
    for (size_t i = 0; i < n; ++i) perm[i] = i;
    std::mt19937 rng(hash_passphrase(passphrase));
    std::shuffle(perm.begin(), perm.end(), rng);
    return perm;
}
