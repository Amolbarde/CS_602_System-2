; Sample input for Assignment 4
SECTION .data
num1 DB 10
num2 DW 20
array RESB 5

SECTION .bss
result RESD 1

SECTION .text
main:
    MOV EAX, 10
    ADD EAX, EBX
    MOV [result], EAX
    SUB EAX, 2
    JMP finish
finish:
    RET
