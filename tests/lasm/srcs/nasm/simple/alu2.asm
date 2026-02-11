; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    add al, [rax]
    add [rax], al
    add ah, [ecx]
    add [ecx], ah

    add ax, [rax]
    add [rax], ax
    add cx, [rdx]
    add [rdx], cx

    add eax, [rax]
    add [rax], eax
    add ecx, [rdx]
    add [rdx], ecx

    add rax, [rax]
    add [rax], rax
    add rcx, [rdx]
    add [rdx], rcx

    adc al, [rax]
    adc [rax], al
    adc ax, [rax]
    adc [rax], ax
    adc eax, [rax]
    adc [rax], eax
    adc rax, [rax]
    adc [rax], rax

    sbb al, [rax]
    sbb [rax], al
    sbb ax, [rax]
    sbb [rax], ax
    sbb eax, [rax]
    sbb [rax], eax
    sbb rax, [rax]
    sbb [rax], rax

    or al, [rax]
    or [rax], al
    or ax, [rax]
    or [rax], ax
    or eax, [rax]
    or [rax], eax
    or rax, [rax]
    or [rax], rax

    sub al, [rax]
    sub [rax], al
    sub ax, [rax]
    sub [rax], ax
    sub eax, [rax]
    sub [rax], eax
    sub rax, [rax]
    sub [rax], rax

    and al, [rax]
    and [rax], al
    and ax, [rax]
    and [rax], ax
    and eax, [rax]
    and [rax], eax
    and rax, [rax]
    and [rax], rax

    xor al, [rax]
    xor [rax], al
    xor ax, [rax]
    xor [rax], ax
    xor eax, [rax]
    xor [rax], eax
    xor rax, [rax]
    xor [rax], rax

    cmp al, [rax]
    cmp [rax], al
    cmp ax, [rax]
    cmp [rax], ax
    cmp eax, [rax]
    cmp [rax], eax
    cmp rax, [rax]
    cmp [rax], rax

    test al, [rax]
    test [rax], al
    test ax, [rax]
    test [rax], ax
    test eax, [rax]
    test [rax], eax
    test rax, [rax]
    test [rax], rax
