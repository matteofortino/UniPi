.global _ZN2cl5elab1ER2sti
_ZN2cl5elab1ER2sti:
  pushq %rbp
  movq %rsp, %rbp

  movq $0, %rax 
loop:
  cmpq $4, %rax 
  je fine


  movq (%rsi, %rax, 8), %r9
  # movslq %edx, %rdx

  cmpq %r9, %rdx 
  jl fine_if

  movb 32(%rsi, %rax, 1), %r9b
  addb %r9b, 32(%rdi, %rax, 1)

  # movl %edx, %r9d 
  # subl %eax, %r9d
  # movslq %r9d, %r9

fine_if:
  movq %rdx, %r9
  subq %rax, %r9
  movq %r9, (%rdi, %rax, 8)

  incq %rax 
  jmp loop
fine:
  popq %rbp 
  ret