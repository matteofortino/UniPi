#include "../include/libce.h"

.global exc_mc
exc_mc:
	.cfi_startproc
	.cfi_def_cfa_offset 40
	.cfi_offset rip, -40
	.cfi_offset rsp, -16
	movq $18, %rdi
	movq $0, %rsi
	mov (%rsp), %rdx
	call gestore_eccezioni
	hlt
	.cfi_endproc
