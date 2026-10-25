#ifndef REGISTER_VALIDATOR_H
#define REGISTER_VALIDATOR_H

#include <fstream>
#include <string>
#include <set>
#include "preprocessor.h"

class RegisterValidator {
    std::set<std::string> registers;
public:
    bool loadRegisters(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) return false;
        std::string reg;
        while (file >> reg) registers.insert(upperCase(reg));
        return true;
    }

    bool isRegister(const std::string& value) const {
        return registers.find(upperCase(trim(value))) != registers.end();
    }
};
#endif
