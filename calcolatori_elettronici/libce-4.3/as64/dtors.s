	.global ctors
ctors:
	// Il C++ prevede che si possa eseguire del codice prima di main (per
	// es. nei costruttori degli oggetti globali). gcc organizza questo
	// codice in una serie di funzioni di cui raccoglie i puntatori
	// nell'array __init_array_start. Il C++ run time deve poi chiamare
	// tutte queste funzioni prima di saltare a main.  Poiché abbiamo
	// compilato il modulo con -nostdlib dobbiamo provvedere da soli a
	// chiamare queste funzioni, altrimenti gli oggetti globali non saranno
	// inizializzati correttamente.
	.cfi_startproc
	.cfi_def_cfa_offset 8
	pushq %rbx
	.cfi_offset rbx, -16
	.cfi_adjust_cfa_offset 8
	pushq %r12
	.cfi_adjust_cfa_offset 8
	.cfi_offset r12, -24
	movabs $__init_array_start, %rbx
	movabs $__init_array_end, %r12
1:	cmpq %r12, %rbx
	je 2f
	call *(%rbx)
	addq $8, %rbx
	jmp 1b
2:	popq %r12
	.cfi_adjust_cfa_offset -8
	.cfi_restore r12
	popq %rbx
	.cfi_adjust_cfa_offset -8
	.cfi_restore rbx
	ret
	.cfi_endproc
