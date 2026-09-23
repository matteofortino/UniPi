/// @file boot.cpp
#ifdef __x86_64__
#error "this must be compiled for 32 bit"
#endif
#include "mboot.h"
#define MAX_LOG LOG_ERR
#include "../include/libce.h"
#include "elf64.h"
#include "../include/vm.h"
#include "../include/boot.h"

/// @defgroup boot Bootstrap da 32 a 64 bit
///
/// Il boostrap avviene in due stadi.
///
/// STADIO 1 (realizzato dal bootloader incluso in QEMU)
/// ----------------------------------------------------
///
/// Carica in memoria il secondo bootloader e eventuali moduli. Il secondo boot
/// loader deve essere un file ELF e viene caricato agli indirizzi specificati
/// nella sua tabella di programma (normalmente nel secondo MiB), mentre i
/// moduli possono essere file generici e vengono semplicemente copiati in RAM
/// (subito dopo il secondo boot loader).  Il primo boot loader porta il
/// processore da modo reale (16 bit) a modo protetto a 32 bit, quindi cede il
/// controllo al secondo boot loader. Passa nel registro EAX una costante che
/// permette al secondo boot loader di capire di essere stato avviato da un boot
/// loader che rispetta lo standard "multiboot".  Nel registro EBX passa
/// l'indirizzo di una struttura multiboot_info_t che contiene varie
/// informazioni (memoria RAM installata nel sistema, indirizzi in RAM delle
/// copie dei moduli, ...). La struttura è definita in "mboot.h" in questa
/// directory.
///
/// Stato della memoria alla fine dello stadio 1:
/// - primo MiB: contiene la struttura multiboot_info_t
/// - secondo MiB: contiene il secondo boot loader (caricato ai suoi indirizzi)
///    e i moduli semplicemente copiati in memoria
///
/// STADIO 2 (realizzato da questo programma)
/// ----------------------------------------
///
/// Lo stadio 1 deve aver caricato almeno un modulo e questo modulo deve essere
/// di tipo ELF. Il secondo bootloader interpreta questo modulo e copia i
/// segmenti ELF al loro indirizzo di caricamento (che deve partire dal terzo
/// MiB in poi per non sovrascrivere il boot loader stesso e i moduli). Porta
/// poi la CPU nella modalità a 64 bit e cede il controllo al primo modulo.
/// Passa in EDI il puntatore ad una struttura boot64_info che contiene varie
/// informazioni (in particolare gli indirizzi dei moduli, che si trovano ancora
/// in memoria dove erano stati caricati dal primo boot loader).
///
/// Stato della memoria alla fine dello stadio 2:
/// - primo MiB: usato come heap dal secondo boot loader, contiene alcune
///   strutture dati che devono essere usate anche in seguito (GDT, TSS,
///   boot64_info, tabelle che compongono il TRIE della finestra FM) e
///   parti libere. L'indirizzo della prima parte libera viene passato
///   al primo modulo tramite boot64_info->memlibera in modo che lo heap
///   possa essere riutilizzato
/// - secondo MiB: si trova nello stesso stato lasciato dal primo boot loader
/// - terzo MiB: primo modulo caricato ai suoi indirizzi
///
/// @{

/// @defgroup boothw  Strutture dati hardware
/// @{
///
/// Per abilitare la modalità a 64 bit con i due livelli utente/sistema dobbiamo
/// definire e inizializzare due strutture dati: la GDT e almeno un segmento
/// TSS. Entrambe risalgono al 286 e al suo sofisticato sistema di
/// segmentazione:
/// - La GDT (Global Descriptor Table) è una tabella di descrittori di segmento;
/// ogni segmento è definito da una base, un limite e alcuni attributi. I
/// segmenti identificano un intervallo di indirizzi [base, base+limite] e i
/// loro attributi ne descrivono il contenuto (codice, dati, ...), le
/// limitazioni sull'utilizzo (il livello di privilegio, la possibilità di
/// scrivervi o meno, etc.) o danno altre informazioni (in particolare la
/// dimensione degli operandi nel caso di segmenti di tipo codice).
/// - I segmenti TSS (Task State Segment) sono un particolare tipo di segmento
/// che nelle intenzioni dell'Intel doveva essere usato al posto dei descrittori
/// di processo per realizzare i cambi di contesto interamente in hardware.
///
/// I segmenti TSS ci interessano solo perché contengono il puntatore alla pila
/// sistema utilizzato dal meccanismo delle interruzioni, e per disabilitare
/// un certo meccanismo che interferisce con l'IOPL.
///
///  La GDT ci interessa per tre motivi:
///  - ci permette di avere codice con operandi a 64 bit (vedere il flag L più
///    avanti);
///  - ci permette di avere codice con livelli di privilegio diversi
///    per l'utente e il sistema (vedere campo DPL più avanti);
///  - deve contenere un descrittore del segmento TSS di cui sopra.

