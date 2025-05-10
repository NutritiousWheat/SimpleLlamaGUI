#include "ConfigApp.h"

#include <QFile>
#include <QJsonDocument>
#include <QDir>

void ConfigApp::initFile()
{
    QFile file(configPath);
    QJsonObject object;

    // string
    object[keyToString(ModelDir)] = defaultModelDir;
    object[keyToString(ModelConfigDir)] = defaultModelConfigDir;
    object[keyToString(SamplerPresetDir)] = defaultSamplerPresetDir;
    object[keyToString(LastUsedModel)] = defaultLastUsedModel;

    // bool
    object[keyToString(AutoLoadLastOnStart)] = defaulAutoLoadLastOnStart;

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(QJsonDocument(object).toJson());
    file.close();
}

void ConfigApp::readFile()
{
    QFile file(configPath);
    QString jsonString;
    QJsonDocument document;
    QJsonObject object;

    if (!file.exists()) {
        initFile();
    }
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    jsonString = file.readAll();
    file.close();
    document = QJsonDocument::fromJson(jsonString.toUtf8());

    if (document.isNull()) {
        throw std::runtime_error("config::verifyConfig: Failed to read config file");
    }

    object = document.object();

    configObject = object;
}

void ConfigApp::writeFile()
{
    QFile file(configPath);
    QJsonDocument document(configObject);
    QString jsonString;

    jsonString = document.toJson();
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(jsonString.toUtf8());
    file.close();

// TODO: implement a delay which is reset afeter every write operation AND ALSO IMPLEMENT MUTEXES
}

void ConfigApp::initDir(QString path)
{
    bool result = true;
    if (!QDir(path).exists()) {
        result = QDir().mkdir(path);
    }
    if (!result) {
        throw std::runtime_error("config::initDir: Failed to create directory");
    }
}

ConfigApp::ConfigApp(const QString &configPath)
{
    this->configPath = configPath;
    readFile();
    initDir(getValue(ModelDir));
    initDir(getValue(ModelConfigDir));
    initDir(getValue(SamplerPresetDir));
}

QString ConfigApp::keyToString(KeyIntE key)
{
    switch (key) {
        default:
            throw std::runtime_error("ConfigApp::keyToString(int): Invalid key");
    }
}

QString ConfigApp::keyToString(KeyDoubleE key)
{
    switch (key) {
        default:
            throw std::runtime_error("ConfigApp::keyToString(double): Invalid key");
    }
}

QString ConfigApp::keyToString(KeyStringE key)
{
    switch (key) {
        case ModelDir:
            return "ModelDir";
        case ModelConfigDir:
            return "ModelConfigDir";
        case SamplerPresetDir:
            return "SamplerPresetDir";
        case LastUsedModel:
            return "LastUsedModel";
        default:
            throw std::runtime_error("ConfigApp::keyToString(string): Invalid key");
    }
}

QString ConfigApp::keyToString(KeyBoolE key)
{
    switch (key) {
        case AutoLoadLastOnStart:
            return "AutoLoadOnStart";
        default:
            throw std::runtime_error("ConfigApp::keyToString(bool): Invalid key");
    }
}

void ConfigApp::setValue(KeyIntE key, int value)
{
    configObject[keyToString(key)] = value;
    writeFile();
}

int ConfigApp::getValue(KeyIntE key)
{
    readFile();
    return configObject[keyToString(key)].toInt();
}

void ConfigApp::setValue(KeyDoubleE key, double value)
{
    configObject[keyToString(key)] = value;
    writeFile();
}

float ConfigApp::getValue(KeyDoubleE key)
{
    readFile();
    return configObject[keyToString(key)].toDouble();
}

void ConfigApp::setValue(KeyStringE key, const QString &value)
{
    configObject[keyToString(key)] = value;
    writeFile();
}

QString ConfigApp::getValue(KeyStringE key)
{
    readFile();
    return configObject[keyToString(key)].toString();
}

void ConfigApp::setValue(KeyBoolE key, bool value)
{
    configObject[keyToString(key)] = value;
    writeFile();
}

bool ConfigApp::getValue(KeyBoolE key)
{
    readFile();
    return configObject[keyToString(key)].toBool();
}
