#include "ConfigBase.h"

#include <QJsonDocument>

QJsonDocument ConfigBase::readFile()
{
    QJsonDocument document = QJsonDocument::fromJson(this->configPath.c_str());
    if (document.isNull())
    {
        throw std::runtime_error("config::verifyConfig: Failed to read config file");
    }
    return document;
}

ConfigBase::ConfigBase(const std::string &config_path)
{
    this->configPath = config_path;
    readFile();
}

QString ConfigBase::keyToString(KeyIntE key)
{

}

QString ConfigBase::keyToString(KeyFloatE key)
{

}

QString ConfigBase::keyToString(KeyStringE key)
{

}

QString ConfigBase::keyToString(KeyBoolE key)
{

}

void ConfigBase::setValue(KeyIntE key, int value)
{
    QJsonDocument document = readFile();
    // set value
}

int ConfigBase::getValue(KeyIntE key)
{
    QJsonDocument document = readFile();
    // set value
}

void ConfigBase::setValue(KeyFloatE key, float value)
{

}

float ConfigBase::getValue(KeyFloatE key)
{

}

void ConfigBase::setValue(KeyStringE key, const QString &value)
{

}

QString ConfigBase::getValue(KeyStringE key)
{

}

void ConfigBase::setValue(KeyBoolE key, bool value)
{

}

bool ConfigBase::getValue(KeyBoolE key)
{

}
