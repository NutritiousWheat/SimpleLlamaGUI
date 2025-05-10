#ifndef CONFIG_H
#define CONFIG_H

#include <QJsonObject>

class ConfigApp {
    /*
     * per model:
     * sampler params + last used preset
     * loading params
     * prompt
     * prompt preset (from llama.cpp presets)
     *
     */
private:
    const QString defaultModelDir = "models";
    const QString defaultModelConfigDir = "modelConfig";
    const QString defaultSamplerPresetDir = "samplerPreset";
    const QString defaultLastUsedModel= "";
    const bool defaulAutoLoadLastOnStart= false;


    QString configPath;
    QJsonObject configObject;
    void initFile();
    void readFile();
    void writeFile();
    void initDir(QString path);

public:
    enum KeyIntE
    {

    };
    enum KeyDoubleE
    {

    };
    enum KeyStringE
    {
        ModelDir,
        ModelConfigDir,
        SamplerPresetDir,
        LastUsedModel
    };
    enum KeyBoolE
    {
        AutoLoadLastOnStart
    };

    explicit ConfigApp(const QString &configPath);
    ~ConfigApp() = default;

    QString keyToString(KeyIntE key);
    QString keyToString(KeyDoubleE key);
    QString keyToString(KeyStringE key);
    QString keyToString(KeyBoolE key);

    void setValue(KeyIntE key, int value);
    int getValue(KeyIntE key);

    void setValue(KeyDoubleE key, double value);
    float getValue(KeyDoubleE key);

    void setValue(KeyStringE key, const QString &value);
    QString getValue(KeyStringE key);

    void setValue(KeyBoolE key, bool value);
    bool getValue(KeyBoolE key);

};



#endif //CONFIG_H
