MOV AL, 3
MOV BL, AL
MOV AL, 1

L1:
MUL BL
DEC BL
JNZ L1

; First factorial is in AX

MOV CL, 4
MOV BL, CL
MOV AL, 1

L2:
MUL BL
DEC BL
JNZ L2

; Second factorial is in AX

HLT