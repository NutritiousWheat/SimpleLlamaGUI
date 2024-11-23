#ifndef RNG_H
#define RNG_H

#include <random>

class RNG
{
    std::random_device dev;
    std::mt19937 rng;
    std::uniform_int_distribution<std::mt19937::result_type> dist;
public:
    RNG();
    ~RNG() = default;
    uint32_t getRand();
};

#endif // RNG_H
