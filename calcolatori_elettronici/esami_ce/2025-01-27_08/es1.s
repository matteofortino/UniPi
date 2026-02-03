.global _ZN2cl5elab1ER3st13st2
_ZN2cl5elab1ER3st13st2:
  push %rbp
  mov %rsp, %rbp  
  sub $56, %rsp

  mov %rdx, -56(%rbp)
  mov %rcx, -48(%rbp)

  push %rdi
  lea -40(%rbp), %rdi
  call _ZN2clC1EPc3st2
  pop %rdi

  mov $0, %rax
loop:
  cmp $4, %rax 
  je fine
  
  movb (%rsi, %rax, 1), %r9b
  cmp %r9b, (%rdi, %rax, 1)
  jge fine_if

  movb -40(%rbp, %rax, 1), %r9b
  movb %r9b, (%rdi, %rax, 1)

  movq -32(%rbp, %rax, 8), %r9
  add %r9, 8(%rdi, %rax, 8)


fine_if:

  inc %rax 
  jmp loop
fine:
  leave
  ret
