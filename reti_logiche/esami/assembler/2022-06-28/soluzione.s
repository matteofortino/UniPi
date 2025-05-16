.include "files/utility.s"
.global _main

.data
x: .long 0
n: .byte 0
msg: .asciz "RADICE NON NATURALE\r"

.text
_main:
  nop
  call indecimal_long
  call newline
  cmp $1, %eax
  jbe fine
  mov %eax, x
  call indecimal_byte
  call newline
  cmp $1, %al
  jbe fine
  mov %al, n

  mov $2, %ebx
loop_esterno:
  mov %ebx, %eax
  mov $1, %cl
loop_interno:
  cmp n, %cl
  je fine_interno

  mul %ebx
  cmp $0, %edx
  jnz non_naturale
  cmp x, %eax
  ja non_naturale

  inc %cl
  jmp loop_interno

fine_interno:
  cmp x, %eax
  je stampa

  inc %bx
  jmp loop_esterno


stampa:
  mov %bx, %ax
  call outdecimal_word
  call newline
  jmp fine

non_naturale:
  lea msg, %ebx 
  call outline
  call newline

fine:
  ret
