#include "../include/libce.h"

.global exc_ac
exc_ac:
	.cfi_startproc
	.cfi_def_cfa_offset 40
	.cfi_escape CE_FAULT
	.cfi_offset rip, -40
	.cfi_offset rsp, -16
	movq $17, %rdi
	movq $0, %rsi
	mov (%rsp), %rdx
	call gestore_eccezioni
	hlt
	.cfi_endproc

