.global _start
.text
// entry point dei programmi bare
_start:
	.cfi_startproc
	// smettiamo di usare lo stack del boot loader
	mov $stack_end, %rsp
	// Il boot loader ha scritto un po' di informazioni in una struttura
	// boot64_info e ne ha passato il puntatore in %rdi. Lo salviamo nella
	// variabile globale boot_info
	mov %rdi, boot_info(%rip)
	// Il primo campo della struttura è il puntatore allo heap creato dal
	// boot loader. Inizializziamo lo heap del programma con lo stesso
	// puntatore, così possiamo continuare ad usare quella memoria senza
	// sovrascrivere le strutture dati allocate dal boot loader (tabelle
	// della finestra di memoria, TSS, GDT, boot64_info).
	movq (%rdi), %rax
	movq %rax, _ZN4heap9memliberaE(%rip)
	// Il secondo campo della struttura è un puntatore al capo punt_nucleo
	// del TSS allocato dal boot loader. Lo salviamo nella variabile
	// globale tss_punt_nucleo.
	movq 8(%rdi), %rax
	movq %rax, tss_punt_nucleo(%rip)
	// Inizializziamo per prima la IDT. In questo modo installiamo in
	// particolare i gestori delle eccezioni, per poter intercettare
	// eventuali errori già nella fase di inizializzazione
	call init_idt
	// Chiamiamo eventuali "costruttori", cioè costruttori di oggetti
	// C++ dichiarati a livello globale, oppure funzioni annotate con
	// __attribute__((constructor)).
	call ctors
	// Cediamo il controllo alla funzione main del programma bare
        call main
	// Chiamiamo eventuali "distruttori", cioè distruttori di oggetti
	// C++ dichiarati a livello globale, oppure funzioni annotate con
	// __attribute__((destructor)).
	call dtors
	// Infine, eseguiamo lo shutdown della macchina
	call end_program
	.cfi_endproc
