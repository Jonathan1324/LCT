; FORMATS: BIN,ELF
; BITS: 32
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    ; NOT - 32-bit Addressing
    not al
    not ax
    not eax
    
    not byte [eax]
    not word [eax]
    not dword [eax]
    
    not byte [eax + 10]
    not word [eax + 10]
    not dword [eax + 10]
    not byte [ebp - 16]
    not word [ebp - 16]
    not dword [ebp - 16]
    not dword [eax + esp]
    not byte [5]
    not word [eax + ebx*4 + 100]
    not dword [ecx*2 + _start]
    
    ; NOT - 16-bit Addressing
    not byte [bx]
    not word [bx]
    not dword [bx]
    
    not byte [bp + 10]
    not word [bp + 10]
    not dword [bp + 10]
    not byte [si]
    not word [di]
    not dword [bx + si]
    
    ; NEG - 32-bit Addressing
    neg al
    neg ax
    neg eax
    
    neg byte [eax]
    neg word [eax]
    neg dword [eax]
    
    neg byte [eax + 10]
    neg word [eax + 10]
    neg dword [eax + 10]
    neg byte [ebp - 16]
    neg word [ebp - 16]
    neg dword [ebp - 16]
    neg dword [eax + esp]
    neg byte [5]
    neg word [eax + ebx*4 + 100]
    neg dword [ecx*2 + _start]
    
    ; NEG - 16-bit Addressing
    neg byte [bx]
    neg word [bx]
    neg dword [bx]
    
    neg byte [bp + 10]
    neg word [bp + 10]
    neg dword [bp + 10]
    neg byte [si]
    neg word [di]
    neg dword [bx + si]
    
    ; INC - 32-bit Addressing
    inc al
    inc ax
    inc eax
    
    inc byte [eax]
    inc word [eax]
    inc dword [eax]
    
    inc byte [eax + 10]
    inc word [eax + 10]
    inc dword [eax + 10]
    inc byte [ebp - 16]
    inc word [ebp - 16]
    inc dword [ebp - 16]
    inc dword [eax + esp]
    inc byte [5]
    inc word [eax + ebx*4 + 100]
    inc dword [1000]
    inc dword [eax + ecx]
    
    ; INC - 16-bit Addressing
    inc byte [bx]
    inc word [bx]
    inc dword [bx]
    
    inc byte [bp + 10]
    inc word [bp + 10]
    inc dword [bp + 10]
    inc byte [si]
    inc word [di]
    inc dword [bx + si]
    
    ; DEC - 32-bit Addressing
    dec al
    dec ax
    dec eax
    
    dec byte [eax]
    dec word [eax]
    dec dword [eax]
    
    dec byte [eax + 10]
    dec word [eax + 10]
    dec dword [eax + 10]
    dec byte [ebp - 16]
    dec word [ebp - 16]
    dec dword [ebp - 16]
    dec dword [eax + esp]
    dec byte [5]
    dec word [eax + ebx*4 + 100]
    dec byte [ecx + ebx*4 + 16]
    dec dword [edx*2 + _start]
    
    ; DEC - 16-bit Addressing
    dec byte [bx]
    dec word [bx]
    dec dword [bx]
    
    dec byte [bp + 10]
    dec word [bp + 10]
    dec dword [bp + 10]
    dec byte [si]
    dec word [di]
    dec dword [bx + si]
