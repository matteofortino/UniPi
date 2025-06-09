.include "files/utility.s"
.global _main

.data
n: .byte 0
matrice: .fill 65025, 1, 0

inserire_n: .asciz "Inserire n:\r"
inserire_matrice: .asciz "Inserire Matrice:\r"

dominanza_diagonale:   .ascii "La matrice e' diagonalmente dominante.\r"
no_dominanza_diagonale:   .ascii "La matrice non e' diagonalmente dominante.\r"
.text
_main:
    nop
    lea inserire_n, %ebx
    call outline
    call indecimal_byte
    cmp $0, %al
    je fine
    mov %al, n
    call newline

    lea inserire_matrice, %ebx
    call outline


    
    lea matrice, %edi
    mov $0, %bl
    mov $0, %esi
loop_esterno:
    cmp n, %bl 
    je fine_loop_esterno
    
    mov $0, %cl
loop_interno:
    cmp n, %cl
    je fine_loop_interno
corpo:
    call indecimal_word
    mov %al, (%edi, %esi, 1);
    inc %esi
    mov $'\t', %al
    call outchar

    inc %cl
    jmp loop_interno
fine_loop_interno:
    call newline
    inc %bl
    jmp loop_esterno
fine_loop_esterno:

    mov $0, %eax
    mov $0, %ebx
    mov $0, %ecx
    mov $0, %edx
    mov $0, %esi
    mov $0, %edi

    lea matrice, %edi

d_loop_esterno:
    cmp n, %bl 
    je d_fine_loop_esterno

    mov %bl, %cl    
    mov (%edi, %ecx, 1), %dl

    mov $0, %ebp
    mov $0, %cl
d_loop_interno:
    cmp n, %cl
    je d_fine_loop_interno

    mov (%edi, %esi, 1), %al
    inc %esi

    cmp %bl, %cl
    je poi

    add %eax, %ebp
poi:
    inc %cl
    jmp d_loop_interno

d_fine_loop_interno:
    cmp %edx, %ebp
    jb no_dominanza
    inc %bl
    jmp d_loop_esterno
d_fine_loop_esterno:

dominanza:
    lea dominanza_diagonale, %ebx
    call outline
    jmp fine
no_dominanza:
    lea no_dominanza_diagonale, %ebx
    call outline
    jmp fine


fine:
    ret


