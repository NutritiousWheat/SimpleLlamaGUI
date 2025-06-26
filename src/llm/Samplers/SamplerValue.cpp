#include "SamplerValue.h"

#include <stdexcept>

SamplerValue::SamplerValue()
{
    isIntValue = true;
    this->value.intValue = 0;
}

SamplerValue::SamplerValue(int value)
{
    isIntValue = true;
    this->value.intValue = value;
}

SamplerValue::SamplerValue(float value)
{
    isIntValue = false;
    this->value.floatValue = value;
}

SamplerValue::SamplerValue(const SamplerValue &value)
{
    this->isIntValue = value.isIntValue;
    this->value = value.value;
}

SamplerValue &SamplerValue::operator=(int value)
{
    isIntValue = true;
    this->value.intValue = value;

    return *this;
}

SamplerValue &SamplerValue::operator=(float value)
{
    isIntValue = false;
    this->value.floatValue = value;

    return *this;
}

SamplerValue::operator int() const
{
    if (!this->isIntValue)
        throw std::runtime_error("value is not an integer");
    return value.intValue;
}

SamplerValue::operator float() const
{
    if (this->isIntValue)
        throw std::runtime_error("value is not a float");

    return value.floatValue;
}

bool SamplerValue::isInt() const
{
    return this->isIntValue;
}
