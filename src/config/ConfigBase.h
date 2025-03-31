#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <qjsondocument.h>

class ConfigBase {
    /*
     * model directory location
     * sampler preset directory location
     * last used model
     *
     * per model:
     * sampler params + last used preset
     * loading params
     * prompt
     * prompt preset (from llama.cpp presets)
     *
     */
protected:
    std::string configPath; #warning replace all std::string with QString
    QJsonDocument readFile();

public:
    enum KeyIntE
    {
        test
    };
    enum KeyFloatE
    {

    };
    enum KeyStringE
    {

    };
    enum KeyBoolE
    {

    };

    explicit ConfigBase(const std::string &config_path);
    virtual ~ConfigBase() = default;

    virtual QString keyToString(KeyIntE key);
    virtual QString keyToString(KeyFloatE key);
    virtual QString keyToString(KeyStringE key);
    virtual QString keyToString(KeyBoolE key);

    void setValue(KeyIntE key, int value);
    int getValue(KeyIntE key);

    void setValue(KeyFloatE key, float value);
    float getValue(KeyFloatE key);

    void setValue(KeyStringE key, const QString &value);
    QString getValue(KeyStringE key);

    void setValue(KeyBoolE key, bool value);
    bool getValue(KeyBoolE key);

};



#endif //CONFIG_H
