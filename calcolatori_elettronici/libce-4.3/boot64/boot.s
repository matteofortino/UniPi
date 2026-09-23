/// @file boot.s
#include "../include/libce.h"
#include "mboot.h"

.text

#define STACK_SIZE                      0x4000
#define MULTIBOOT_HEADER_FLAGS         0x00000003

// Entry point del boot loader
// In questo sistema usiamo lo standard multiboot, che definisce il formato che
// il secondo loader deve rispettare e il formato delle informazioni passate
// dal primo loader al secondo.  Il formato del file contenente il secondo
// loader deve essere quello di un normale file eseguibile, ma, nei primi
// 2*4KiB, deve contenere la struttura multiboot_header, definita più avanti.
.global  _start
_start:
     .cfi_startproc
     jmp     .Lmultiboot_entry	// scavalchiamo la struttra richiesta
				// dal primo loader, che deve
				// trovarsi verso l'inizio del file
.align  4
// le seguenti informazioni sono richieste dal primo loader
.Lmultiboot_header:
     .long   MULTIBOOT_HEADER_MAGIC 				// valore magico
     .long   MULTIBOOT_HEADER_FLAGS   				// flag
     .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS) // checksum
     .long   .Lmultiboot_header
     .long   _start
     .long   _edata
     .long   _end
     .long   .Lmultiboot_entry

.Lmultiboot_entry:
     cli
     // cambiamo lo stack perché riutilizzeremo come parte dello heap
     // la memoria in cui si trova lo stack attuale
     movl    $stack_end, %esp

     pushl   %ebx			// parametri passati dal primo loader
     pushl   %eax
     call    disable_8259
     call    main			// il resto dell'inizializzazione
					// è scritto in C++
     call    reboot			// se main ritorna c'è stato un errore
     .cfi_endproc


/// @cond
.global gdt_init
gdt_init:
	.cfi_startproc
	push %ebp
	mov %esp, %ebp
	push %edi

	mov 8(%ebp), %edx		// &gdt -> edx
	// aggiorniamo il puntatore alla gdt che verra usato da
	// attiva_paginazione (istruzione lgdt)
	mov %edx, gdt64_ptr + 2
	// azzeriamo tutta la gdt
	  mov %edx, %edi
	  xor %eax, %eax
	  mov $((DIM_GDT_ENTRY*NUM_GDT_ENTRIES)>>2), %ecx
	  rep stosl
	// descrittore segmento codice sistema
	lea (ID_CODE_SYS*8)(%edx), %edi
	movb $0b10011011, 5(%edi)	// P=1, DPL=sys, S=cod/dati, tipo=codice
	movb $0b00100000, 6(%edi)	// L=1 (64 bit)
	// descrittore segmento codice utente
	lea (ID_CODE_USR*8)(%edx), %edi
	movb $0b11111011, 5(%edi)	// P=1, DPL=usr, S=cod/dati, tipo=codice
	movb $0b00100000, 6(%edi)	// L=1 (64 bit)
	// descrittore segmento pila/dati utente
	lea (ID_DATA_USR*8)(%edx), %edi
	movb $0b11110011, 5(%edi)	// P=1, DPL=usr, S=cod/dati, tipo=dati scrivibile
					// (l'amd64 ignora l'attributo scrivible, ma lo
					// settiamo lo stesso per documentazione)
	// descrittore segmento TSS
	// qui base e limite sono significativi e devono corrispondere
	// all'indirizzo e alla dimensione del segmento TSS
	lea (ID_SYS_TSS*8)(%edx), %edi
	movl 12(%ebp), %eax		// &tss -> eax
	movw %ax, 2(%edi)		// base[0-15]
	shrl $16, %eax
	movb %al, 4(%edi)		// base[16-23]
	movb %ah, 7(%edi)		// base[24-31]
	// non scriviamo in base[61-32] perché il nostro TSS è sicuramente
	// nella parte bassa della memoria
	movw $(DIM_TSS-1), (%edi)	// limit (sizeof(TSS)-1)
	movb $0b10001001, 5(%edi)	// P=1, DPL=sys, S=sistema, tipo=TSS

	pop %edi
	leave
	ret
	.cfi_endproc

