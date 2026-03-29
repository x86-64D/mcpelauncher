//
// Created by adrian on 8/15/20.
//

#include "util.h"

bool ReadEnvFlag(const char* name, bool def) {
    auto val = getenv(name);
    if(!val) {
        return def;
    }
    std::string sval = val;
    return sval == "true" || sval == "1" || sval == "on";
}

int ReadEnvInt(const char* name, int def) {
    auto val = getenv(name);
    if(!val) {
        return def;
    }
    std::string sval = val;
    return std::stoi(sval);
}