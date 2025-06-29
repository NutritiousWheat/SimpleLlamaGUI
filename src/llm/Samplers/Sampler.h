#ifndef SAMPLERS_H
#define SAMPLERS_H

#include <QMap>
#include <QObject>

#include "SamplerValue.h"

class Sampler : public QObject
{
    Q_OBJECT
public:
    enum SamplerType {
        SAMPLER_TEMP = 0,
        SAMPLER_TOP_K,
        SAMPLER_TOP_P,
        SAMPLER_MIN_P,

        SAMPLERS_COUNT
    };

    struct SamplerRange
    {
        SamplerValue min;
        SamplerValue max;
    };

private:
    static QMap<SamplerType, QString> names;
    static QMap<SamplerType, SamplerValue> defaultValues;
    static QMap<SamplerType, SamplerRange> ranges;
    static QMap<SamplerType, bool> intSampler;

    SamplerType type;
    SamplerRange range;
    SamplerValue value = SamplerValue(0.0f);

public:
    Sampler();
    explicit Sampler(SamplerType type);
    Sampler(const Sampler &newSampler);
    ~Sampler() = default;

    static bool isInt(SamplerType type);

    Sampler &operator=(const Sampler &newSampler);

    [[nodiscard]] bool isInt() const;
    void setValue(int value);
    void setValue(float value);

    [[nodiscard]] SamplerValue getValue() const;
    [[nodiscard]] SamplerType getType() const;
    [[nodiscard]] SamplerRange getRange() const;
    QString getName();

};

#endif //SAMPLERS_H
