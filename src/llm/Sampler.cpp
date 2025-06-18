#include "Sampler.h"

#include <stdexcept>
#include <QMap>

float Sampler::floatMultiplier = 100.f;

QMap<QString, Sampler::SamplerTypeE> Sampler::nameMap
        = {{"Temperature", SAMPLER_TEMP}, // TODO: UI shouldn't get strings from this
           {"Top K", SAMPLER_TOP_K},
           {"Top P", SAMPLER_TOP_P},
           {"Min P", SAMPLER_MIN_P}};

Sampler::Sampler()
{
    this->type = SAMPLERS_COUNT; // invalidates the empty sampler
    this->value = 0;
}

Sampler::Sampler(const SamplerTypeE type)
{
    this->type = type;
    if (type == SAMPLER_TOP_K) // TODO: store type in sampler
        this->setValueInt(getDefaultValueInt());
    else
        this->setValueFloat(getDefaultValueFloat());
}

Sampler::Sampler(const Sampler &new_sampler)
{
    this->type = new_sampler.type;
    this->value = new_sampler.value;
}

Sampler &Sampler::operator=(const Sampler &new_sampler)
{
    this->type = new_sampler.type;
    this->value = new_sampler.value;
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

void Sampler::setValueInt(const int value)
{
    this->value = value;
}

void Sampler::setValueFloat(const float value)
{
    if (isInt()) {
        this->value = value;
    }

    this->value = value * floatMultiplier;
}

int Sampler::getValueInt() const
{
    return this->value;
}

float Sampler::getValueFloat() const
{
    if (isInt()) {
        return value;
    }
    return this->value / floatMultiplier;
}

int Sampler::getDefaultValueInt() const
{
    switch (this->type) {
    case SAMPLER_TOP_K:
        return 0;
    case SAMPLER_TEMP:
    case SAMPLER_TOP_P:
    case SAMPLER_MIN_P:
        throw std::invalid_argument("Invalid Value type");
    default:
        throw std::invalid_argument("Invalid Sampler type");
    }
}

float Sampler::getDefaultValueFloat() const
{
    switch (this->type) {
    case SAMPLER_TEMP:
        return 1.0f;
    case SAMPLER_TOP_K:
        throw std::invalid_argument("Invalid Value type");
    case SAMPLER_TOP_P:
        return 1.0f;
    case SAMPLER_MIN_P:
        return 0.0f;
    default:
        throw std::invalid_argument("Invalid Sampler type"); // TODO: use custom exception types
    }
}

Sampler::SamplerRangeIntT Sampler::getRangeInt() const
{
    SamplerRangeFloatT rangeFloat;
    SamplerRangeIntT rangeFloatScaled = {};
    if (!isInt()) {
        rangeFloat = getRangeFloat();
        rangeFloatScaled.min = rangeFloat.min * floatMultiplier;
        rangeFloatScaled.max = rangeFloat.max * floatMultiplier;
    }

    switch (this->type) {
    case SAMPLER_TEMP:
        return rangeFloatScaled;
    case SAMPLER_TOP_K:
        return {-1, 200};
    case SAMPLER_TOP_P:
        return rangeFloatScaled;
    case SAMPLER_MIN_P:
        return rangeFloatScaled;
    default:
        throw std::invalid_argument("Invalid Sampler type");
    }
}

Sampler::SamplerRangeFloatT Sampler::getRangeFloat() const
{
    SamplerRangeIntT rangeInt;
    SamplerRangeFloatT rangeIntFloatifieded = {};
    if (isInt()) {
        rangeInt = getRangeInt();
        rangeIntFloatifieded.min = rangeInt.min;
        rangeIntFloatifieded.max = rangeInt.max;
    }

    switch (this->type) {
    case SAMPLER_TEMP:
        return {0.0f, 5.0f};
    case SAMPLER_TOP_K:
        return rangeIntFloatifieded;
    case SAMPLER_TOP_P:
        return {0.0f, 1.0f};
    case SAMPLER_MIN_P:
        return {0.0f, 1.0f};
    default:
        throw std::invalid_argument("Invalid Sampler type");
    }
}

Sampler::SamplerTypeE Sampler::stringToSamplerType(const QString &type)
{
    if (nameMap.keys().contains(type)) {
        return nameMap.value(type);
    }

    throw std::invalid_argument("Invalid Sampler type");
}

QString Sampler::samplerTypeToString(SamplerTypeE type)
{
    if (nameMap.values().contains(type)) {
        return nameMap.key(type);
    }

    throw std::invalid_argument("Invalid Sampler type");
}
