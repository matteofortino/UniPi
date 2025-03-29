#include "../include/libce.h"

.global exc_page_fault
exc_page_fault:
	.cfi_startproc
	.cfi_def_cfa_offset 48
	.cfi_escape CE_FAULT
	.cfi_offset rip, -40
	.cfi_offset rsp, -16
	movq $14, %rdi
	popq %rsi
	.cfi_adjust_cfa_offset -8
	mov (%rsp), %rdx
	call gestore_eccezioni
	hlt
	.cfi_endproc
