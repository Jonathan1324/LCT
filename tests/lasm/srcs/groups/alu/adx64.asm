; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    ; ADCX - 64-bit Register to Register
    adcx rax, rbx
    adcx rax, rcx
    adcx rax, rdx
    adcx rbx, rcx
    adcx rcx, rdx
    adcx rdx, rsi
    adcx rsi, rdi
    adcx rdi, r8
    adcx r8, r9
    adcx r9, r10
    adcx r10, r11
    adcx r11, r12
    adcx r12, r13
    adcx r13, r14
    adcx r14, r15

    ; ADCX - 64-bit Register to Memory (64-bit addressing)
    adcx rax, [rax]
    adcx rbx, [rbx + 10]
    adcx rcx, [rcx + rax*2 + 100]
    adcx rdx, [rdx + rbx*4 - 5]
    adcx rsi, [rsi + rdi*8 + 1000]
    adcx rdi, [r8 + 100]
    adcx r8, [r9 + r10*2 + 200]

    ; ADCX - 64-bit Register to Memory (32-bit addressing)
    adcx rax, [eax]
    adcx rbx, [150]
    adcx rcx, [ecx + eax*2 + 100]
    adcx rdx, [edx + ebx*4 - 5]
    adcx rsi, [esi + edi*8 + 1000]

    ; ADCX - 32-bit Register to Register
    adcx eax, ebx
    adcx eax, ecx
    adcx eax, edx
    adcx ebx, ecx
    adcx ecx, edx
    adcx edx, esi
    adcx esi, edi
    adcx edi, r8d
    adcx r8d, r9d
    adcx r9d, r10d
    adcx r10d, r11d
    adcx r11d, r12d
    adcx r12d, r13d
    adcx r13d, r14d
    adcx r14d, r15d

    ; ADCX - 32-bit Register to Memory (64-bit addressing)
    adcx eax, [rax]
    adcx ebx, [rbx + 10]
    adcx ecx, [rcx + rax*2 + 100]
    adcx edx, [rdx + rbx*4 - 5]
    adcx esi, [rsi + rdi*8 + 1000]
    adcx edi, [r8 + 100]
    adcx r8d, [r9 + r10*2 + 200]

    ; ADCX - 32-bit Register to Memory (32-bit addressing)
    adcx eax, [eax]
    adcx ebx, [ebx + 10]
    adcx ecx, [ecx + eax*2 + 100]
    adcx edx, [edx + ebx*4 - 5]
    adcx esi, [esi + edi*8 + 1000]

    ; ADOX - 64-bit Register to Register
    adox rax, rbx
    adox rax, rcx
    adox rax, rdx
    adox rbx, rcx
    adox rcx, rdx
    adox rdx, rsi
    adox rsi, rdi
    adox rdi, r8
    adox r8, r9
    adox r9, r10
    adox r10, r11
    adox r11, r12
    adox r12, r13
    adox r13, r14
    adox r14, r15

    ; ADOX - 64-bit Register to Memory (64-bit addressing)
    adox rax, [rax]
    adox rbx, [rbx + 10]
    adox rcx, [rcx + rax*2 + 100]
    adox rdx, [rdx + rbx*4 - 5]
    adox rsi, [rsi + rdi*8 + 1000]
    adox rdi, [r8 + 100]
    adox r8, [r9 + r10*2 + 200]

    ; ADOX - 64-bit Register to Memory (32-bit addressing)
    adox rax, [eax]
    adox rbx, [ebx + 10]
    adox rcx, [ecx + eax*2 + 100]
    adox rdx, [edx + ebx*4 - 5]
    adox rsi, [esi + edi*8 + 1000]

    ; ADOX - 32-bit Register to Register
    adox eax, ebx
    adox eax, ecx
    adox eax, edx
    adox ebx, ecx
    adox ecx, edx
    adox edx, esi
    adox esi, edi
    adox edi, r8d
    adox r8d, r9d
    adox r9d, r10d
    adox r10d, r11d
    adox r11d, r12d
    adox r12d, r13d
    adox r13d, r14d
    adox r14d, r15d

    ; ADOX - 32-bit Register to Memory (64-bit addressing)
    adox eax, [rax]
    adox ebx, [10]
    adox ecx, [rcx + rax*2 + 100]
    adox edx, [rdx + rbx*4 - 5]
    adox esi, [rsi + rdi*8 + 1000]
    adox edi, [r8 + 100]
    adox r8d, [r9 + r10*2 + 200]

    ; ADOX - 32-bit Register to Memory (32-bit addressing)
    adox eax, [eax]
    adox ebx, [ebx + 10]
    adox ecx, [ecx + eax*2 + 100]
    adox edx, [edx + ebx*4 - 5]
    adox esi, [esi + edi*8 + 1000]
