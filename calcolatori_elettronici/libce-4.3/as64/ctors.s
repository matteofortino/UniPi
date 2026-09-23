	.global dtors
dtors:
	.cfi_startproc
	.cfi_def_cfa_offset 8
	pushq %rbx
	.cfi_offset rbx, -16
	.cfi_adjust_cfa_offset 8
	pushq %r12
	.cfi_adjust_cfa_offset 8
	.cfi_offset r12, -24
	movabs $__fini_array_start, %rbx
	movabs $__fini_array_end, %r12
1:	cmpq %r12, %rbx
	je 2f
	call *(%rbx)
	addq $8, %rbx
	jmp 1b
	call run_dtors
2:	popq %r12
	.cfi_adjust_cfa_offset -8
	.cfi_restore r12
	popq %rbx
	.cfi_adjust_cfa_offset -8
	.cfi_restore rbx
	ret
	.cfi_endproc
