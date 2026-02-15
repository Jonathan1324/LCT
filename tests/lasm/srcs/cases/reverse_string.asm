; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .bss
    buffer resb 256        ; buffer for input

section .data
    prompt db "Enter a string: ", 0
    prompt_len equ $-prompt
    newline db 10          ; linefeed character

section .text
    global _start

_start:
    ; --- write prompt ---
    mov rax, 1             ; sys_write
    mov rdi, 1             ; file descriptor: stdout
    mov rsi, prompt        ; pointer to prompt string
    mov rdx, prompt_len    ; length of prompt
    syscall

    ; --- read input ---
    mov rax, 0             ; sys_read
    mov rdi, 0             ; file descriptor: stdin
    mov rsi, buffer        ; pointer to buffer
    mov rdx, 255           ; maximum bytes to read
    syscall
    mov r8, rax            ; r8 = number of bytes read (including newline)
    dec r8                 ; r8 = length without newline (for output)
    mov rcx, r8            ; rcx = length for reverse loop

    ; --- reverse string ---
    xor rbx, rbx           ; index from start
    dec rcx                 ; rcx = last character index
reverse_loop:
    cmp rbx, rcx
    jge done_reverse
    ; swap buffer[rbx] and buffer[rcx]
    mov al, [buffer + rbx]
    mov dl, [buffer + rcx]
    mov [buffer + rbx], dl
    mov [buffer + rcx], al
    inc rbx
    dec rcx
    jmp reverse_loop
done_reverse:

    ; --- write reversed string ---
    mov rax, 1             ; sys_write
    mov rdi, 1             ; stdout
    mov rsi, buffer        ; pointer to buffer
    mov rdx, r8            ; length of string (saved original length)
    syscall

    ; --- write newline ---
    mov rax, 1
    mov rdi, 1             ; stdout
    mov rsi, newline
    mov rdx, 1
    syscall

    ; --- exit ---
    mov rax, 60            ; sys_exit
    xor rdi, rdi           ; exit code 0
    syscall
