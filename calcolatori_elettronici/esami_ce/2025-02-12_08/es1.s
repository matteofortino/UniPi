.global _ZN2clC1E3st1
.global _ZN2clC1ER3st1Pi
_ZN2clC1E3st1:
  push %rbp
  mov %rsp, %rbp
  subq $16, %rsp
  mov %rsi, -16(%rbp)
  mov %rdx, -8(%rbp)

  mov $0, %rax
loop:
  cmp $4, %rax
  je fine

  movl -16(%rbp, %rax, 4), %r9d
  movb %r9b, 32(%rdi, %rax, 1)

  shl %r9d
  movslq %r9d, %r10
  mov %r10, (%rdi, %rax, 8)

  # shl %r9d
  movb %r9b, 36(%rdi, %rax, 1)


  inc %rax
  jmp loop
fine :
  leave
  ret
_ZN2clC1ER3st1Pi:
  push %rbp
  mov %rsp, %rbp
  
  mov $0, %rax
loop_1:
  cmp $4, %rax
  je fine_1

  mov (%rsi, %rax, 4), %r9d

  movb %r9b, 32(%rdi, %rax, 1)

  shl $2, %r9d
  movslq %r9d, %r10
  mov %r10, (%rdi, %rax, 8)

  mov (%rdx, %rax, 4), %r9d 
  movb %r9b, 36(%rdi, %rax, 1)

  inc %rax
  jmp loop_1
fine_1:
  leave
  ret