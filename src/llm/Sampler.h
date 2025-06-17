#ifndef SAMPLERS_H
#define SAMPLERS_H

#include <QObject>
#include <QMap>

// TODO: rewrite this garbage

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

    struct SamplerRangeIntT
    {
        int min;
        int max;
    };

    struct SamplerRangeFloatT
    {
        float min;
        float max;
    };

private:
    SamplerTypeE type;
    int value = 0;

    static QMap<QString, SamplerTypeE> nameMap;
    static float floatMultiplier;

public:
    Sampler();
    explicit Sampler(SamplerTypeE type);
    explicit Sampler(const QString& type);
    Sampler(const Sampler &new_sampler);
    ~Sampler() = default;

    Sampler &operator=(const Sampler &new_sampler);

    [[nodiscard]] bool isInt() const;

    SamplerTypeE getType() const;

    void setValueInt(int value);
    void setValueFloat(float value);

    int getValueInt() const;
    float getValueFloat() const;

    int getDefaultValueInt() const;
    float getDefaultValueFloat() const;

    SamplerRangeIntT getRangeInt() const;
    SamplerRangeFloatT getRangeFloat() const;

    static SamplerTypeE stringToSamplerType(const QString& type);
    static QString samplerTypeToString(SamplerTypeE type);

};

#endif //SAMPLERS_H
