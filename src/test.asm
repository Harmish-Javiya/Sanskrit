global _main

section .data
    msg db "namaste duniya", 10, 0
    msglen equ $ - msg

section .text
_main:
    ; print "namaste duniya"
    mov rax, 0x2000004  ; write syscall
    mov rdi, 1          ; stdout
    mov rsi, msg        ; message
    mov rdx, msglen     ; length
    syscall

    ; exit
    mov rax, 0x2000001  ; exit syscall
    mov rdi, 0          ; exit code 0
    syscall
