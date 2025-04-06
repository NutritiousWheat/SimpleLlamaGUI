#include "Sampler.h"

#include <stdexcept>

Sampler::Sampler()
{
    this->type = SAMPLERS_COUNT; // invalidates the empty sampler
    this->value = {};
}

Sampler::Sampler(const SamplerTypeE type)
{
    this->type = type;

    if (isInt()) {
        this->value.intValue = getDefaultValueInt();
    } else {
        this->value.floatValue = getDefaultValueFloat();
    }
}

Sampler::Sampler(const Sampler &new_sampler)
{
    this->type = new_sampler.type;
    this->value = new_sampler.value;
}

Sampler &Sampler::operator=(const Sampler &new_sampler)
{
    this->type = new_sampler.type;
    this->value.intValue = new_sampler.value.intValue;
    return *this;
}

bool Sampler::isInt() const
{
    switch (this->type) {
    case SAMPLER_TOP_K:
        return true;
    case SAMPLER_TEMP:
    case SAMPLER_TOP_P:
    case SAMPLER_MIN_P:
        return false;
    default:
        throw std::invalid_argument("Invalid Sampler type");
    }
}

Sampler::SamplerTypeE Sampler::getType() const
{
    return this->type;
}

void Sampler::setValueInt(const int32_t value)
{
    if (!isInt()) {
        throw std::invalid_argument("Wrong sampler type");
    }

    this->value.intValue = value;
}

void Sampler::setValueFloat(const float value)
{
    if (isInt()) {
        throw std::invalid_argument("Wrong sampler type");
    }

    this->value.floatValue = value;
}

int32_t Sampler::getValueInt() const
{
    if (!isInt()) {
        throw std::invalid_argument("Wrong sampler type");
    }

    return this->value.intValue;
}

float Sampler::getValueFloat() const
{
    if (isInt()) {
        throw std::invalid_argument("Wrong sampler type");
    }

    return this->value.floatValue;
}

int32_t Sampler::getDefaultValueInt() const
{
    if (!isInt()) {
        throw std::invalid_argument("Wrong sampler type");
    }

    switch (this->type) {
    case SAMPLER_TOP_K:
        return 0;
    default:
        throw std::invalid_argument("Invalid Sampler type");
    }
}

float Sampler::getDefaultValueFloat() const
{
    if (isInt()) {
        throw std::invalid_argument("Wrong sampler type");
    }

    switch (this->type) {
    case SAMPLER_TEMP:
        return 1.0f;
    case SAMPLER_TOP_P:
        return 1.0f;
    case SAMPLER_MIN_P:
        return 0.0f;
    default:
        throw std::invalid_argument("Invalid Sampler type");
    }
}
