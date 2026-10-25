ASSIGNMENT 4: SYMBOL TABLE GENERATION AND INSTRUCTION VALIDATION

Files:
- main.cpp                 Main program and instruction-form checks
- preprocessor.h           Parses assembly instructions and operands
- mnemonics_validator.h    Checks mnemonic names against opcode.txt
- operand_validator.h      Identifies and checks operand types/symbols
- register_validator.h     Checks register names against registers.txt
- symbol_table.h           Builds and prints the symbol table
- opcode.txt               Mnemonic list
- registers.txt            Register list
- text.asm                 Sample assembly input

Compile (from this folder):
    g++ -std=c++11 main.cpp -o assignment4

Run:
    ./assignment4

When prompted, enter:
    opcode.txt
    text.asm

Validation implemented:
- Mnemonic must exist in opcode.txt.
- Registers are read from registers.txt.
- Bare symbols must exist in the generated symbol table.
- [symbol] memory operands must refer to a known symbol.
- MOV/ADD/SUB/AND/OR/XOR/CMP require two operands and reject immediate/bare-symbol destinations and memory-to-memory forms.
- INC/DEC/NOT/MUL/DIV require one register or memory operand.
- JMP/JZ/JNE/CALL require one symbol, constant, or memory operand.
- RET/NOP require no operands.

Limitations:
- This is an educational validator for a limited instruction subset, not a complete IA-32 assembler.
- It does not generate machine code or verify all CPU encoding/size rules.
- Symbol sizes for DB/DW/DD declarations are approximate (comma-separated values are counted).
- Text-section locations count each recognized instruction as one unit, not actual byte addresses.
- Bare symbols used as data sources are treated as recognized symbols; use [symbol] for explicit memory syntax in this simplified assembly format.
