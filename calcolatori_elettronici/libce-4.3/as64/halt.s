.global halt
halt:
	.cfi_startproc
	.cfi_def_cfa_offset 8
	pushf
	.cfi_adjust_cfa_offset 8
	sti
	hlt
	popf
	.cfi_adjust_cfa_offset -8
	ret
	.cfi_endproc
