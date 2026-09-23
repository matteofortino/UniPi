.include "files/utility.s"
.global _main
.data
fuori: .asciz "FUORI\r"
dentro: .asciz "DENTRO\r"

.text
_main:
    nop
    call inword
    call newline 
    mov %ax, %bx // x -> bx
    call inword
    call newline
    mov %ax, %cx // y -> cx


    cmp $0, %bx
    jge prossimo
    neg %bx
prossimo:
    cmp $0, %cx
    je fine
    jg dopo
    neg %cx

dopo:
    cmp $500, %bx
    ja fine
    cmp $500, %cx
    ja fine


    mov %bx, %ax 
    add %cx, %ax 

    cmp $64, %ax 
    jb fuori_msg
    cmp $128, %ax
    ja fuori

    lea dentro, %ebx
    call outline
    call newline
    jmp again

fuori_msg:
    lea fuori, %ebx
    call outline
    call newline

again:
    jmp _main
fine: 
    ret


