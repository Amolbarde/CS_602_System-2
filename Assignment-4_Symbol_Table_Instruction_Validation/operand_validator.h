#ifndef OPERAND_VALIDATOR_H
#define OPERAND_VALIDATOR_H

#include <string>
#include <fstream>
#include <cctype>
#include "register_validator.h"
#include "symbol_table.h"

class OperandValidator {
    RegisterValidator registerValidator;

    bool isInteger(const std::string& value) const {
        if (value.empty()) return false;
        std::size_t i = (value[0] == '+' || value[0] == '-') ? 1 : 0;
        if (i == value.size()) return false;
        for (; i < value.size(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(value[i]))) return false;
        }
        return true;
    }

public:
    bool loadRegisters(const std::string& filename) {
        return registerValidator.loadRegisters(filename);
    }

    std::string getOperandType(std::string operand) const {
        operand = trim(operand);
        if (registerValidator.isRegister(operand)) return "REGISTER";
        if (operand.size() >= 2 && operand.front() == '[' && operand.back() == ']')
            return "MEMORY";
        if (isInteger(operand)) return "CONSTANT";
        if (!operand.empty() &&
            (std::isalpha(static_cast<unsigned char>(operand[0])) || operand[0] == '_'))
            return "SYMBOL";
        return "UNKNOWN";
    }

    bool isKnownOperand(const std::string& operand, const SymbolTable& symbols) const {
        std::string value = trim(operand);
        if (getOperandType(value) == "SYMBOL") return symbols.contains(value);

        // For [symbol] and [register], check that the contents are recognizable.
        if (value.size() >= 2 && value.front() == '[' && value.back() == ']') {
            std::string inside = trim(value.substr(1, value.size() - 2));
            if (inside.empty()) return false;
            if (registerValidator.isRegister(inside) || isInteger(inside)) return true;
            return symbols.contains(inside);
        }
        return getOperandType(value) != "UNKNOWN";
    }
};
#endif