/// @brief Entrata della GDT
///
/// Come i @ref gate della IDT, anche i descrittori della GDT sono molto più
/// complicati di quanto avrebbero potuto essere, per via della loro storia:
/// - nel 286 la base era su 24 bit e il limite su 16 bit
///   (segmenti di max 64 KiB ciascuno in uno spazio di memoria da 16 MiB).
///   I descrittori occupavano 6 byte.
/// - nel 386 la base è passata a 32 bit e il limite a 20 bit, con un
///   flag di granularità (flag G) che permetteva di interpretare il
///   limite come numero di byte (G=0) o come numero di pagine da 4 KiB (G=1)
///   (segmenti di max 1 MiB o 4 GiB in uno spazio di memoria da 4 GiB).
///   I descrittori occupavano 8 byte.
/// - l'architettura a 64 bit dell'AMD ha reso base e limite non più
///   significativi per la maggior parte dei segmenti. Il descrittore
///   di segmento TSS è uno di quelli che fa eccezione, e in quel caso
///   il descrittore è stato esteso a 16 byte per poter avere una base
///   a 64 bit.
///
/// Il formato odierno dei descrittori è il seguente (la seconda riga
/// può essere assente se il tipo non la richiede):
///      
/// @code
///            ____________ base[24-31]     _
///           /                             |
///           |       _____ flags           | aggiunti nel 386
///           |      /                      |
///           |     |    __ limit[16-19]    -
///           |     |   /
///           |     |   |
///           |     |   |          descrittore originale del 286
///           |     |   | |-----------------------------------------------|
///        63 |     |   |  47   40 39   32 31           16 15            0
///       +---+---+-+-+-+-+---+---+---+---+---+---+---+---+---+---+---+---+
///       |   .   | . | . | access|     base[0-23]        |  limit[0-15]  |  +0
///       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+       _
///       | -   -   -   -   -   -   -   - |      base[32-63]              |  +8   | amd64
///       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+       -
///             +7      +6      +5      +4      +3      +2      +1
///     
/// @endcode
/// L'access byte ha il seguente formato (sostanzialmente mai cambiato):
///     
///         7   6   5   4   3   2   1   0           P: presenza
///       +---+---+---+---+---+---+---+---+         DPL: Desc. Priv. Level
///       | P |  DPL  | S |     Type      |         S: codice/dati (1) vs sistema (0)
///       +---+---+---+---+---+---+---+---+         Type: tipo
///     
/// E questi sono i flag:
///     
///         3   2   1   0                           G: granularità       (386)
///       +---+---+---+---+                         DB: 16/32 bit        (386)
///       | G |DB | L | - |                         L: 64 bit            (amd64)
///       +---+---+---+---+
///     
/// @note manipoliamo questa struttura in Assembler. Qui ci limitiamo a definire
/// una struttura che abbia la dimensione e l'allineamento corretti, in modo da
/// poterla allocare con new.
struct alignas(16) GDT_entry {
	char bytes[DIM_GDT_ENTRY];
};

/// Tabella GDT
struct GDT {
	GDT_entry entry[NUM_GDT_ENTRIES];
};

