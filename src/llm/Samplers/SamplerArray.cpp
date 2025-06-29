#include "SamplerArray.h"

SamplerArray::SamplerArray()
{
    for (int i = 0; i < Sampler::SAMPLERS_COUNT; i++) {
        this->samplers[i] = Sampler(static_cast<Sampler::SamplerType>(i));
    }
}

Sampler &SamplerArray::operator[](Sampler::SamplerType type) {
    return this->samplers[static_cast<int>(type)];
}