.global tss_init
tss_init:
	.cfi_startproc
	push %ebp
	mov %esp, %ebp
	push %edi
	mov 8(%ebp), %edx	// &tss -> edx
	// azzeriamo tutto il TSS
	  mov %edx, %edi
	  xor %eax, %eax
	  mov $(DIM_TSS>>2), %ecx
	  rep stosl
	// scriviamo la dimensione del TSS nell'offset della iobitmap
	movw $DIM_TSS, (DIM_TSS - 2)(%edx)
	// restituiamo l'indirizzo del campo RSP0
	lea 4(%edx), %eax
	xor %edx, %edx
	pop %edi
	leave
	ret
	.cfi_endproc

/// @cond
.global attiva_paginazione
// attiva la paginazione
attiva_paginazione:
	// attiviamo PAE (Physical Address Extensions) settando il bit 4 di CR4
	movl %cr4, %eax
        orl $0x00000020, %eax    // cr4.pae
	// opzionalmente abilitiamo anche LA57 settando il bit 12 di CR4.
	cmpl $4, 12(%esp)
	je 1f
	orl $0x00001000, %eax	 // cr4.la57
1:	movl %eax, %cr4

	/// settiamo il bit 15 del registro EFER
        movl $0xC0000080, %ecx   // efer msr
        rdmsr
        orl $0x00000100, %eax    // efer.lme
        wrmsr

	// settiamo il bit 31 di CR0
        movl %cr0, %eax
        orl $0x80010000, %eax    // paging & write-protect
        movl %eax, %cr0
	// ora la modalità a 64 bit è attiva, ma il segmento corrente usa
	// ancora operandi a 32 bit. Dobbiamo caricare in CS il selettore del
	// segmento di codice sistema che aveamo preparato nella GDT. Prima
	// però dobbiamo caricare nel processore l'indirizzo della nostra GDT
	// (istruzione lgdt):
	lgdt gdt64_ptr
	// Non possiamo caricare CS con una movw, ma possiamo eseguire un "far
	// jump", cioè un salto in cui si specifica un selettore di segmento
	// oltre all'offset di destinazione
	ljmp $SEL_CODICE_SISTEMA, $long_mode

	// l'assemblatore sta assemblando questo file assumendo che si tratti
	// di codice a 32 bit. Con ".code64" gli chiediamo di assemblare il
	// resto con istruzioni a 64 bit
.code64
long_mode:
	// azzeriamo tutti i flag (tranne il bit 1 che per motivi storici deve
	// essere sempre settato)
	pushq $2
	popfq
	// azzeriamo tutti i gli altri registri selettori di segmento
	movw $0, %ax
	movw %ax, %ds
	movw %ax, %ss
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %es
	// carichiamo il selettore del segmento TSS nel registro speciale TR.
	// Il meccanismo delle interruzioni usa questo registro per accedere al
	// campo del TSS che contiene l'indirizzo della pila sistema.
	movw $SEL_SYS_TSS, %ax
	ltr %ax

	// se era stata richiesta la modalità di debug aspettiamo che gdb si
	// colleghi. Lo script 'debug' usa gdb per scrivere 0 in wait_for_gdb,
	// in modo da fare uscire il boot loader da questo ciclo e fargli
	// cedere il controllo al primo modulo.
wait:
	cmpl $1, wait_for_gdb(%rip)
	pause
	je wait

	// cediamo il controllo al primo modulo
	movl 4(%rsp), %edi	// boot64_info*
	movl 8(%rsp), %eax	// entry point
	movl $stack_end, %esp	// riallinea la pila
	jmp *%rax

//////////////////
// sezione dati //
//////////////////
.code32
.data
// gdt64_ptr è usato dall'istruzione lgdt per caricare il registro GDTR con la
// base e il limite della GDT. Il limite si trova nei primi due byte e la base
// nei successivi 4 byte.
gdt64_ptr:
	.word (DIM_GDT_ENTRY*NUM_GDT_ENTRIES)-1
	.long 0		// verrà impostato a runtime
.bss
.align 16
	.global stack
stack:
	.space STACK_SIZE, 0
stack_end:
/// @endcond