/// @brief Segmento TSS
///
/// Anche la strana struttura del TSS è dovuta alla sua storia. Il TSS del
/// 386 inizia così
///
///       +3  +2  +1  +0
///      +---+---+---+---+
///      |   |   |   |   |  0
///      +---+---+---+---+
///      | - | - |  SS0  | +4
///      +---+---+---+---+
///      |     ESP0      | +8
///      +---+---+---+---+
///            ...
///
/// Dove SS0:ESP0 è il puntatore alla pila di livello 0 (sistema) espressa come
/// selettore di segmento (16 bit) e offset (32 bit), in accordo con
/// l'architettura segmentata.  Nell'amd64 serve un puntatore alla pila non
/// segmentato e su 64 bit. L'AMD ha dunque riutilizzato i 4 byte all'offset +4
/// per estendere ESP0 in RSP0 (64 bit): il risultato è che RSP0 nel segmento TSS
/// della modalità a 64 bit è disallineato:
///
///        +7  +6  +5  +4  +3  +2  +1  +0
///     +---+---+---+---+---+---+---+---+
///     | RSP0 (low)    |   |   |   |   |  0
///     +---+---+---+---+---+---+---+---+
///          ...        |   RSP0 (high) | +8
///                     +---+---+---+---+
///  
/// Inoltre, essendo sparito SS0, il meccanismo delle interruzioni carica un valore
/// costante in SS (il selettore nullo).
///
struct alignas(16) TSS {
	char bytes[DIM_TSS];
};

/// @brief Inizializza il segmento TSS
/// @param tss	puntatore al segmento TSS
///
/// Gli ultimi due byte del TSS contengno l'offset (a partire dalla base del
/// TSS) di una "iobitmap". Questa struttura dati contiene un bit per ogni
/// indirizzo dello spazio di I/O (65536 bit, pari a 2 KiB).  Se l'iobitmap è
/// presente, il processore concede l'accesso alle porte di I/O se il
/// corrispondente bit è zero, anche se il privilegio corrente è (logicamente)
/// inferiore dell'IOPL.  Nell'idea originaria il sistema dovrebbe definire un
/// segmento TSS per ogni processo e in questo modo la iobitmap avrebbe permesso
/// di definire con precisione quali processi possono accedere a quali porte di
/// I/O.  Nel nostro caso vogliamo definire un unico TSS e usare l'IOPL per
/// vietare completamente l'accesso allo spazio di I/O ai processi utente, ma
/// per farlo dobbiamo "disattivare" il meccanismo dell'I/O bitmap. Questo si
/// ottiene definendo un offset che punti oltre il limite del TSS stesso. Dal
/// momento che il TSS è grande 104 byte, useremo un offset di 104.
extern "C" natq tss_init(TSS* tss);

/// @brief Initializza la GDT
/// @param gdt		puntatore alla GDT
/// @param tss		puntatore al TSS già inizializzato
///
/// Nell'archiettura 286/386, ad ogni istante sono attivi solo un numero
/// limitato di segmenti: quelli selezionati dai registri selettori di segmento
/// (CS, DS, SS, etc.).  Questi registri vanno caricati con dei "selettori di
/// segmento", che sono quantità di 16 bit con il seguente formato:
///
///       15                                              3       1   0
///     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
///     |                indice nella GDT                   |   |  RPL  |
///     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
///
///     (RPL: Requested Privilege Level)
///
/// Quando si carica un selettore in uno dei registri CS, DS, SS etc.  il
/// processore 286/386 esegue vari controlli sul tipo e gli attributi del segmento
/// indicizzato, il privilegio corrente, l'RPL e il DPL del segmento
/// indicizzato. Per esempio, CS è il Code Selector e può essere caricato
/// solo con segmenti codice, DS è il Data Selector e può essere caricato
/// solo con segmenti dati, SS è lo Stack Selector e può essere caricato
/// solo con segmenti dati scrivibili.
/// Il "privilegio corrente" non è altro che l'RPL del descrittore di
/// segmento codice che si trova attualmente in CS. Per quel che ci
/// riguarda, è sempre uguale al DPL del descrittore di segmento codice
/// indicizzato dal selettore.
///
/// I controlli sui privilegi e sugli attributi in generale sono stati quasi
/// completamente eliminati nell'amd64, ma non del tutto. CS deve contenere il
/// selettore di segmento codice e l'RPL di questo selettore (uguale, per quanto
/// detto prima, al DPL del segmento) continua a identificare il livello di
/// privilegio corrente. DS e SS non sono usati a livello sistema e possono
/// essere caricati con 0, ma la IRETQ controlla che SS venga caricato con
/// qualcosa di valido quando si sta tornando a livello utente. Per essere
/// valido, l'RPL caricato in SS deve essere uguale al livello di privilegio
/// corrente.  Per avere due livelli di privilegio ci servono dunque due
/// segmenti codice, uno con DPL=0 (sistema) e uno con DPL=3 (utente).  Inoltre,
/// ci serve un segmento dati con DPL=3 per poterlo caricare in SS quando il
/// processore è a livello utente.
///
///  Inizializzeremo dunque la GDT nel seguente modo:
/// (si vedano anche le #define in fondo a libce.h: gli ID_* sono gli indici
///  nella GDT e i SEL_* sono i corrispondenti selettori)
///
///  |indice |  offset |  selettore | contenuto                                |
///  |-------|---------|------------|------------------------------------------|
///  |    0  |     0   |   00       | descrittore nullo                        |
///  |    1  |     8   |   08       | descrittore segmento codice sistema      |
///  |    2  |    16   |   13       | descrittore segmento codice utente       |
///  |    3  |    24   |   1b       | descrittore segmento pila/dati utente    |
///  |    4  |    32   |   20       | descrittore segmento TSS                 |
///  |       |    40   |            | (parte alta del descrittore segmento TSS)|
///
/// dimensione: 48 byte
///
/// @note il descrittore di indice 0 deve essere nullo.
extern "C" void gdt_init(GDT* gdt, TSS* tss);

