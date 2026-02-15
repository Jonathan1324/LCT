; FORMATS: BIN,ELF
; BITS: 16
; EXPECT: SUCCESS

[bits 16]

section .text
    global _start

_start:
    ; JMP - 32-bit Register
    jmp eax
    jmp ebx
    jmp ecx
    jmp edx
    jmp esi
    jmp edi
    jmp ebp
    jmp esp

    ; JMP - 16-bit Register
    jmp ax
    jmp bx
    jmp cx
    jmp dx
    jmp si
    jmp di
    jmp bp
    jmp sp

    ; JMP - 32-bit Memory (32-bit addressing)
    jmp word [eax]
    jmp dword [ebx + 10]
    jmp word [ecx + eax*2 + 100]
    jmp dword [edx + ebx*4 - 5]
    jmp word [esi + edi*8 + 1000]

    ; JMP - 32-bit Memory (16-bit addressing)
    jmp word [bx]
    jmp dword [bp + 10]
    jmp word [bx + si + 100]
    jmp dword [bx + di - 5]

    ; JMP - 16-bit Memory (32-bit addressing)
    jmp [eax]
    jmp [ebx + 10]
    jmp [ecx + eax*2 + 100]

    ; JMP - 16-bit Memory (16-bit addressing)
    jmp [bx]
    jmp [bp + 10]
    jmp [bx + si]

    ; JMP - Direct Labels (relative to next instruction)
    jmp label1
    jmp label2
    jmp label3
    jmp label4
    jmp label5
    jmp label6
    jmp label7
    jmp label8

label1:
    nop

label2:
    nop

label3:
    nop

label4:
    nop

label5:
    nop

label6:
    nop

label7:
    nop

label8:
    nop

    ; JE - Jump if Equal
    je je_label1
    je je_label2
    je je_label3
    je je_label4

je_label1:
    nop

je_label2:
    nop

je_label3:
    nop

je_label4:
    nop

    ; JNE - Jump if Not Equal
    jne jne_label1
    jne jne_label2
    jne jne_label3
    jne jne_label4

jne_label1:
    nop

jne_label2:
    nop

jne_label3:
    nop

jne_label4:
    nop

    ; JG - Jump if Greater (signed)
    jg jg_label1
    jg jg_label2
    jg jg_label3
    jg jg_label4

jg_label1:
    nop

jg_label2:
    nop

jg_label3:
    nop

jg_label4:
    nop

    ; JGE - Jump if Greater or Equal (signed)
    jge jge_label1
    jge jge_label2
    jge jge_label3
    jge jge_label4

jge_label1:
    nop

jge_label2:
    nop

jge_label3:
    nop

jge_label4:
    nop

    ; JL - Jump if Less (signed)
    jl jl_label1
    jl jl_label2
    jl jl_label3
    jl jl_label4

jl_label1:
    nop

jl_label2:
    nop

jl_label3:
    nop

jl_label4:
    nop

    ; JLE - Jump if Less or Equal (signed)
    jle jle_label1
    jle jle_label2
    jle jle_label3
    jle jle_label4

jle_label1:
    nop

jle_label2:
    nop

jle_label3:
    nop

jle_label4:
    nop

    ; JA - Jump if Above (unsigned)
    ja ja_label1
    ja ja_label2
    ja ja_label3
    ja ja_label4

ja_label1:
    nop

ja_label2:
    nop

ja_label3:
    nop

ja_label4:
    nop

    ; JAE - Jump if Above or Equal (unsigned)
    jae jae_label1
    jae jae_label2
    jae jae_label3
    jae jae_label4

jae_label1:
    nop

jae_label2:
    nop

jae_label3:
    nop

jae_label4:
    nop

    ; JB - Jump if Below (unsigned)
    jb jb_label1
    jb jb_label2
    jb jb_label3
    jb jb_label4

jb_label1:
    nop

jb_label2:
    nop

jb_label3:
    nop

jb_label4:
    nop

    ; JBE - Jump if Below or Equal (unsigned)
    jbe jbe_label1
    jbe jbe_label2
    jbe jbe_label3
    jbe jbe_label4

jbe_label1:
    nop

jbe_label2:
    nop

jbe_label3:
    nop

jbe_label4:
    nop

    ; JO - Jump if Overflow
    jo jo_label1
    jo jo_label2
    jo jo_label3
    jo jo_label4

jo_label1:
    nop

jo_label2:
    nop

jo_label3:
    nop

jo_label4:
    nop

    ; JNO - Jump if No Overflow
    jno jno_label1
    jno jno_label2
    jno jno_label3
    jno jno_label4

jno_label1:
    nop

jno_label2:
    nop

jno_label3:
    nop

jno_label4:
    nop

    ; JS - Jump if Sign (negative)
    js js_label1
    js js_label2
    js js_label3
    js js_label4

js_label1:
    nop

js_label2:
    nop

js_label3:
    nop

js_label4:
    nop

    ; JNS - Jump if No Sign (positive or zero)
    jns jns_label1
    jns jns_label2
    jns jns_label3
    jns jns_label4

jns_label1:
    nop

jns_label2:
    nop

jns_label3:
    nop

jns_label4:
    nop

    ; JP - Jump if Parity
    jp jp_label1
    jp jp_label2
    jp jp_label3
    jp jp_label4

jp_label1:
    nop

jp_label2:
    nop

jp_label3:
    nop

jp_label4:
    nop

    ; JNP - Jump if No Parity
    jnp jnp_label1
    jnp jnp_label2
    jnp jnp_label3
    jnp jnp_label4

jnp_label1:
    nop

jnp_label2:
    nop

jnp_label3:
    nop

jnp_label4:
    nop

    ; JC - Jump if Carry
    jc jc_label1
    jc jc_label2
    jc jc_label3
    jc jc_label4

jc_label1:
    nop

jc_label2:
    nop

jc_label3:
    nop

jc_label4:
    nop

    ; JNC - Jump if No Carry
    jnc jnc_label1
    jnc jnc_label2
    jnc jnc_label3
    jnc jnc_label4

jnc_label1:
    nop

jnc_label2:
    nop

jnc_label3:
    nop

jnc_label4:
    nop
