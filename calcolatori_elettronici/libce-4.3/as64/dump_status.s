read_stack:
	.cfi_startproc
	movq (%rsi), %rax
	ret
	.cfi_endproc

	.global dump_status
dump_status:
	.cfi_startproc
	.cfi_def_cfa_offset 8
	push (%rsp)
	.cfi_adjust_cfa_offset 8
	subq $1, (%rsp)
	pushf
	.cfi_adjust_cfa_offset 8
	push $0
	.cfi_adjust_cfa_offset 8
	push %r15
	.cfi_adjust_cfa_offset 8
	push %r14
	.cfi_adjust_cfa_offset 8
	push %r13
	.cfi_adjust_cfa_offset 8
	push %r12
	.cfi_adjust_cfa_offset 8
	push %r11
	.cfi_adjust_cfa_offset 8
	push %r10
	.cfi_adjust_cfa_offset 8
	push %r9
	.cfi_adjust_cfa_offset 8
	push %r8
	.cfi_adjust_cfa_offset 8
	push %rsp
	.cfi_adjust_cfa_offset 8
	addq $(13*8), (%rsp)
	push %rbp
	.cfi_adjust_cfa_offset 8
	push %rdi
	.cfi_adjust_cfa_offset 8
	push %rsi
	.cfi_adjust_cfa_offset 8
	push %rbx
	.cfi_adjust_cfa_offset 8
	push %rcx
	.cfi_adjust_cfa_offset 8
	push %rdx
	.cfi_adjust_cfa_offset 8
	push %rax
	.cfi_adjust_cfa_offset 8
	xor %rax, %rax
	mov %cs, %ax
	push %rax
	.cfi_adjust_cfa_offset 8
	leaq read_stack(%rip), %rax
	push %rax
	.cfi_adjust_cfa_offset 8
	push $0
	.cfi_adjust_cfa_offset 8
	push $0
	.cfi_adjust_cfa_offset 8
	push $0
	.cfi_adjust_cfa_offset 8
	mov %rdi, %rsi
	mov %rsp, %rdi
	call cfi_dump
	add $(24*8), %rsp
	.cfi_adjust_cfa_offset 24*8
	ret
	.cfi_endproc