/// @brief Attiva la paginazione a 64 bit e salta all'entry point del primo modulo
/// @param info		puntatore alla struttura che descrive i moduli
/// @param entry	entry point a cui saltare
/// @param max_liv	livelli del TRIE (4 o 5)
///
/// Per attivare la modalità a 64 bit dobbiamo:
/// 1. attivare PAE settando il bit 4 di CR4; 
/// 2. opzionalmente, abilitare LA57 settando il bit 12 di CR4;
/// 3. settare il bit 15 del registro MSR EFER;
/// 4. settare il bit 31 di CR0.
/// 5. caricare CS con il selettore di un segmento che abbia L=1
///
/// Punto 1: PAE (Physical Address Extensions) è stato introdotto nel Pentium Pro per permettere
/// l'accesso a più di 4 GiB di memoria fisica. Con PAE la dimensione
/// dei descrittori di pagina e tabella passa da 4 a 8 byte.
///
/// Punto 2: con LA57 i bit significativi degli indirizzi virtuali passano da 48
/// a 57 e i TRIE passano da 4 a 5 livelli (non tutti i processori
/// supportano LA57).
///
/// Punto 3: dal Pentium in poi processori Intel/AMD implementano una serie di
/// MSR (Model Specific Register) a cui si accede con le istruzioni
/// rmdsr (lettura) e wrmsr (scrittura). Ogni MSR è identificato da un
/// codice numerico che va passato tramite ECX. Qui ci interessa il
/// registro EFER (Extended Feature Enable Register, codice 0xC0000080)
/// introdotto dall'AMD nel K7.  Il bit 15 di questo registro abilita
/// LME (Long Mode Extension), ovvero la modalità a 64 bit. Attenzione
/// però: la modalità a 64 bit è una sotto-modalità della paginazione, e
/// quindi verrà effettivamente attivata solo quando avremo attivato
/// anche la paginazione
///
/// Punto 4: per attivare la paginazione settiamo il bit 31 di CR0.
/// Visto che abbiamo abilitato anche PAE e settato il bit 15 di EFER,
/// il processore passerà contestualmente nella modalità a 64 bit.
///
/// @note Settiamo anche il bit 16 (write-protect) di CR0, in modo che il bit R/W
/// delle tabelle delle pagine abbia effetto anche quando il processore
/// si trova a livello sistema. Può essere utile per intercettare
/// errori nel modulo sistema, in particolare quando si svolgono
/// gli esercizi.
///
/// Punto 5: per caricare CS dobbiamo eseguire una "far jump", dopo
/// aver caricato la nostra GDT (preparata da gdt_init()).
extern "C" [[noreturn]] void attiva_paginazione(boot64_info *info, natl entry, int max_liv);
/// @}

/// @brief Controlla che un modulo non si sovrapponga a quelli già esistenti
/// @param info 	puntatore alla struttura che descrive i moduli
/// @param beg		base del modulo da controllare
/// @param end		limite del modulo da controllare
bool controlla_collisione(boot64_info *info, natq beg, natq end)
{
	for (natq i = 0; i < info->nummod; i++) {
		boot64_modinfo *m = &info->mod[i];
		if (end <= m->mod_start || beg > m->mod_end)
			continue;
		flog(LOG_ERR, "Trovata sovrapposizione con mod[%lld]", i);
		return false;
	}
	return true;
}

