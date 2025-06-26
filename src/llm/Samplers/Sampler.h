#ifndef SAMPLERS_H
#define SAMPLERS_H

#include <QMap>
#include <QObject>

#include "SamplerValue.h"

class Sampler : public QObject
{
    Q_OBJECT
public:
    enum Type {
        SAMPLER_TEMP = 0,
        SAMPLER_TOP_K,
        SAMPLER_TOP_P,
        SAMPLER_MIN_P,

        SAMPLERS_COUNT
    };

    struct Range
    {
        SamplerValue min;
        SamplerValue max;
    };

private:
    Type type;
    SamplerValue value = SamplerValue(0.0f);

    static QMap<Type, QString> nameMap;

public:
    Sampler();
    explicit Sampler(Type type);
    Sampler(const Sampler &newSampler);
    ~Sampler() = default;

    Sampler &operator=(const Sampler &newSampler);

    [[nodiscard]] bool isInt() const;

    Type getType() const;

    void setValue(int value);
    void setValue(float value);


    SamplerValue getValue() const;
    SamplerValue getDefaultValue() const;
    Range getRange() const;

    QString getName();

};

#endif //SAMPLERS_H
