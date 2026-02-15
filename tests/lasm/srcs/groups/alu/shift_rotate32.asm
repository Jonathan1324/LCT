; FORMATS: BIN,ELF
; BITS: 32
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    shl al, cl
    shl ax, cl
    shl eax, cl
    shl byte [eax], cl
    shl word [eax], cl
    shl dword [eax], cl
    shl byte [bx], cl
    shl word [bx], cl
    shl dword [bx + si], cl
    
    shl al, 1
    shl ax, 2
    shl eax, 3
    shl byte [eax], 1
    shl word [eax], 2
    shl dword [eax], 3
    shl byte [bx], 2
    shl word [bx], 1
    shl dword [bx + si], 3
    
    shr al, cl
    shr ax, cl
    shr eax, cl
    shr byte [eax], cl
    shr word [eax], cl
    shr dword [eax], cl
    shr byte [bx], cl
    shr word [bx], cl
    shr dword [bx + si], cl
    
    shr al, 2
    shr ax, 1
    shr eax, 3
    shr byte [eax], 2
    shr word [eax], 1
    shr dword [eax], 3
    shr byte [bx], 2
    shr word [bx], 2
    shr dword [bx + si], 3
    
    sal al, cl
    sal ax, cl
    sal eax, cl
    sal byte [eax], cl
    sal word [eax], cl
    sal dword [eax], cl
    sal byte [bx], cl
    sal word [bx], cl
    sal dword [bx + si], cl
    
    sal al, 2
    sal ax, 2
    sal eax, 3
    sal byte [eax], 1
    sal word [eax], 2
    sal dword [eax], 3
    sal byte [bx], 2
    sal word [bx], 1
    sal dword [bx + si], 3
    
    sar al, cl
    sar ax, cl
    sar eax, cl
    sar byte [eax], cl
    sar word [eax], cl
    sar dword [eax], cl
    sar byte [bx], cl
    sar word [bx], cl
    sar dword [bx + si], cl
    
    sar al, 1
    sar ax, 2
    sar eax, 3
    sar byte [eax], 2
    sar word [eax], 2
    sar dword [eax], 3
    sar byte [bx], 2
    sar word [bx], 0
    sar dword [bx + si], 3
    
    rol al, cl
    rol ax, cl
    rol eax, cl
    rol byte [eax], cl
    rol word [eax], cl
    rol dword [eax], cl
    rol byte [bx], cl
    rol word [bx], cl
    rol dword [bx + si], cl
    
    rol al, 2
    rol ax, 1
    rol eax, 3
    rol byte [eax], 2
    rol word [eax], 2
    rol dword [eax], 1
    rol byte [bx], 2
    rol word [bx], 2
    rol dword [bx + si], 3
    
    ror al, cl
    ror ax, cl
    ror eax, cl
    ror byte [eax], cl
    ror word [eax], cl
    ror dword [eax], cl
    ror byte [bx], cl
    ror word [bx], cl
    ror dword [bx + si], cl
    
    ror al, 1
    ror ax, 2
    ror eax, 3
    ror byte [eax], 8
    ror word [eax], 2
    ror dword [eax], 1
    ror byte [bx], 2
    ror word [bx], 2
    ror dword [bx + si], 3
    
    rcl al, cl
    rcl ax, cl
    rcl eax, cl
    rcl byte [eax], cl
    rcl word [eax], cl
    rcl dword [eax], cl
    rcl byte [bx], cl
    rcl word [bx], cl
    rcl dword [bx + si], cl
    
    rcl al, 2
    rcl ax, 2
    rcl eax, 1
    rcl byte [eax], 2
    rcl word [eax], 2
    rcl dword [eax], 1
    rcl byte [bx], 2
    rcl word [bx], 2
    rcl dword [bx + si], 3
    
    rcr al, cl
    rcr ax, cl
    rcr eax, cl
    rcr byte [eax], cl
    rcr word [eax], cl
    rcr dword [eax], cl
    rcr byte [bx], cl
    rcr word [bx], cl
    rcr dword [bx + si], cl
    
    rcr al, 2
    rcr ax, 1
    rcr eax, 3
    rcr byte [eax], 2
    rcr word [eax], 1
    rcr dword [eax], 3
    rcr byte [bx], 2
    rcr word [bx], 2
    rcr dword [bx + si], 3
