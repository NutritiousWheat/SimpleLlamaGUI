#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <qjsondocument.h>

class config {
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
private:
    std::string config_path;
    QJsonDocument read_file();

public:
    explicit config(const std::string &config_path);
    config();
    void set_value();
    void get_value();
};



#endif //CONFIG_H
