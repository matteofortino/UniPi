#include "../include/libce.h"

.global componi_gate
# input:
#   rdi: indirizzo routine
#   esi: trap(1)/interrupt(0)
#   edx: DPL (utente=3/sistema=0)
#
# output:
#
#  +---+---+---+---+---+---+---+---+
#  |o31:16 | A | 0 |  sel  | o15:0 |	%rax
#  +---+---+---+---+---+---+---+---+
#  |       0       |     o63:32    |	%rdx
#  +---+---+---+---+---+---+---+---+
#
# dove:
#   a = indirizzo routine (da rdi)
#   A = byte di accesso (in base a esi e edx)
#   sel = SEL_CODICE_SISTEMA (non usiamo gate utente)
componi_gate:
	.cfi_startproc
	movl %edi, %eax  		// o31:16 -> eax31:16
	movb $0, %al
	movb $0b10001110, %ah 	        // byte di accesso (presente, 64bit, tipo interrupt)
	cmpl $0, %esi			// richiesto tipo trap?
	je 1f				// salta se no
	orb $1, %ah			// altrimenti cambia il tipo da interrupt a trap
1:	shlb $5, %dl			// sposta i 2 bit del DPL nella loro posizione in A
	orb %dl, %ah			// scrittura del DPL nel byte di accesso
	shlq $16, %rax			// fai spazio per sel
	movw $SEL_CODICE_SISTEMA, %ax	// scrittura di sel
	shlq $16, %rax			// sposta tutto nella posizione finale in %rax
	movw %di, %ax			// scrittura di o15:0
	shrq $32, %rdi
	movl %edi, %edx			// o64:32 in %edx (parte alta automaticamente azzerata)
	ret
	.cfi_endproc
