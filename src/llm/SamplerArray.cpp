#include "SamplerArray.h"

SamplerArray::SamplerArray()
{
    for (int i = 0; i < Sampler::SAMPLERS_COUNT; i++) {
        this->samplers[i] = Sampler(static_cast<Sampler::SamplerTypeE>(i));
    }
}

Sampler &SamplerArray::operator[](Sampler::SamplerTypeE type) {
    return this->samplers[static_cast<int>(type)];
}