/// @brief Estrae le informazioni sui moduli.
/// @param mod		puntatore alla struttura passata dal primo boot loader
/// @param info		puntatore alla struttura da riempire
/// @param check_align	opzionalmente, controlla che l'allinamento richiesto non sia
/// 			superiore alla pagina
///
/// Estrae le informazioni rilevanti dal modulo ELF e le copia nella struttura
/// boot64_info. In questo modo il modulo successivo (il modulo sistema) non ha
/// bisogno di dover interpretare il formato ELF.
bool decodifica_modulo(multiboot_module_t* mod, boot64_modinfo *info, bool check_align) {
	Elf64_Ehdr* elf_h = ptr_cast<Elf64_Ehdr>(mod->mod_start);

	// verifichiamo che si tratti di un file ELF
	if (!(elf_h->e_ident[EI_MAG0] == ELFMAG0 &&
	      elf_h->e_ident[EI_MAG1] == ELFMAG1 &&
	      elf_h->e_ident[EI_MAG2] == ELFMAG2 &&
	      elf_h->e_ident[EI_MAG2] == ELFMAG2))
	{
		flog(LOG_ERR, "Formato del modulo '%s' non riconosciuto",
			ptr_cast<char>(mod->cmdline));
		return false;
	}

	// e che contenga un eseguibile per x86_64/amd64
	if (!(elf_h->e_ident[EI_CLASS] == ELFCLASS64  &&
	      elf_h->e_ident[EI_DATA]  == ELFDATA2LSB &&
	      elf_h->e_type	       == ET_EXEC     &&
	      elf_h->e_machine 	       == EM_AMD64))
	{
		flog(LOG_ERR, "Il modulo '%s' non contiene un esegubile per x86_64",
				ptr_cast<char>(mod->cmdline));
		return false;
	}

	// interpretiamo la sua tabella di programma
	info->numseg = 0;
	for (int i = 0; i < elf_h->e_phnum; i++) {
		Elf64_Phdr* elf_ph = ptr_cast<Elf64_Phdr>(mod->mod_start + elf_h->e_phoff +
				i * elf_h->e_phentsize);

		// ci interessano solo i segmenti PT_LOAD
		if (elf_ph->p_type != PT_LOAD)
			continue;

		if (check_align && elf_ph->p_align > DIM_PAGINA) {
			flog(LOG_ERR, "Il modulo '%s' contiene un segmento con allineamento superiore alla pagina",
					ptr_cast<char>(mod->cmdline));
			return false;
		}

		if (info->numseg > MAX_SEGMENTS) {
			flog(LOG_ERR, "Il modulo '%s' contiene più di %d segmenti caricabili",
					ptr_cast<char>(mod->cmdline), MAX_SEGMENTS);
			return false;
		}

		boot64_segment *s = &info->segments[info->numseg];

		s->offset = elf_ph->p_offset;
		s->vaddr  = elf_ph->p_vaddr;
		s->size   = elf_ph->p_filesz;
		s->memsize= elf_ph->p_memsz;
		s->flags  = (elf_ph->p_flags & PF_W) ? BIT_RW : 0;
		flog(LOG_INFO, "  . seg[%d]: off %6llx vaddr %16llx size %6llx memsize %6llx",
				i, s->offset, s->vaddr, s->size, s->memsize);

		info->numseg++;
	}

	if (!find_eh_frame(mod->mod_start, info->eh_frame_base, info->eh_frame_size)) {
		flog(LOG_WARN, "Il modulo '%s' non contiene l'exception header",
					ptr_cast<char>(mod->cmdline));
	}

	info->mod_start = mod->mod_start;
	info->mod_end   = mod->mod_end;
	info->entry_point = elf_h->e_entry;

	return true;
}

