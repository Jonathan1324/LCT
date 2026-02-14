; FORMATS: ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .bss
    name resb 128             ; buffer for user input

section .data
    ask_name db "What's your name? ", 0
    fmt_hello db "Hello, %s", 10, 0   ; "%s\n" format string

section .text
    global main
    extern printf
    extern fgets
    extern stdin

main:
    ; --- call printf("What's your name? ") ---
    mov rdi, ask_name       ; format string
    xor rax, rax            ; no xmm registers used
    call printf

    ; --- call fgets(name, 128, stdin) ---
    mov rdi, name           ; buffer
    mov rsi, 128            ; size
    mov rdx, [stdin]        ; FILE* stdin
    call fgets

    ; --- call printf("Hello, %s\n", name) ---
    mov rdi, fmt_hello      ; format string
    mov rsi, name           ; argument for %s
    xor rax, rax
    call printf

    ; --- exit(0) ---
    mov rax, 60             ; sys_exit
    xor rdi, rdi
    syscall
