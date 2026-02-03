.global _ZN2clC1EcR3st2
_ZN2clC1EcR3st2:
  pushq %rbp
  movq %rsp, %rbp


  movq $0, %rax
loop: 
  cmpq $4, %rax 
  je fine

  movb %sil, (%rdi, %rax, 1)
  movb %sil, 40(%rdi, %rax, 1)
  incb %sil

  movb (%rdx, %rax, 1), %r9b 
  addb 40(%rdi, %rax, 1), %r9b
  movsbq %r9b, %r9
  movq %r9, 8(%rdi, %rax, 8)


  incq %rax 
  jmp loop
fine:
  popq %rbp
  ret

