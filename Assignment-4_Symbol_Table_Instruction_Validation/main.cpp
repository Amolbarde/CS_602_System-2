#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "preprocessor.h"
#include "mnemonics_validator.h"
#include "operand_validator.h"
#include "symbol_table.h"

using namespace std;

static bool isMemoryType(const string& type) { return type == "MEMORY"; }
static bool isRegOrMem(const string& type) { return type == "REGISTER" || type == "MEMORY"; }

// Validate a useful educational subset of common IA-32 instruction forms.
static bool validOperandForm(const string& mnemonic, const vector<string>& ops,
                             const vector<string>& types) {
    const size_t n = ops.size();
    if (mnemonic == "RET" || mnemonic == "NOP") return n == 0;

    if (mnemonic == "MOV" || mnemonic == "ADD" || mnemonic == "SUB" ||
        mnemonic == "AND" || mnemonic == "OR" || mnemonic == "XOR" ||
        mnemonic == "CMP") {
        if (n != 2) return false;
        // Destination cannot be an immediate constant or a bare label.
        if (!isRegOrMem(types[0])) return false;
        // x86 does not allow memory-to-memory forms for these instructions.
        if (isMemoryType(types[0]) && isMemoryType(types[1])) return false;
        // A bare symbol as source is treated as a label/address expression in this
        // simplified syntax; use [symbol] when the intent is memory access.
        if (types[1] == "UNKNOWN") return false;
        return true;
    }

    if (mnemonic == "INC" || mnemonic == "DEC" || mnemonic == "NOT" ||
        mnemonic == "MUL" || mnemonic == "DIV") {
        return n == 1 && isRegOrMem(types[0]);
    }

    if (mnemonic == "JMP" || mnemonic == "JZ" || mnemonic == "JNE" || mnemonic == "CALL") {
        return n == 1 && (types[0] == "SYMBOL" || types[0] == "CONSTANT" || types[0] == "MEMORY");
    }

    // Mnemonic exists in opcode.txt but has no validation rule in this program.
    return false;
}

int main() {
    string opcodeFile, asmFile;
    cout << "Enter opcode file: ";
    cin >> opcodeFile;
    cout << "Enter assembly file: ";
    cin >> asmFile;

    MnemonicValidator mnemonicValidator;
    OperandValidator operandValidator;
    if (!mnemonicValidator.loadOpcodeFile(opcodeFile)) {
        cout << "Could not open opcode file: " << opcodeFile << "\n";
        return 1;
    }
    if (!operandValidator.loadRegisters("registers.txt")) {
        cout << "Warning: could not open registers.txt; register names may be reported as unknown.\n";
    }

    SymbolTable symbolTable;
    if (!symbolTable.build(asmFile)) {
        cout << "Could not open assembly file: " << asmFile << "\n";
        return 1;
    }
    symbolTable.print();

    ifstream file(asmFile);
    string line;
    cout << "\n========== INSTRUCTION VALIDATION ==========\n";
    while (getline(file, line)) {
        ParsedLine p = preprocess(line);
        if (p.mnemonic.empty()) continue;

        cout << "\nMnemonic: " << p.mnemonic;
        bool mnemonicOK = mnemonicValidator.isValid(p.mnemonic);
        cout << "\nMnemonic Status: " << (mnemonicOK ? "Found" : "Not Found");

        vector<string> types;
        bool operandsKnown = true;
        for (const string& op : p.operands) {
            string type = operandValidator.getOperandType(op);
            types.push_back(type);
            bool known = operandValidator.isKnownOperand(op, symbolTable);
            cout << "\nOperand: " << op << "\nType: " << type
                 << "\nOperand Status: " << (known ? "Recognized" : "Unknown symbol/operand");
            if (!known) operandsKnown = false;
        }

        bool formOK = mnemonicOK && operandsKnown &&
                      validOperandForm(p.mnemonic, p.operands, types);
        if (mnemonicOK && (p.mnemonic == "RET" || p.mnemonic == "NOP") && !p.operands.empty())
            formOK = false;

        cout << "\nOperand Count: " << p.operands.size();
        cout << "\nOverall Status: " << (formOK ? "VALID" : "INVALID");
        if (mnemonicOK && operandsKnown && !validOperandForm(p.mnemonic, p.operands, types))
            cout << " (invalid operand count/type combination)";
        cout << "\n";
    }
    return 0;
}
