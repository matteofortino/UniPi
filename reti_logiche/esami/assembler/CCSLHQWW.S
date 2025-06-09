# 1 "ASSEMBLY.S"
# 1 "c:///"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "c:/files/gas/bin/../lib/gcc/djgpp/4.10/djgpp.ver" 1
# 1 "c:/files/gas/include/sys/version.h" 1 3 4
# 2 "c:/files/gas/bin/../lib/gcc/djgpp/4.10/djgpp.ver" 2
# 1 "<command line>" 2
# 1 "ASSEMBLY.S"
.include "files/utility.s"
.global _main

.text
_main:
    nop
    mov $0, %eax
    call indecimal_word
    mov %ax, %bx
    call newline
    call indecimal_word

loop:
    add %eax, %eax
    dec %bx
    cmp $1, %bx
    je fine
    jmp loop

fine:
    ret
