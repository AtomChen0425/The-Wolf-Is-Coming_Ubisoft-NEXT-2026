#pragma once
#include <cstdlib>

inline float Rand01() { return float(std::rand()) / float(RAND_MAX); }
inline int RandInt(int lo, int hi) { return lo + (std::rand() % (hi - lo + 1)); }