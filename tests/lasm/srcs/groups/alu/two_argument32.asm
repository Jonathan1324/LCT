; FORMATS: BIN,ELF
; BITS: 32
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    ; ADD - 8-bit
    add al, bl
    add al, [eax]
    add al, 127
    add [eax], al
    add byte [eax], 100
    add ah, [ecx + 10]
    add [ecx + ebx*2 - 5], ah
    add byte [ebp + eax*4 + 1000], 75
    add al, [bx]
    add ah, [bp + 10]
    add byte [bx + si], 50

    ; ADD - 16-bit
    add ax, bx
    add ax, [eax]
    add ax, 1000
    add [eax], ax
    add word [eax], 50
    add cx, [edx + eax*2 + 100]
    add [edx + 2000], cx
    add word [ebp + esi*4 + 3000], 2000
    add ax, [bx]
    add cx, [bp + si]
    add word [bx + di], -10

    ; ADD - 32-bit
    add eax, ebx
    add eax, [eax]
    add eax, 10000
    add [eax], eax
    add dword [eax], 50000
    add ecx, [edx + eax*2 + 100]
    add [edx + 2000], ecx
    add dword [ebp + esi*4 + 3000], 20000
    add eax, [bx]
    add ecx, [bp + si]
    add dword [bx + di], 10000

    ; ADC - 8-bit
    adc al, bl
    adc al, [eax]
    adc al, 99
    adc [eax], al
    adc byte [eax], 50
    adc ah, [ecx + eax*2 + 1000]
    adc byte [ecx + 500], ah
    adc al, [bx]
    adc ah, [bp]
    adc byte [bx + si], 30

    ; ADC - 16-bit
    adc ax, bx
    adc ax, [eax]
    adc ax, 5000
    adc [eax], ax
    adc word [eax], 1000
    adc cx, [edx + ebx*4 + 2000]
    adc word [edx + 1500], cx
    adc ax, [bx]
    adc cx, [bp + di]
    adc word [bx + si], 800

    ; ADC - 32-bit
    adc eax, ebx
    adc eax, [eax]
    adc eax, 50000
    adc [eax], eax
    adc dword [eax], 10000
    adc ecx, [edx + ebx*4 + 2000]
    adc dword [edx + 1500], ecx
    adc eax, [bx]
    adc ecx, [bp + di]
    adc dword [bx + si], 8000

    ; SBB - 8-bit
    sbb al, bl
    sbb al, [eax]
    sbb al, 88
    sbb [eax], al
    sbb byte [eax], 60
    sbb ah, [ecx + eax*2 + 1000]
    sbb byte [ecx + 500], ah
    sbb al, [bx]
    sbb ah, [bp]
    sbb byte [bx + si], 40

    ; SBB - 16-bit
    sbb ax, bx
    sbb ax, [eax]
    sbb ax, 4000
    sbb [eax], ax
    sbb word [eax], 1500
    sbb cx, [edx + ebx*4 + 2000]
    sbb word [edx + 1500], cx
    sbb ax, [bx]
    sbb cx, [bp + di]
    sbb word [bx + si], 900

    ; SBB - 32-bit
    sbb eax, ebx
    sbb eax, [eax]
    sbb eax, 40000
    sbb [eax], eax
    sbb dword [eax], 15000
    sbb ecx, [edx + ebx*4 + 2000]
    sbb dword [edx + 1500], ecx
    sbb eax, [bx]
    sbb ecx, [bp + di]
    sbb dword [bx + si], 9000

    ; OR - 8-bit
    or al, bl
    or al, [eax]
    or al, 77
    or [eax], al
    or byte [eax], 70
    or ah, [ecx + eax*2 + 1000]
    or byte [ecx + 500], ah
    or al, [bx]
    or ah, [bp]
    or byte [bx + si], 35

    ; OR - 16-bit
    or ax, bx
    or ax, [eax]
    or ax, 3000
    or [eax], ax
    or word [eax], 2000
    or cx, [edx + ebx*4 + 2000]
    or word [edx + 1500], cx
    or ax, [bx]
    or cx, [bp + di]
    or word [bx + si], 1500

    ; OR - 32-bit
    or eax, ebx
    or eax, [eax]
    or eax, 30000
    or [eax], eax
    or dword [eax], 20000
    or ecx, [edx + ebx*4 + 2000]
    or dword [edx + 1500], ecx
    or eax, [bx]
    or ecx, [bp + di]
    or dword [bx + si], 15000

    ; SUB - 8-bit
    sub al, bl
    sub al, [eax]
    sub al, 66
    sub [eax], al
    sub byte [eax], 80
    sub ah, [ecx + eax*2 + 1000]
    sub byte [ecx + 500], ah
    sub al, [bx]
    sub ah, [bp]
    sub byte [bx + si], 45

    ; SUB - 16-bit
    sub ax, bx
    sub ax, [eax]
    sub ax, 2000
    sub [eax], ax
    sub word [eax], 2500
    sub cx, [edx + ebx*4 + 2000]
    sub word [edx + 1500], cx
    sub ax, [bx]
    sub cx, [bp + di]
    sub word [bx + si], 1100

    ; SUB - 32-bit
    sub eax, ebx
    sub eax, [eax]
    sub eax, 20000
    sub [eax], eax
    sub dword [eax], 25000
    sub ecx, [edx + ebx*4 + 2000]
    sub dword [edx + 1500], ecx
    sub eax, [bx]
    sub ecx, [bp + di]
    sub dword [bx + si], 11000

    ; AND - 8-bit
    and al, bl
    and al, [eax]
    and al, 55
    and [eax], al
    and byte [eax], 90
    and ah, [ecx + eax*2 + 1000]
    and byte [ecx + 500], ah
    and al, [bx]
    and ah, [bp]
    and byte [bx + si], 60

    ; AND - 16-bit
    and ax, bx
    and ax, [eax]
    and ax, 1000
    and [eax], ax
    and word [eax], 3000
    and cx, [edx + ebx*4 + 2000]
    and word [1500], cx
    and ax, [bx]
    and cx, [bp + di]
    and word [bx + si], 1800

    ; AND - 32-bit
    and eax, ebx
    and eax, [eax]
    and eax, 10000
    and [eax], eax
    and dword [eax], 30000
    and ecx, [edx + ebx*4 + 2000]
    and dword [edx + 1500], ecx
    and eax, [bx]
    and ecx, [bp + di]
    and dword [bx + si], 18000

    ; XOR - 8-bit
    xor al, bl
    xor al, [eax]
    xor al, 44
    xor [eax], al
    xor byte [eax], 110
    xor ah, [ecx + eax*2 + 1000]
    xor byte [ecx + 500], ah
    xor al, [bx]
    xor ah, [bp]
    xor byte [bx + si], 55

    ; XOR - 16-bit
    xor ax, bx
    xor ax, [eax]
    xor ax, 4000
    xor [eax], ax
    xor word [eax], 1200
    xor cx, [edx + ebx*4 + 2000]
    xor word [edx + 1500], cx
    xor ax, [bx]
    xor cx, [bp + di]
    xor word [bx + si], 1600

    ; XOR - 32-bit
    xor eax, ebx
    xor eax, [eax]
    xor eax, 40000
    xor [eax], eax
    xor dword [eax], 12000
    xor ecx, [edx + ebx*4 + 2000]
    xor dword [edx + 1500], ecx
    xor eax, [bx]
    xor ecx, [bp + di]
    xor dword [bx + si], 16000

    ; CMP - 8-bit
    cmp al, bl
    cmp al, [eax]
    cmp al, 33
    cmp [eax], al
    cmp byte [eax], 120
    cmp ah, [ecx + eax*2 + 1000]
    cmp byte [ecx + 500], ah
    cmp al, [bx]
    cmp ah, [bp]
    cmp byte [bx + si], 25

    ; CMP - 16-bit
    cmp ax, bx
    cmp ax, [eax]
    cmp ax, 6000
    cmp [eax], ax
    cmp word [eax], 4000
    cmp cx, [edx + ebx*4 + 2000]
    cmp word [edx + 1500], cx
    cmp ax, [bx]
    cmp cx, [bp + di]
    cmp word [bx + si], 2000

    ; CMP - 32-bit
    cmp eax, ebx
    cmp eax, [eax]
    cmp eax, 60000
    cmp [eax], eax
    cmp dword [eax], 40000
    cmp ecx, [edx + ebx*4 + 2000]
    cmp dword [edx + 1500], ecx
    cmp eax, [bx]
    cmp ecx, [bp + di]
    cmp dword [bx + si], 20000

    ; TEST - 8-bit
    test al, bl
    test al, [eax]
    test al, 22
    test [eax], al
    test byte [eax], 130
    test ah, [ecx + eax*2 + 1000]
    test byte [ecx + 500], ah
    test al, [bx]
    test ah, [bp]
    test byte [bx + si], 75

    ; TEST - 16-bit
    test ax, bx
    test ax, [eax]
    test ax, -7000
    test [eax], ax
    test word [eax], 5000
    test cx, [edx + ebx*4 + 2000]
    test word [edx + 1500], cx
    test ax, [bx]
    test cx, [bp + di]
    test word [bx + si], 2500

    ; TEST - 32-bit
    test eax, ebx
    test eax, [eax]
    test eax, 120
    test [eax], eax
    test dword [eax], -80
    test ecx, [edx + ebx*4 + 2000]
    test dword [edx + 1500], ecx
    test eax, [bx]
    test ecx, [bp + di]
    test dword [bx + si], 2
