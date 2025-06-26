#ifndef SAMPLERARRAY_H
#define SAMPLERARRAY_H

#include "Sampler.h"

class SamplerArray {
    Sampler samplers[Sampler::SAMPLERS_COUNT]{};

    public:
    SamplerArray();
    ~SamplerArray() = default;

    Sampler &operator[](Sampler::Type type);
    Sampler &operator=(Sampler &new_sampler);
};



#endif //SAMPLERARRAY_H
