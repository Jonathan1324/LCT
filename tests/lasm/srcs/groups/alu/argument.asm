; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    not al
    not ax
    not eax
    not rax
    
    not byte [rax]
    not word [rax]
    not dword [rax]
    not qword [rax]
    
    not byte [eax + 10]
    not word [eax + 10]
    not dword [eax + 10]
    not qword [rbp - 16]
    not dword [rax + rsp]
    not byte [5]
    not word [rax + rbx*4 + 100]
    not dword [r15d*2 + _start]
    
    neg al
    neg ax
    neg eax
    neg rax
    
    neg byte [rax]
    neg word [rax]
    neg dword [rax]
    neg qword [rax]
    
    neg byte [eax + 10]
    neg word [eax + 10]
    neg dword [eax + 10]
    neg qword [rbp - 16]
    neg dword [rax + rsp]
    neg byte [5]
    neg word [rax + rbx*4 + 100]
    neg dword [r15d*2 + _start]
    
    inc al
    inc ax
    inc eax
    inc rax
    
    inc byte [rax]
    inc word [rax]
    inc dword [rax]
    inc qword [rax]
    
    inc byte [eax + 10]
    inc word [eax + 10]
    inc dword [eax + 10]
    inc qword [rbp - 16]
    inc dword [rax + rsp]
    inc byte [5]
    inc word [rax + rbx*4 + 100]
    inc dword [1000]
    inc qword [rbp + rax + rbp]
    
    dec al
    dec ax
    dec eax
    dec rax
    
    dec byte [rax]
    dec word [rax]
    dec dword [rax]
    dec qword [rax]
    
    dec byte [eax + 10]
    dec word [eax + 10]
    dec dword [eax + 10]
    dec qword [rbp - 16]
    dec dword [rax + rsp]
    dec byte [5]
    dec word [rax + rbx*4 + 100]
    dec byte [eax + ebx*4 + 16]
    dec word [r15d*2 + _start]
