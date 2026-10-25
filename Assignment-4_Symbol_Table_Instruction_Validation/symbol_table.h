#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "preprocessor.h"

struct Symbol {
    std::string symbolName;
    int size;
    int location;
    std::string section;
};

class SymbolTable {
    std::vector<Symbol> symbols;

    int directiveSize(const std::string& directive) const {
        if (directive == "DB" || directive == "RESB") return 1;
        if (directive == "DW" || directive == "RESW") return 2;
        if (directive == "DD" || directive == "RESD") return 4;
        return 0;
    }

public:
    bool build(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) return false;
        symbols.clear();

        std::string line, section;
        int dataLocation = 0, bssLocation = 0, textLocation = 0;

        while (std::getline(file, line)) {
            size_t comment = line.find(';');
            if (comment != std::string::npos) line = line.substr(0, comment);
            line = trim(line);
            if (line.empty()) continue;

            std::string upper = upperCase(line);
            if (upper == "SECTION .DATA") { section = ".data"; continue; }
            if (upper == "SECTION .BSS")  { section = ".bss"; continue; }
            if (upper == "SECTION .TEXT") { section = ".text"; continue; }

            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string label = trim(line.substr(0, colon));
                if (!label.empty()) {
                    int loc = (section == ".data") ? dataLocation :
                              (section == ".bss") ? bssLocation : textLocation;
                    symbols.push_back({label, 0, loc, section});
                }
                line = trim(line.substr(colon + 1));
                if (line.empty()) continue;
            }

            std::stringstream ss(line);
            std::string name, directive, value;
            ss >> name >> directive;
            directive = upperCase(directive);
            int unit = directiveSize(directive);
            if (unit == 0 || (section != ".data" && section != ".bss")) {
                // Approximate text location counter for recognized instructions.
                if (section == ".text" && !name.empty()) textLocation += 1;
                continue;
            }

            int size = unit;
            if (directive == "RESB" || directive == "RESW" || directive == "RESD") {
                ss >> value;
                int count = 0;
                try { count = std::stoi(value); } catch (...) { count = 0; }
                if (count < 0) count = 0;
                size = unit * count;
            } else {
                // DB/DW/DD declarations: count comma-separated values approximately.
                std::string rest;
                std::getline(ss, rest);
                rest = trim(rest);
                if (!rest.empty()) {
                    int items = 1;
                    for (char c : rest) if (c == ',') ++items;
                    size = unit * items;
                }
            }

            int loc = (section == ".data") ? dataLocation : bssLocation;
            symbols.push_back({name, size, loc, section});
            if (section == ".data") dataLocation += size;
            else bssLocation += size;
        }
        return true;
    }

    bool contains(const std::string& name) const {
        const std::string wanted = upperCase(trim(name));
        for (const Symbol& s : symbols) {
            if (upperCase(s.symbolName) == wanted) return true;
        }
        return false;
    }

    void print() const {
        std::cout << "\n================ SYMBOL TABLE ================\n";
        std::cout << "Symbol\t\tSize\tLocation\tSection\n";
        std::cout << "------------------------------------------------\n";
        for (const Symbol& s : symbols) {
            std::cout << s.symbolName << "\t\t";
            if (s.size == 0) std::cout << "-\t";
            else std::cout << s.size << "\t";
            std::cout << s.location << "\t\t" << s.section << "\n";
        }
        std::cout << "===============================================\n";
    }
};
#endif