/// @brief Carica il primo modulo secondo le indicazioni della sua tabella di programma.
///
/// @param info		puntatore alla struttura che descrive i moduli
/// @param mem_tot	memoria disponibile
bool carica_modulo(boot64_info *info, paddr mem_tot) {
	boot64_modinfo *m = &info->mod[0];
	for (unsigned int i = 0; i < m->numseg; i++) {
		boot64_segment *s = &m->segments[i];

		// evitiamo di sovrascrivere quanto si trova già in memoria
		if (s->vaddr < 2*MiB) {
			flog(LOG_ERR, "Il modulo %d richiede un caricamento all'indirizzo '%llx',"
					"con possibile sovrascrittura di altri moduli",
					i, s->vaddr);
			return false;
		}

		if (!controlla_collisione(info, s->vaddr, s->vaddr + s->memsize))
			return false;

		if (s->vaddr + s->memsize < s->vaddr || s->vaddr + s->memsize >= mem_tot) {
			flog(LOG_ERR, "Memoria insufficiente");
			return false;
		}

		memcpy(voidptr_cast(s->vaddr), voidptr_cast(m->mod_start + s->offset), s->size);
		flog(LOG_INFO, "- copiati %6llx byte da %8llx a %8llx",
			s->size, m->mod_start +  s->offset, s->vaddr);
		if (s->memsize != s->size) {
			memset(voidptr_cast(s->vaddr + s->size), 0, s->memsize - s->size);
			flog(LOG_INFO, "- azzerati ulteriori %llx byte",
					s->memsize - s->size);
		}
	}
	flog(LOG_INFO, "- entry point %llx", m->entry_point);
	return true;
}

/// Massimo numero di argomenti per il boot loader
static const int MAXARGS = 10;
/// Flag settato se è richiesto il collegamento al debugger
static int debug_mode = 0;

/// @brief Interpreta gli argomenti passati al boot loader.
/// @param cmd	stringa degli argomenti
///
/// Scompone la stringa cmd in argomenti separati da spazi.
/// Al momento comprendiamo un solo argomento: `-s`, che attiva
/// la modalità debug.
static void parse_args(char *cmd)
{
	char *argv[MAXARGS];
	int argc = 0;

	char *scan = cmd;
	do {
		argv[argc] = scan;
		while (*scan && *scan != ' ')
			scan++;
		if (*scan) {
			*scan = '\0';
			for (scan++; *scan && *scan == ' '; scan++)
				;
		}
		argc++;
	} while (*scan && argc <= MAXARGS);

	for (int i = 0; i < argc; i++) {
		if (argv[i][0] != '-')
			continue;
		if (argv[i][1] == 's' && !debug_mode) {
			flog(LOG_INFO, "DEBUG MODE");
			debug_mode = 1;
		}
	}
}

/// @brief Mappa la memoria fisica e l'area PCI in memoria virtuale
///
/// @param root_tab	indirizzo fisico della tabella radice
/// @param mem_tot	dimensione della memoria fisica
bool crea_finestra_FM(paddr root_tab, paddr mem_tot)
{
	auto identity_map = [] (vaddr v) -> paddr { return v; };
	/// mappiamo tutta la memoria fisica:
	/// - a livello sistema (bit U/S non settato)
	/// - scrivibile (bit R/W settato)
	/// - con pagine di grandi dimensioni (bit PS)
	///   (usiamo pagine di livello 2 che sono sicuramente disponibili)

	/// @note vogliamo saltare la prima pagina per intercettare *nullptr, e inoltre
	/// vogliamo settare il bit PWT per le pagine che contengono la memoria
	/// video.  Per farlo dobbiamo rinunciare a settare PS per la prima regione
	natq first_reg = dim_region(1);

	// [0, DIM_PAGINA): non mappato
	// [DIM_PAGINA, 0xa0000): memoria normale
	if (map(root_tab, DIM_PAGINA, 0xa0000, BIT_RW, identity_map) != 0xa0000)
		return false;
	// [0xa0000, 0xc0000): memoria video
	if (map(root_tab, 0xa0000, 0xc0000, BIT_RW|BIT_PWT, identity_map) != 0xc0000)
		return false;
	// [0xc0000, first_reg): memoria normale
	if (map(root_tab, 0xc0000, first_reg, BIT_RW, identity_map) != first_reg)
		return false;

	// mappiamo il resto della memoria con PS settato
	if (mem_tot > first_reg) {
		if (map(root_tab, first_reg, mem_tot, BIT_RW, identity_map, 2) != mem_tot)
			return false;
	}

	flog(LOG_INFO, "Creata finestra sulla memoria centrale:  [%16llx, %16llx)", DIM_PAGINA, mem_tot);

	/// Mappiamo tutti gli altri indirizzi, fino a 4GiB, settando sia PWT che PCD.
	/// Questa zona di indirizzi è utilizzata in particolare dall'APIC per mappare i propri registri.
	vaddr	beg_pci = allinea(mem_tot, 2*MiB),
		end_pci = 4*GiB;
	if (map(root_tab, beg_pci, end_pci, BIT_RW|BIT_PCD|BIT_PWT, identity_map, 2) != end_pci)
		return false;

	flog(LOG_INFO, "Creata finestra per memory-mapped-IO:    [%16llx, %16llx)", beg_pci, end_pci);
	return true;
}

