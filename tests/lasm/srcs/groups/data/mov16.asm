; FORMATS: BIN,ELF
; BITS: 16
; EXPECT: SUCCESS

[bits 16]

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
    mov dh, al

    ; MOV - 8-bit Register to Immediate
    mov al, 0
    mov al, 1
    mov al, 127
    mov bl, 255
    mov cl, 100
    mov dl, 99
    mov ah, 50
    mov dh, 200

    ; MOV - 8-bit Register to Memory (32-bit addressing)
    mov al, [eax]
    mov bl, [ebx + 10]
    mov cl, [ecx + eax*2 + 100]
    mov [eax], al
    mov [ebx + 10], bl
    mov [ecx + eax*2 + 100], cl

    ; MOV - 8-bit Register to Memory (16-bit addressing)
    mov al, [bx]
    mov bl, [bp + 10]
    mov cl, [bx + si + 100]
    mov [bx], al
    mov [bp + 10], bl
    mov [bx + si + 100], cl

    ; MOV - 8-bit Memory to Immediate
    mov byte [eax], 50
    mov byte [ebx + 10], 100
    mov byte [ecx + eax*2 + 100], 75
    mov byte [bx], 25
    mov byte [bp + 10], 60

    ; MOV - 16-bit Register to Register
    mov ax, bx
    mov ax, cx
    mov ax, dx
    mov bx, ax
    mov cx, dx
    mov dx, si
    mov si, di
    mov di, ax

    ; MOV - 16-bit Register to Immediate
    mov ax, 0
    mov ax, 1
    mov ax, 100
    mov ax, 1000
    mov ax, 5000
    mov bx, 10000
    mov cx, 32767
    mov dx, 65535

    ; MOV - 16-bit Register to Memory (32-bit addressing)
    mov ax, [eax]
    mov bx, [ebx + 10]
    mov cx, [ecx + eax*2 + 100]
    mov [eax], ax
    mov [ebx + 10], bx
    mov [ecx + eax*2 + 100], cx

    ; MOV - 16-bit Register to Memory (16-bit addressing)
    mov ax, [bx]
    mov bx, [bp + 10]
    mov cx, [bx + si + 100]
    mov [bx], ax
    mov [bp + 10], bx
    mov [bx + si + 100], cx

    ; MOV - 16-bit Memory to Immediate
    mov word [eax], 1000
    mov word [ebx + 10], 5000
    mov word [ecx + eax*2 + 100], 2000
    mov word [bx], 800
    mov word [bp + 10], 1200

    ; MOV - 32-bit Register to Register
    mov eax, ebx
    mov eax, ecx
    mov eax, edx
    mov ebx, eax
    mov ecx, edx
    mov edx, esi
    mov esi, edi
    mov edi, eax

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

    ; MOV - 32-bit Register to Memory (32-bit addressing)
    mov eax, [eax]
    mov ebx, [ebx + 10]
    mov ecx, [ecx + eax*2 + 100]
    mov [eax], eax
    mov [ebx + 10], ebx
    mov [ecx + eax*2 + 100], ecx

    ; MOV - 32-bit Register to Memory (16-bit addressing)
    mov eax, [bx]
    mov ebx, [bp + 10]
    mov ecx, [bx + si + 100]
    mov [bx], eax
    mov [bp + 10], ebx
    mov [bx + si + 100], ecx

    ; MOV - 32-bit Memory to Immediate
    mov dword [eax], 10000
    mov dword [ebx + 10], 50000
    mov dword [ecx + eax*2 + 100], 20000
    mov dword [bx], 8000
    mov dword [bp + 10], 12000

    ; MOV - Direct Addressing
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

    ; MOV - Segment Register from Memory (32-bit addressing)
    mov fs, [eax]
    mov cs, [ebx + 10]
    mov ds, [ecx + eax*2 + 100]
    mov [eax], cs
    mov [ebx + 10], ds
    mov [ecx + eax*2 + 100], es

    ; MOV - Segment Register from Memory (16-bit addressing)
    mov fs, [bx]
    mov cs, [bp + 10]
    mov ds, [bx + si + 100]
    mov [bx], cs
    mov [bp + 10], ds
    mov [bx + si + 100], es

    ; MOV - Control Registers (only Register to Register)
    mov eax, cr0
    mov ebx, cr2
    mov ecx, cr3
    mov edx, cr4

    mov cr0, eax
    mov cr2, ebx
    mov cr3, ecx
    mov cr4, edx

    ; MOV - Debug Registers (only Register to Register)
    mov eax, dr0
    mov ebx, dr1
    mov ecx, dr2
    mov edx, dr3
    mov esi, dr6
    mov edi, dr7

    mov dr0, eax
    mov dr1, ebx
    mov dr2, ecx
    mov dr3, edx
    mov dr6, esi
    mov dr7, edi
