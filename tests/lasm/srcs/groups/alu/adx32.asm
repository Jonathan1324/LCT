; FORMATS: BIN,ELF
; BITS: 32
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    ; ADCX - 32-bit Register to Register
    adcx eax, ebx
    adcx eax, ecx
    adcx eax, edx
    adcx ebx, ecx
    adcx ecx, edx
    adcx edx, esi
    adcx esi, edi
    adcx edi, eax

    ; ADCX - 32-bit Register to Memory (32-bit addressing)
    adcx eax, [eax]
    adcx ebx, [ebx + 10]
    adcx ecx, [ecx + eax*2 + 100]
    adcx edx, [edx + ebx*4 - 5]
    adcx esi, [esi + edi*8 + 1000]
    adcx edi, [ebp + 100]
    adcx eax, [esp + eax*2 + 200]

    ; ADCX - 32-bit Register to Memory (16-bit addressing)
    adcx eax, [bx]
    adcx ebx, [150]
    adcx ecx, [bx + si]
    adcx edx, [bp + di - 5]
    adcx esi, [bx + di + 1000]

    ; ADOX - 32-bit Register to Register
    adox eax, ebx
    adox eax, ecx
    adox eax, edx
    adox ebx, ecx
    adox ecx, edx
    adox edx, esi
    adox esi, edi
    adox edi, eax

    ; ADOX - 32-bit Register to Memory (64-bit addressing)
    adox eax, [eax]
    adox ebx, [10]
    adox ecx, [ecx + eax*2 + 100]
    adox edx, [edx + ebx*4 - 5]
    adox esi, [esi + edi*8 + 1000]
    adox edi, [esp + 100]
    adox eax, [edi + ebp*2 + 200]

    ; ADOX - 32-bit Register to Memory (32-bit addressing)
    adox eax, [bx]
    adox ebx, [150]
    adox ecx, [bx + si]
    adox edx, [bp + di - 5]
    adox esi, [bx + di + 1000]
