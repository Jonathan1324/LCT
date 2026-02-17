; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    ; MOV - 8-bit Register to Register
    mov al, bl
    mov al, cl
    mov al, dl
    mov al, bh
    mov ah, al
    mov ah, bl
    mov cl, dl
    mov dl, sil
    mov r8b, r9b
    mov r10b, r11b
    mov r12b, r13b
    mov r14b, r15b

    ; MOV - 8-bit Register to Immediate
    mov al, 0
    mov al, 1
    mov al, 127
    mov bl, 255
    mov cl, 100
    mov dl, 99
    mov ah, 50
    mov r8b, 200
    mov r9b, 75
    mov r10b, 150

    ; MOV - 8-bit Register to Memory
    mov al, [rax]
    mov bl, [rbx + 10]
    mov cl, [rcx + rax*2 + 100]
    mov [rax], al
    mov [rbx + 10], bl
    mov [rcx + rax*2 + 100], cl

    ; MOV - 8-bit Memory to Immediate
    mov byte [rax], 50
    mov byte [rbx + 10], 100
    mov byte [rcx + rax*2 + 100], 75

    ; MOV - 16-bit Register to Register
    mov ax, bx
    mov ax, cx
    mov ax, dx
    mov bx, ax
    mov cx, dx
    mov dx, si
    mov si, di
    mov r8w, r9w
    mov r10w, r11w
    mov r12w, r13w
    mov r14w, r15w

    ; MOV - 16-bit Register to Immediate
    mov ax, 0
    mov ax, 1
    mov ax, 100
    mov ax, 1000
    mov ax, 5000
    mov bx, 10000
    mov cx, 32767
    mov dx, 65535
    mov r8w, 2000
    mov r9w, 4000

    ; MOV - 16-bit Register to Memory
    mov ax, [rax]
    mov bx, [rbx + 10]
    mov cx, [rcx + rax*2 + 100]
    mov [rax], ax
    mov [rbx + 10], bx
    mov [rcx + rax*2 + 100], cx

    ; MOV - 16-bit Memory to Immediate
    mov word [rax], 1000
    mov word [rbx + 10], 5000
    mov word [rcx + rax*2 + 100], 2000

    ; MOV - 32-bit Register to Register
    mov eax, ebx
    mov eax, ecx
    mov eax, edx
    mov ebx, eax
    mov ecx, edx
    mov edx, esi
    mov esi, edi
    mov r8d, r9d
    mov r10d, r11d
    mov r12d, r13d
    mov r14d, r15d

    ; MOV - 32-bit Register to Immediate
    mov eax, 0
    mov eax, 1
    mov eax, 100
    mov eax, 1000
    mov eax, 10000
    mov eax, 100000
    mov ebx, 50000
    mov ecx, 2147483647
    mov edx, 4294967295
    mov r8d, 5000
    mov r9d, 20000
    mov r10d, 75000

    ; MOV - 32-bit Register to Memory
    mov edx, [10]
    mov [10], edx
    mov eax, [10]
    mov [10], eax

    mov eax, [rax]
    mov ebx, [rbx + 10]
    mov ecx, [rcx + rax*2 + 100]
    mov [rax], eax
    mov [rbx + 10], ebx
    mov [rcx + rax*2 + 100], ecx

    ; MOV - 32-bit Memory to Immediate
    mov dword [rax], 10000
    mov dword [rbx + 10], 50000
    mov dword [rcx + rax*2 + 100], 20000

    ; MOV - 64-bit Register to Register
    mov rax, rbx
    mov rax, rcx
    mov rax, rdx
    mov rbx, rax
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov r8, r9
    mov r10, r11
    mov r12, r13
    mov r14, r15

    ; MOV - 64-bit Register to Immediate
    mov rax, 0
    mov rax, 1
    mov rax, 100
    mov rax, 1000
    mov rax, -10000
    mov rax, 100000
    mov rbx, 1000000
    mov rcx, 10000000
    mov rdx, -100000000
    mov r8, 100000000
    mov r9, 10000000000

    ; MOV - 64-bit Register to Memory
    mov rax, [rax]
    mov rbx, [rbx + 10]
    mov rcx, [rcx + rax*2 + 100]
    mov [rax], rax
    mov [rbx + 10], rbx
    mov [rcx + rax*2 + 100], rcx

    ; MOV - 64-bit Memory to Immediate
    mov qword [rax], 10000
    mov qword [rbx + 10], 50000
    mov qword [rcx + rax*2 + 100], 20000

    ; MOV - Direct Addressing
    mov rdx, [10]
    mov [10], rdx
    mov rax, [10]
    mov [10], rax
    mov edx, [10]
    mov [10], edx
    mov eax, [10]
    mov [10], eax
    mov al, [10]
    mov [10], al
    mov ah, [10]
    mov [10], ah
    mov ax, [10]
    mov [10], ax

    ; MOV - Segment Registers to GPR
    mov ax, cs
    mov bx, ds
    mov cx, es
    mov dx, fs
    mov si, gs
    mov di, ss

    ; MOV - GPR to Segment Registers
    mov cs, ax
    mov ds, bx
    mov es, cx
    mov fs, dx
    mov gs, si
    mov ss, di

    ; MOV - Segment Register from Memory
    mov fs, [rax]
    mov cs, [rbx + 10]
    mov ds, [rcx + rax*2 + 100]
    mov [rax], cs
    mov [rbx + 10], ds
    mov [rcx + rax*2 + 100], es

    ; MOV - Control Registers (only Register to Register)
    mov rax, cr0
    mov rbx, cr2
    mov rcx, cr3
    mov rdx, cr4
    mov r8, cr8

    mov cr0, rax
    mov cr2, rbx
    mov cr3, rcx
    mov cr4, rdx
    mov cr8, r8

    ; MOV - Debug Registers (only Register to Register)
    mov rax, dr0
    mov rbx, dr1
    mov rcx, dr2
    mov rdx, dr3
    mov r8, dr6
    mov r9, dr7

    mov dr0, rax
    mov dr1, rbx
    mov dr2, rcx
    mov dr3, rdx
    mov dr6, r8
    mov dr7, r9
