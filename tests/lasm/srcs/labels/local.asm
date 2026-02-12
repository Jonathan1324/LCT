; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

section .text
    global _start

F1:
    db 0
L1:
    db 1
L2:
    db 2
