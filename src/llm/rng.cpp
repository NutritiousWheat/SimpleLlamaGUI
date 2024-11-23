#include "rng.h"

RNG::RNG()
{
    this->rng = std::mt19937(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 0xFFFFFFFF);
}

uint32_t RNG::getRand()
{
    return dist(rng);
}
