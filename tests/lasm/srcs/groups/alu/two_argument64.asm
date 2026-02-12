; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    ; ADD - 8-bit
    add al, bl
    add al, [rax]
    add al, 127
    add [rax], al
    add byte [rax], 100
    add ah, [ecx + 10]
    add [rcx + rbx*2 - 5], ah
    add byte [rbp + rax*4 + 1000], 75

    ; ADD - 16-bit
    add ax, bx
    add ax, [rax]
    add ax, 1000
    add [rax], ax
    add word [rax], 50
    add cx, [rdx + rax*2 + 100]
    add [rdx + 2000], cx
    add word [rbp + rsi*4 + 3000], -10

    ; ADD - 32-bit
    add eax, ebx
    add eax, [rax]
    add eax, -10000
    add [rax], eax
    add dword [rax], 2
    add ecx, [rdx + rax*2 + 100]
    add [rdx + 2000], ecx
    add dword [rbp + rsi*4 + 3000], -77

    ; ADD - 64-bit
    add rax, rbx
    add rax, [rax]
    add rax, -10000
    add [rax], rax
    add qword [rax], 2
    add rcx, [rdx + rax*2 + 100]
    add [rdx + 2000], rcx
    add qword [rbp + rsi*4 + 3000], -77

    ; ADC - 8-bit
    adc al, bl
    adc al, [rax]
    adc al, 99
    adc [rax], al
    adc byte [rax], 50
    adc ah, [ecx + eax*2 + 1000]
    adc byte [ecx + 500], ah

    ; ADC - 16-bit
    adc ax, bx
    adc ax, [rax]
    adc ax, 5000
    adc [rax], ax
    adc word [rax], 1000
    adc cx, [rdx + rbx*4 + 2000]
    adc word [rdx + 1500], cx

    ; ADC - 32-bit
    adc eax, ebx
    adc eax, [rax]
    adc eax, 50000
    adc [rax], eax
    adc dword [rax], 10000
    adc ecx, [rdx + rbx*4 + 2000]
    adc dword [rdx + 1500], ecx

    ; ADC - 64-bit
    adc rax, rbx
    adc rax, [rax]
    adc rax, 50000
    adc [rax], rax
    adc qword [rax], 10000
    adc rcx, [rdx + rbx*4 + 2000]
    adc qword [rdx + 1500], rcx

    ; SBB - 8-bit
    sbb al, bl
    sbb al, [rax]
    sbb al, 88
    sbb [rax], al
    sbb byte [rax], 60
    sbb ah, [rcx + rax*2 + 1000]
    sbb byte [ecx + 500], ah

    ; SBB - 16-bit
    sbb ax, bx
    sbb ax, [rax]
    sbb ax, 4000
    sbb [rax], ax
    sbb word [rax], 1500
    sbb cx, [rdx + rbx*4 + 2000]
    sbb word [rdx + 1500], cx

    ; SBB - 32-bit
    sbb eax, ebx
    sbb eax, [rax]
    sbb eax, 40000
    sbb [rax], eax
    sbb dword [rax], 15000
    sbb ecx, [rdx + rbx*4 + 2000]
    sbb dword [rdx + 1500], ecx

    ; SBB - 64-bit
    sbb rax, rbx
    sbb rax, [rax]
    sbb rax, 40000
    sbb [rax], rax
    sbb qword [rax], 15000
    sbb rcx, [rdx + rbx*4 + 2000]
    sbb qword [rdx + 1500], rcx

    ; OR - 8-bit
    or al, bl
    or al, [rax]
    or al, 77
    or [rax], al
    or byte [rax], 70
    or ah, [rcx + rax*2 + 1000]
    or byte [ecx + 500], ah

    ; OR - 16-bit
    or ax, bx
    or ax, [rax]
    or ax, 3000
    or [rax], ax
    or word [rax], 2000
    or cx, [rdx + rbx*4 + 2000]
    or word [rdx + 1500], cx

    ; OR - 32-bit
    or eax, ebx
    or eax, [rax]
    or eax, 30000
    or [rax], eax
    or dword [rax], 20000
    or ecx, [rdx + rbx*4 + 2000]
    or dword [rdx + 1500], ecx

    ; OR - 64-bit
    or rax, rbx
    or rax, [rax]
    or rax, 30000
    or [rax], rax
    or qword [rax], 20000
    or rcx, [rdx + rbx*4 + 2000]
    or qword [rdx + 1500], rcx

    ; SUB - 8-bit
    sub al, bl
    sub al, [rax]
    sub al, 66
    sub [rax], al
    sub byte [rax], 80
    sub ah, [ecx + eax*2 + 1000]
    sub byte [ecx + 500], ah

    ; SUB - 16-bit
    sub ax, bx
    sub ax, [rax]
    sub ax, 2000
    sub [rax], ax
    sub word [rax], 2500
    sub cx, [rdx + rbx*4 + 2000]
    sub word [rdx + 1500], cx

    ; SUB - 32-bit
    sub eax, ebx
    sub eax, [rax]
    sub eax, 20000
    sub [rax], eax
    sub dword [rax], 25000
    sub ecx, [rdx + rbx*4 + 2000]
    sub dword [rdx + 1500], ecx

    ; SUB - 64-bit
    sub rax, rbx
    sub rax, [rax]
    sub rax, 20000
    sub [rax], rax
    sub qword [rax], 25000
    sub rcx, [rdx + rbx*4 + 2000]
    sub qword [rdx + 1500], rcx

    ; AND - 8-bit
    and al, bl
    and al, [rax]
    and al, 55
    and [rax], al
    and byte [rax], 90
    and ah, [ecx + eax*2 + 1000]
    and byte [ecx + 500], ah

    ; AND - 16-bit
    and ax, bx
    and ax, [rax]
    and ax, 1000
    and [rax], ax
    and word [rax], 3000
    and cx, [rdx + rbx*4 + 2000]
    and word [1500], cx

    ; AND - 32-bit
    and eax, ebx
    and eax, [rax]
    and eax, 10000
    and [rax], eax
    and dword [rax], 30000
    and ecx, [rdx + rbx*4 + 2000]
    and dword [rdx + 1500], ecx

    ; AND - 64-bit
    and rax, rbx
    and rax, [rax]
    and rax, 10000
    and [rax], rax
    and qword [rax], 30000
    and rcx, [rdx + rbx*4 + 2000]
    and qword [rdx + 1500], rcx

    ; XOR - 8-bit
    xor al, bl
    xor al, [rax]
    xor al, 44
    xor [rax], al
    xor byte [rax], 110
    xor ah, [rcx + rax*2 + 1000]
    xor byte [ecx + 500], ah

    ; XOR - 16-bit
    xor ax, bx
    xor ax, [rax]
    xor ax, 4000
    xor [rax], ax
    xor word [rax], 1200
    xor cx, [rdx + rbx*4 + 2000]
    xor word [rdx + 1500], cx

    ; XOR - 32-bit
    xor eax, ebx
    xor eax, [rax]
    xor eax, 40000
    xor [rax], eax
    xor dword [rax], 12000
    xor ecx, [rdx + rbx*4 + 2000]
    xor dword [rdx + 1500], ecx

    ; XOR - 64-bit
    xor rax, rbx
    xor rax, [rax]
    xor rax, 40000
    xor [rax], rax
    xor qword [rax], 12000
    xor rcx, [rdx + rbx*4 + 2000]
    xor qword [rdx + 1500], rcx

    ; CMP - 8-bit
    cmp al, bl
    cmp al, [rax]
    cmp al, 33
    cmp [rax], al
    cmp byte [rax], 120
    cmp ah, [ecx + eax*2 + 1000]
    cmp byte [ecx + 500], ah

    ; CMP - 16-bit
    cmp ax, bx
    cmp ax, [rax]
    cmp ax, 6000
    cmp [rax], ax
    cmp word [rax], 4000
    cmp cx, [rdx + rbx*4 + 2000]
    cmp word [rdx + 1500], cx

    ; CMP - 32-bit
    cmp eax, ebx
    cmp eax, [rax]
    cmp eax, 60000
    cmp [rax], eax
    cmp dword [rax], 40000
    cmp ecx, [rdx + rbx*4 + 2000]
    cmp dword [rdx + 1500], ecx

    ; CMP - 64-bit
    cmp rax, rbx
    cmp rax, [rax]
    cmp rax, 60000
    cmp [rax], rax
    cmp qword [rax], 40000
    cmp rcx, [rdx + rbx*4 + 2000]
    cmp qword [rdx + 1500], rcx

    ; TEST - 8-bit
    test al, bl
    test al, [rax]
    test al, 22
    test [rax], al
    test byte [rax], -1
    test ah, [ecx + eax*2 + 1000]
    test byte [ecx + 500], ah

    ; TEST - 16-bit
    test ax, bx
    test ax, [rax]
    test ax, -7
    test [rax], ax
    test word [rax], -8
    test cx, [rdx + rbx*4 + 2000]
    test word [rdx + 1500], cx

    ; TEST - 32-bit
    test eax, ebx
    test eax, [rax]
    test eax, 5
    test [rax], eax
    test dword [rax], 128
    test ecx, [rdx + rbx*4 + 2000]
    test dword [rdx + 1500], ecx

    ; TEST - 64-bit
    test rax, rbx
    test rax, [rax]
    test rax, 127
    test [rax], rax
    test qword [rax], -94
    test rcx, [rdx + rbx*4 + 2000]
    test qword [rdx + 1500], rcx