/// @brief sincronizzazione con gdb
///
/// Se è richiesta la connessione con il debugger, settiamo a 1 questo flag.
/// Il codice in attiva_paginazione(), prima di saltare all'entry point del
/// primo modulo,  entra in un ciclo e ci resta fino a quando il flag vale 1.
/// Lo script debug resetta questo flag alla partenza di gdb.
volatile int wait_for_gdb;

/// @brief Entry point C++ del boot loader
/// @param magic	codice che identifica lo standard multiboot
/// @param mbi		puntatore alle informazioni passate dal primo bootstrap loader
///
/// @note L'entry point vero e proprio è _start in boot.s.
///
/// Il primo bootstrap loader (in QEMU) attiva il modo protetto (per poter
/// accedere agli indirizzi di memoria principale superiori a 1MiB) e carica il
/// secondo boostrap loader (questo programma) in memoria, quindi salta alla
/// sua prima istruzione. Il primo bootstrap loader può anche passare delle
/// informazioni al secondo (tramite i registri e la memoria).
///
/// Il primo loader, prima di saltare alla prima istruzione del secondo (l'entry
/// point specificato nel file eseguibile), lascia nel registro %eax un valore
/// di riconoscimento e in %ebx l'indirizzo di una struttura dati, contentente
/// varie informazioni (in particolare, la quantità di memoria principale
/// installata nel sistema, il dispositivo da cui è stato eseguito il bootstrap
/// e l'indirizzo di memoria in cui sono stati caricati gli eventuali moduli)
extern "C" void main (natl magic, multiboot_info_t* mbi)
{
	boot64_info tmp_info;
	char *cmdline = nullptr;
	paddr mem_lower = 640*KiB;
	paddr mem_tot = 8*MiB;

	// inizializzazione della porta seriale per il debugging
	serial::init1();
	flog(LOG_INFO, "Boot loader di Calcolatori Elettronici, v1.0");

	// controlliamo di essere stati caricati da un bootloader che rispetti
	// lo standard multiboot
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		flog(LOG_ERR, "Numero magico non valido: %#x", magic);
		return;
	}

	// se il primo bootloader ci ha passato informazioni sulla memoria, la usiamo
	// per costruire correttamente la finestra FM (altrimenti usiamo valori di default)
	if (mbi->flags & MULTIBOOT_INFO_MEMORY) {
		mem_lower = (mbi->mem_lower + 1) * KiB;
		mem_tot = (mbi->mem_upper + 0x480) * KiB;
	}
	flog(LOG_INFO, "Memoria totale: %lld MiB, heap: %lld KiB",
			mem_tot/MiB, (mem_lower - DIM_PAGINA)/KiB);

	// se il primo bootloader ci ha passato una stringa di opzioni, proviamo a esaminarla
	if (mbi->flags & MULTIBOOT_INFO_CMDLINE) {
		cmdline = ptr_cast<char>(mbi->cmdline);
		flog(LOG_INFO, "Argomenti: %s", cmdline);
		parse_args(cmdline);
	}

	// Il numero dei moduli viene passato dal primo bootloader in mods_count
	// Abbiamo bisogno di almeno un modulo.
	if (!(mbi->flags & MULTIBOOT_INFO_MODS) || mbi->mods_count < 1)
	{
		flog(LOG_ERR, "Informazioni sui moduli assenti o errate");
		return;
	}

	// raccogliamo le informazioni che ci servono su tutti i moduli
	memset(&tmp_info, 0, sizeof(tmp_info));
	flog(LOG_INFO, "Il boot loader precedente ha caricato %d modul%s:", mbi->mods_count,
			mbi->mods_count > 1 ? "i" : "o");
	multiboot_module_t* mod = ptr_cast<multiboot_module_t>(mbi->mods_addr);
	if (mbi->mods_count > MAX_MODULES) {
		flog(LOG_ERR, "Troppi moduli (massimo %d)", MAX_MODULES);
		return;
	}
	bool check_align = false;
	for (unsigned int i = 0; i < mbi->mods_count; i++) {
		flog(LOG_INFO, "- mod[%d]: start=%x end=%x file=%s",
			i, mod[i].mod_start, mod[i].mod_end, ptr_cast<const char>(mod[i].cmdline));
		if (!decodifica_modulo(&mod[i], &tmp_info.mod[i], check_align))
			return;
		check_align = true;
	}
	tmp_info.nummod = mbi->mods_count;
	// e carichiamo il primo
	flog(LOG_INFO, "Copio mod[0] agli indirizzi specificati nel file ELF:");
	if (!carica_modulo(&tmp_info, mem_tot)) {
		flog(LOG_ERR, "ATTENZIONE: impossibile caricare mod[0]");
		return;
	}

	// ora mbi non ci serve più e possiamo riusare la memoria nel primo MiB come heap
	heap_init(voidptr_cast(DIM_PAGINA), mem_lower - DIM_PAGINA);
	// nello heap creiamo tre strutture che devono essere preservate anche dopo che
	// avremo ceduto il controllo al primo modulo:
	// 1) La struttura boot64_info;
	boot64_info *info = new boot64_info{tmp_info};
	if (!info) {
		flog(LOG_ERR, "ATTENZIONE: impossibile allocare boot64_info");
		return;
	}
	// 2) Il segmento TSS
	TSS *tss = new TSS;
	if (!tss) {
		flog(LOG_ERR, "ATTENZIONE: impossibile allocare il segmento TSS");
		return;
	}
	// che inizializziamo, passando al primo modulo l'informazione su dove
	// si trova tss_punt_nucleo al suo interno;
	info->tss_punt_nucleo = tss_init(tss);
	// 3) la GDT
	GDT *gdt = new GDT;
	if (!gdt) {
		flog(LOG_ERR, "ATTENZIONE: impossible allocare la tabella GDT");
		return;
	}
	// che inizializziamo.
	gdt_init(gdt, tss);
	// i dettagli su GDT e TSS si trovano in boot64/boot.s

	// Ora creiamo la finestra FM. Le tabelle verranno allocate nello heap.
	paddr root_tab = alloca_tab();
	if (!root_tab) {
		flog(LOG_ERR, "ATTENZIONE: impossibile allocare la tabella radice");
		return;
	}
	if (!crea_finestra_FM(root_tab, mem_tot)) {
		flog(LOG_ERR, "ATTENZIONE: fallimento in crea_finestra_FM()");
		return;
	}

	// prima di attivare la paginazione dobbiamo caricare cr3
	loadCR3(root_tab);

	// Inizializziamo l'APIC (il vecchio PIC è stato disabilitato da _start
	// prima di chiamare main)
	flog(LOG_INFO, "Inizializzo l'APIC");
	if (!apic::init())
		return;

	// Inizializziamo la console (video + tastiera). In particolare disabilitiamo
	// la tastiera a generare richieste di interruzione
	flog(LOG_INFO, "Inizializzo video e tastiera");
	vid::clear();
	kbd::disable_intr();

	if (debug_mode) {
		wait_for_gdb = 1;
		flog(LOG_INFO, "Attendo collegamento da gdb...");
	}
	// Da questo punto in poi non ci sono più allocazioni nello heap. Passiamo
	// l'attuale contenuto di memlibera (testa della lista dei chunk liberi
	// nello heap) al primo modulo, in modo che possa usare il resto della
	// memoria disponibile nel primo MiB.
	info->memlibera = heap_getinitmem();
	flog(LOG_INFO, "Attivo la modalita' a 64 bit e cedo il controllo a mod[0]...");
	attiva_paginazione(info, info->mod[0].entry_point, MAX_LIV);

	/* mai raggiunto */
	return;
}
/// @}
