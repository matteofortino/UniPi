.include "files/utility.s"
.global _main


.text
_main: 
    nop
    mov $0, %eax
    mov $0, %esi
    call indecimal_word
    mov %ax, %bx
    call newline
    call indecimal_word
    mov %ax, %cx
prima_parte:
    mov %cl, %al
    mul %bl

    adc %eax, %esi

    mov %ch, %al
    mul %bl

    shl $8, %eax
    add %eax, %esi

    mov %cl, %al
    mul %bh

    add %eax, %esi

    mov %ch, %al
    mul %bh

    shl $16, %eax
    add %eax, %esi

    mov %esi, %eax

    call newline
    call outdecimal_long

    ret
