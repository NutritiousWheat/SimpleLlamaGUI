#ifndef SAMPLERVALUE_H
#define SAMPLERVALUE_H

class SamplerValue {
    union ValueType
    {
        int intValue;
        float floatValue;
    };

    bool isIntValue;
    ValueType value{};

public:
    SamplerValue();
    explicit SamplerValue(int value);
    explicit SamplerValue(float value);
    SamplerValue(const SamplerValue &value);

    SamplerValue &operator=(int value);
    SamplerValue &operator=(float value);
    SamplerValue &operator=(const SamplerValue &value) = default;

    explicit operator int() const;
    explicit operator float() const;

    [[nodiscard]] bool isInt() const;
};

#endif //SAMPLERVALUE_H
