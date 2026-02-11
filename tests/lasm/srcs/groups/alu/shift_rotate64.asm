; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    shl al, cl
    shl ax, cl
    shl eax, cl
    shl rax, cl
    
    shl byte [rax], cl
    shl word [rax], cl
    shl dword [rax], cl
    shl qword [rax], cl
    
    shl al, 2
    shl ax, 2
    shl eax, 3
    shl rax, 4
    
    shl byte [rax], 2
    shl word [rax], 2
    shl dword [rax], 3
    shl qword [rax], 4
    
    shr al, cl
    shr ax, cl
    shr eax, cl
    shr rax, cl
    
    shr byte [rax], cl
    shr word [rax], cl
    shr dword [rax], cl
    shr qword [rax], cl
    
    shr al, 2
    shr ax, 2
    shr eax, 3
    shr rax, 4
    
    shr byte [rax], 2
    shr word [rax], 2
    shr dword [rax], 3
    shr qword [rax], 4
    
    sal al, cl
    sal ax, cl
    sal eax, cl
    sal rax, cl
    
    sal byte [rax], cl
    sal word [rax], cl
    sal dword [rax], cl
    sal qword [rax], cl
    
    sal al, 2
    sal ax, 2
    sal eax, 3
    sal rax, 4

    sal byte [rax], 2
    sal word [rax], 2
    sal dword [rax], 3
    sal qword [rax], 4
    
    sar al, cl
    sar ax, cl
    sar eax, cl
    sar rax, cl
    
    sar byte [rax], cl
    sar word [rax], cl
    sar dword [rax], cl
    sar qword [rax], cl
    
    sar al, 2
    sar ax, 2
    sar eax, 3
    sar rax, 4
    
    sar byte [rax], 2
    sar word [rax], 2
    sar dword [rax], 3
    sar qword [rax], 4
    
    rol al, cl
    rol ax, cl
    rol eax, cl
    rol rax, cl
    
    rol byte [rax], cl
    rol word [rax], cl
    rol dword [rax], cl
    rol qword [rax], cl
    
    rol al, 2
    rol ax, 2
    rol eax, 3
    rol rax, 4
    
    rol byte [rax], 2
    rol word [rax], 2
    rol dword [rax], 3
    rol qword [rax], 4
    
    ror al, cl
    ror ax, cl
    ror eax, cl
    ror rax, cl
    
    ror byte [rax], cl
    ror word [rax], cl
    ror dword [rax], cl
    ror qword [rax], cl
    
    ror al, 2
    ror ax, 2
    ror eax, 3
    ror rax, 4
    
    ror byte [rax], 2
    ror word [rax], 2
    ror dword [rax], 3
    ror qword [rax], 4
    
    rcl al, cl
    rcl ax, cl
    rcl eax, cl
    rcl rax, cl
    
    rcl byte [rax], cl
    rcl word [rax], cl
    rcl dword [rax], cl
    rcl qword [rax], cl
    
    rcl al, 2
    rcl ax, 2
    rcl eax, 3
    rcl rax, 4
    
    rcl byte [rax], 2
    rcl word [rax], 2
    rcl dword [rax], 3
    rcl qword [rax], 4
    
    rcr al, cl
    rcr ax, cl
    rcr eax, cl
    rcr rax, cl
    
    rcr byte [rax], cl
    rcr word [rax], cl
    rcr dword [rax], cl
    rcr qword [rax], cl
    
    rcr al, 2
    rcr ax, 2
    rcr eax, 3
    rcr rax, 4
    
    rcr byte [rax], 2
    rcr word [rax], 2
    rcr dword [rax], 3
    rcr qword [rax], 4
