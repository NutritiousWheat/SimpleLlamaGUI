#include "config.h"

#include <QJsonDocument>

QJsonDocument config::read_file()
{
    QJsonDocument document = QJsonDocument::fromJson(this->config_path.c_str());
    if (document.isNull())
    {
        throw std::runtime_error("config::verifyConfig: Failed to read config file");
    }
    return document;
}

config::config(const std::string &config_path)
{
    this->config_path = config_path;
    read_file();
}

config::config()
{
    this->config_path = config_path;
    read_file();
}

void config::set_value()
{
    QJsonDocument document = read_file();
    // set value
}

void config::get_value()
{
    QJsonDocument document = read_file();
    // set value
}