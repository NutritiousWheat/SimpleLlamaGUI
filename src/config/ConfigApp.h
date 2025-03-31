#ifndef CONFIG_APP_H
#define CONFIG_APP_H

#include "ConfigBase.h"

class ConfigApp : public ConfigBase {
public:


    QString keyToString(key_int_e key);
    QString keyToString(key_float_e key);
    QString keyToString(key_string_e key);
    QString keyToString(key_bool_e key);
};



#endif //CONFIG_APP_H
