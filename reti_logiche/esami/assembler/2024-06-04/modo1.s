.include "files/utility.s"
.global _main

.text 
_main:
    nop
    mov $0, %ecx
    call indecimal_word
    mov %ax, %bx
    call newline
    call indecimal_word
    mov %ax, %cx
    mov $0, %eax
    // finisce in ax
loop:
    add %ecx, %eax
    dec %bx
    cmp $0, %bx
    je fine
    jmp loop

fine:
    call newline
    call outdecimal_long
    ret
