#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

struct ParsedLine {
    std::string mnemonic;
    std::vector<std::string> operands;
};

inline std::string trim(const std::string& s) {
    const std::string whitespace = " \t\r\n";
    size_t start = s.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(whitespace);
    return s.substr(start, end - start + 1);
}

inline std::string upperCase(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return s;
}

inline ParsedLine preprocess(std::string line) {
    ParsedLine result;
    size_t comment = line.find(';');
    if (comment != std::string::npos) line = line.substr(0, comment);
    line = trim(line);
    if (line.empty()) return result;

    // Ignore section declarations and data declarations for instruction checking.
    std::string upper = upperCase(line);
    if (upper.rfind("SECTION", 0) == 0 || upper.rfind("GLOBAL", 0) == 0)
        return result;

    // A label may appear alone or before an instruction, e.g. start: MOV EAX, 1
    size_t colon = line.find(':');
    if (colon != std::string::npos) {
        line = trim(line.substr(colon + 1));
        if (line.empty()) return result;
    }

    // Ignore data declarations in the validation output.
    std::stringstream probe(line);
    std::string first, second;
    probe >> first >> second;
    std::string secondUpper = upperCase(second);
    if (secondUpper == "DB" || secondUpper == "DW" || secondUpper == "DD" ||
        secondUpper == "RESB" || secondUpper == "RESW" || secondUpper == "RESD")
        return result;

    std::stringstream ss(line);
    ss >> result.mnemonic;
    result.mnemonic = upperCase(result.mnemonic);

    std::string rest;
    std::getline(ss, rest);
    rest = trim(rest);
    if (!rest.empty()) {
        std::stringstream ops(rest);
        std::string op;
        while (std::getline(ops, op, ',')) {
            op = trim(op);
            if (!op.empty()) result.operands.push_back(op);
        }
    }
    return result;
}
#endif
