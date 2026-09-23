// carica un gate nella IDT
// parametri:
// num: indice (a partire da 0) in IDT del gate da caricare
// routine: indirizzo della routine da associare al gate
.macro carica_gate num routine
	movq $\routine, %rdi
	xorl %esi, %esi
	xorl %edx, %edx
	call componi_gate
	movq %rax, idt + \num*16
	movq %rdx, idt + \num*16 + 8
.endm


.global init_idt
init_idt:
	.cfi_startproc
        pushq %rcx
	.cfi_adjust_cfa_offset 8
        pushq %rax
	.cfi_adjust_cfa_offset 8
        pushq %rdi
	.cfi_adjust_cfa_offset 8

        movabsq $idt, %rdi
        movq $512, %rcx
        movq $0, %rax
        cld
        rep stosq

	carica_gate	0 		exc_div_error
	carica_gate	1 		exc_debug
	carica_gate	2 		exc_nmi
	carica_gate	3 		exc_breakpoint
	carica_gate	4 		exc_overflow
	carica_gate	5 		exc_bound_re
	carica_gate	6 		exc_inv_opcode
	carica_gate	7 		exc_dev_na
	carica_gate	8 		exc_dbl_fault
	carica_gate	9 		exc_coproc_so
	carica_gate	10 		exc_inv_tss
	carica_gate	11 		exc_segm_fault
	carica_gate	12 		exc_stack_fault
	carica_gate	13 		exc_prot_fault
	carica_gate	14 		exc_page_fault
	carica_gate	16 		exc_fp
	carica_gate	17 		exc_ac
	carica_gate	18 		exc_mc
	carica_gate	19 		exc_simd

        lidt    idt_pointer                // carichiamo la nuova IDT

	sti

        popq %rdi
	.cfi_adjust_cfa_offset -8
        popq %rax
	.cfi_adjust_cfa_offset -8
        popq %rcx
	.cfi_adjust_cfa_offset -8
        ret
	.cfi_endproc
