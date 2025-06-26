#include "Sampler.h"

#include <stdexcept>
#include <QMap>

QMap<Sampler::Type, QString> Sampler::nameMap
        = {{SAMPLER_TEMP, "Temperature"}, // TODO: UI shouldn't get strings from this
           {SAMPLER_TOP_K, "Top K"},
           {SAMPLER_TOP_P, "Top P"},
           {SAMPLER_MIN_P, "Min P"}};

Sampler::Sampler()
{
    this->type = SAMPLERS_COUNT; // invalidates the empty sampler
    this->value = 0;
}

Sampler::Sampler(const Type type)
{
    this->type = type;
    this->value = getDefaultValue();
}

Sampler::Sampler(const Sampler &newSampler)
{
    this->type = newSampler.type;
    this->value = newSampler.value;
}

Sampler &Sampler::operator=(const Sampler &newSampler)
{
    this->type = newSampler.type;
    this->value = newSampler.value;
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

Sampler::Type Sampler::getType() const
{
    return this->type;
}

void Sampler::setValue(int value)
{
    if (!isInt())
        throw std::invalid_argument("trying to assign 'int' value to a 'float' sampler");

    this->value = value;
}

void Sampler::setValue(float value)
{
    if (isInt())
        throw std::invalid_argument("trying to assign 'float' value to a 'int' sampler");

    this->value = value;
}

SamplerValue Sampler::getValue() const
{
    return this->value;
}

SamplerValue Sampler::getDefaultValue() const
{
    switch (this->type) {
    case SAMPLER_TOP_K:
        return SamplerValue(0);
    case SAMPLER_TEMP:
        return SamplerValue(1.0f);
    case SAMPLER_TOP_P:
        return SamplerValue(1.0f);
    case SAMPLER_MIN_P:
        return SamplerValue(0.0f);
    default:
        throw std::invalid_argument("Invalid Sampler type");
    }
}

Sampler::Range Sampler::getRange() const
{
    switch (this->type) {
    case SAMPLER_TEMP:
        return {SamplerValue(0.0f), SamplerValue(5.0f)};
    case SAMPLER_TOP_K:
        return {SamplerValue(-1), SamplerValue(200)};
    case SAMPLER_TOP_P:
        return {SamplerValue(0.0f), SamplerValue(1.0f)};
    case SAMPLER_MIN_P:
        return {SamplerValue(0.0f), SamplerValue(1.0f)};
    default:
        throw std::invalid_argument("Invalid Sampler type");
    }
}

QString Sampler::getName()
{
    return nameMap.value(type);
}
