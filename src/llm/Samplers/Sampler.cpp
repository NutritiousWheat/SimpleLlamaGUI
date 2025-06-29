#include "Sampler.h"

#include <stdexcept>
#include <QMap>

QMap<Sampler::SamplerType, QString> Sampler::names
    = {{SAMPLER_TEMP, "Temperature"}, // TODO: UI shouldn't get strings from this
       {SAMPLER_TOP_K, "Top K"},
       {SAMPLER_TOP_P, "Top P"},
       {SAMPLER_MIN_P, "Min P"},

        {SAMPLERS_COUNT, "Unknown Sampler"}};

QMap<Sampler::SamplerType, SamplerValue> Sampler::defaultValues
    = {{SAMPLER_TEMP, SamplerValue(0.0f)},
       {SAMPLER_TOP_K, SamplerValue(0)},
       {SAMPLER_TOP_P, SamplerValue(1.0f)},
       {SAMPLER_MIN_P, SamplerValue(0.0f)},

        {SAMPLERS_COUNT, SamplerValue(0)}};

QMap<Sampler::SamplerType, Sampler::SamplerRange> Sampler::ranges
    = {{SAMPLER_TEMP, {SamplerValue(0.0f), SamplerValue(5.0f)}},
       {SAMPLER_TOP_K, {SamplerValue(-1), SamplerValue(200)}},
       {SAMPLER_TOP_P, {SamplerValue(0.0f), SamplerValue(1.0f)}},
       {SAMPLER_MIN_P, {SamplerValue(0.0f), SamplerValue(1.0f)}},

        {SAMPLERS_COUNT, {SamplerValue(0), SamplerValue(0)}}};

QMap<Sampler::SamplerType, bool> Sampler::intSampler
    = {{SAMPLER_TEMP, false},
       {SAMPLER_TOP_K, true},
       {SAMPLER_TOP_P, false},
       {SAMPLER_MIN_P, false},

        {SAMPLERS_COUNT, true}};

Sampler::Sampler()
{
    this->type = SAMPLERS_COUNT; // invalidates the empty sampler
    this->value = 0;
    this->range = {SamplerValue(0), SamplerValue(0)};
}

Sampler::Sampler(const SamplerType type)
{
    this->type = type;
    this->value = defaultValues[type];
    this->range = ranges[type];
}

Sampler::Sampler(const Sampler &newSampler)
{
    this->type = newSampler.type;
    this->value = newSampler.value;
    this->range = newSampler.range;
}

Sampler &Sampler::operator=(const Sampler &newSampler)
{
    this->type = newSampler.type;
    this->value = newSampler.value;
    this->range = newSampler.range;
    return *this;
}

bool Sampler::isInt(const SamplerType type)
{
    switch (type) {
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

bool Sampler::isInt() const
{
    return isInt(this->type);
}

Sampler::SamplerType Sampler::getType() const
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

Sampler::SamplerRange Sampler::getRange() const
{
    return this->range;
}

QString Sampler::getName()
{
    return names.value(type);
}
