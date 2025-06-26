#include "SamplerArray.h"

SamplerArray::SamplerArray()
{
    for (int i = 0; i < Sampler::SAMPLERS_COUNT; i++) {
        this->samplers[i] = Sampler(static_cast<Sampler::Type>(i));
    }
}

Sampler &SamplerArray::operator[](Sampler::Type type) {
    return this->samplers[static_cast<int>(type)];
}
