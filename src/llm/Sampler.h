#ifndef SAMPLERS_H
#define SAMPLERS_H

#include <cstdint>
#include <QObject>

class Sampler : public QObject
{
    Q_OBJECT
public:
    enum SamplerTypeE {
        SAMPLER_TEMP = 0,
        SAMPLER_TOP_K,
        SAMPLER_TOP_P,
        SAMPLER_MIN_P,

        SAMPLERS_COUNT
    };

    Q_ENUM(SamplerTypeE type)
private:
    SamplerTypeE type;
    union {
        float floatValue;
        int32_t intValue;
    } value{};

public:
    Sampler();
    explicit Sampler(SamplerTypeE type);
    Sampler(const Sampler &new_sampler);
    ~Sampler() = default;

    Sampler &operator=(const Sampler &new_sampler);

    [[nodiscard]] bool isInt() const;

    SamplerTypeE getType() const;

    void setValueInt(int32_t value);
    void setValueFloat(float value);

    int32_t getValueInt() const;
    float getValueFloat() const;

    int32_t getDefaultValueInt() const;
    float getDefaultValueFloat() const;
};

#endif //SAMPLERS_H
