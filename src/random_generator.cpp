#include <cstdint>

inline uint64_t random(uint64_t& seed){
    seed = seed * 6364136223846793005ULL + 1;
    return (seed >> 16);
}