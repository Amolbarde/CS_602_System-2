#ifndef MNEMONICS_VALIDATOR_H
#define MNEMONICS_VALIDATOR_H

#include <fstream>
#include <string>
#include <set>
#include <sstream>
#include "preprocessor.h"

class MnemonicValidator {
    std::set<std::string> mnemonics;
public:
    bool loadOpcodeFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) return false;
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;
            std::stringstream ss(line);
            std::string mnemonic;
            ss >> mnemonic;
            if (!mnemonic.empty()) mnemonics.insert(upperCase(mnemonic));
        }
        return true;
    }

    bool isValid(const std::string& mnemonic) const {
        return mnemonics.find(upperCase(mnemonic)) != mnemonics.end();
    }
};
#endif
