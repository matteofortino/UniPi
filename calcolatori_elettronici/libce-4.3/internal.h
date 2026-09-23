/// @file internal.h
/// @brief funzioni usate internamente
#include "include/libce.h"
#include "include/vm.h"

/// @addtogroup internal Funzioni usate internamente
/// @{
/// Massimo unsigned int
static const uintmax_t UINT_MAX = 4294967295U;
/// inizializza la idt per i programmi "bare"
extern "C" void init_idt();
/// @brief cerca un carattere in una stringa
/// @param src	stringa in cui cercare
/// @param c	carattere da cercare
/// @return	puntatore alla prima occorrenza del carattere nella stringa,
/// 		o nullptr se non trovato
char *index(const char *src, int c);
/// @brief calcola la lunghezza di una stringa, con limite massimo
/// @param s	puntatore alla stringa
/// @param len	lunghezza massima
/// @return	lunghezza della stringa, se inferiore a _len_, altrimenti _len_
size_t strnlen(const char *s, size_t len);
/// disabilita le interruzioni esterne mascherabili
extern "C" void cli();
/// abilita le interruzioni esterne mascherabili
extern "C" void sti();
/// @brief mostra un messaggio di errore e spegne la macchina
/// @param msg	messaggio da stampare
extern "C" [[noreturn]] void panic(const char *msg);
/// invoca i costruttori definiti nella sezione init_array
extern "C" void ctors();
/// invoca i distruttori definiti nella sezione fini_array
extern "C" void dtors();
/// @}

/// @addtogroup intr
/// @{

/// @brief gate della IDT
///
/// I gate della IDT hanno una struttura complessa, dovuta alla loro storia:
/// i gate sono stati introdotti nell'architettura a 16 bit del 286 con la
/// seguente forma, su 6 byte (3 word):
///    
///              +5  +4  +3  +2  +1  +0
///     +---+---+---+---+---+---+---+---+
///     | xxxxx | A | 0 |  sel  |  off  |
///     +---+---+---+---+---+---+---+---+
///    
/// Dove A è il byte di accesso e sel:off è l'indirizzo della routine
/// nello spazio segmentato (selettore di segmento + offset all'interno
/// del segmento).
///
/// Il byte di accesso A ha la seguente forma:
///    
///       7 6 5 4 3 2 1 0
///      +-+-+-+-+-+-+-+-+
///      |P|DPL|0| tipo  |
///      +-+-+-+-+-+-+-+-+
///    
/// i tipi che ci interessano sono: 1110 (interrupt) oppure 1111
/// (trap), ma il 286 ne definiva anche altri.
///
/// Nel 386 l'offset è stato esteso a 32bit e il descrittore è stato
/// portato a 8 byte. I 16 bit in più dell'offset sono stati aggiunti
/// in cima e il resto è rimasto uguale:
///    
///      +7  +6  +5  +4  +3  +2  +1  +0
///     +---+---+---+---+---+---+---+---+
///     | o31:16| A | 0 |  sel  | o15:0 |
///     +---+---+---+---+---+---+---+---+
///    
/// Per l'architettura a 64 bit, AMD ha portato l'offset a 64 bit e il
/// descrittore a 16 byte. L'architettura non è più segmentata, ma 'sel' rimane
/// per specificare il livello di privilegio e la modalità a 64 bit, mentre
/// 'offset' è l'indirizzo virtuale della routine. Il byte di accesso continua
/// ad avere la stessa forma, ma i tipi ammessi sono solo interrupt (1110) o
/// trap (1111).
///    
///      +7  +6  +5  +4  +3  +2  +1  +0
///     +---+---+---+---+---+---+---+---+
///     | o31:16| A | 0 |  sel  | o15:0 |
///     +---+---+---+---+---+---+---+---+
///     |       0       |     o63:32    |  +8
///     +---+---+---+---+---+---+---+---+
///    
/// @note Manipoliamo questi campi direttamente in assembly. In C++ definiamo una
/// struttura `gate` che ha il solo scopo di occupare lo spazio necessario:
struct alignas(16) gate {
	natq qw1;
	natq qw2;
};

/// la IDT è un array di gate
extern gate idt[];

/// @brief compone un gate della IDT
/// @param routine	indirizzo della routine da associare al gate
/// @param trap		true se il gate deve essere di tipo trap
/// @param liv		dpl del gate (LIV_UTENTE o LIV_SISTEMA)
/// @return		il gate con il formato corretto
///
/// La funzione è scritta in assembly.  Imposta il gate il modo che la routine
/// giri a livello sistema in modalità 64bit.
extern "C" gate componi_gate(void routine(), bool trap, int liv);

/// @addtogroup apic
/// @{
namespace apic {

	/// numero di piedini per le richieste di interruzione
	const natl IRQ_MAX = 24;

	/// @name registri mappati in memoria
	/// 
	/// l'APIC ha alcuni registri mappati nello spazio di memoria e diversi
	/// registri interni. I registri IRR, ISR, EOIR, IOREGSEL e IOWIN sono mappati
	/// nello spazio di memoria e vi si accede con normali istruzioni con
	/// operandi in memoria (per es. mov). Definiamo un puntatore che contiene
	/// l'indirizzo del registro e poi lo dereferenziamo.
	/// @{

	/// puntatore a End Of Interrupt Register
	extern volatile natl* pEOIR;
	/// puntatore a I/O REGister Selector (indirizzo non noto staticamente)
	extern volatile natl* pIOREGSEL;
	/// puntatore a I/O WINdow (indirizzo non noto staticamente)
	extern volatile natl* pIOWIN;
	/// @}

	/// @name registri IRR e ISR
	///
	/// Ciascun registro occupa in realtà 8 sotto-registri da 4 byte, con indice i da 0 a 7.
	/// @{
	
	/// @brief legge una sottoparte di IRR
	/// @param i	indice della sottoparte da leggere
	/// @return	sottoparte letta
	natl read_irr(natb i);

	/// @brief legge una sottoparte di ISR
	/// @param i	indice della sottoparte da leggere
	/// @return	sottoparte letta
	natl read_isr(natb i);
	/// @}

	/// @name registri interni
	///
	/// I registri interni sono tutti di 4 byte. Vi si può accedere solo
	/// indirettamente, tramite IOREGSEL e IOWIN: prima si scrive l'indice
	/// del registro interno desiderato in IOREGSEL e a quel punto IOWIN
	/// funge da finestra sul registro interno: leggendo e scrivendo IOWIN
	/// si legge e scrive il registro interno selezionato.  Le funzioni in()
	/// e out() semplificano questo procedimento.
	/// @{
	
	/// @brief legge un registro interno dell'APIC
	/// @param off 	indice del registro interno
	/// @return	contenuto del registro
	natl in(natb off);

	/// @brief scrive in un registro interno dell'APIC
	/// @param off	indice del registro interno
	/// @param v	valore da scrivere
	void out(natb off, natl v);

	/// @name registri associati ai piedini di interruzione
	/// 
	/// i registri interni più importanti per il programmatore sono
	/// quelli che compongono la "redirection table", che inizia all'indice 0x10.
	/// La tabella ha 24 righe (una per ogni piedino di richiesta di
	/// interruzione) e ogni riga occupa due registri interni (detti h e l)
	///    
	///         h        l
	///     +--------+--------+
	///     |        |        |  RTO           (IRQ 0)
	///     +--------+--------+
	///     |        |        |  RTO + 2       (IRQ 1)
	///     +--------+--------+
	///     ~                 ~
	///     +-----------------+
	///     |        |        |  RTO + 2*23    (IRQ 23)
	///     +--------+--------+
	///    
	/// La parte 'l' di ogni riga contiene le informazioni più importanti:
	/// il vettore associato all'IRQ (bit 0-7), il bit per mascherare
	/// le richieste (bit 16) e il bit che seleziona il Trigger Mode
	/// sul livello oppure sul fronte (bit 15).
	/// @{

	/// Primo indice della redirection table
	const natl RTO = 0x10;
	/// bit del trigger mode (1=livello, 0=fronte)
	const natl TRGM_BIT = (1U << 15); 
	/// bit della maschera (1=mascherato)
	const natl MIRQ_BIT = (1U << 16);

	/// @brief leggi la parte alta di una riga della redirection table
	/// @param irq	piedino a cui è associata la riga
	/// @return	contenuto della parte alta della riga
	natl read_rth(natb irq);
	/// @brief leggi la parte bassa di una riga della redirection table
	/// @param irq	piedino a cui è associata la riga
	/// @return	contenuto della parte alta della riga
	natl read_rtl(natb irq);
	/// @brief scrivi la parte alta di una riga della redirection table
	/// @param irq	piedino a cui è associata la riga
	/// @param w	valore da scrivere
	void write_rth(natb irq, natl w);
	/// @brief scrivi la parte bassa di una riga della redirection table
	/// @param irq	piedino a cui è associata la riga
	/// @param w	valore da scrivere
	void write_rtl(natb irq, natl w);
	/// @}
	/// @}

}
/// @}
/// @}

/// @addtogorup libexc
/// @{

/// Associa una stringa descrittiva ad ogni tipo di eccezione.
extern const char* eccezioni[];
/// @}

/// @addtogroup IO
/// @{

namespace kbd {
/// @addtogroup kbd
/// @{

	/// Massimo numero di codici di scansione riconosciuti
	const natl MAX_CODE = 42;
	/// @name Registri del controllore della tastiera
	/// @{
	const ioaddr iRBR = 0x60;	///< Receive Buffer Register
	const ioaddr iTBR = 0x60;	///< Transmit Buffer Register
	const ioaddr iCMR = 0x64;	///< Command Register
	const ioaddr iSTR = 0x64;	///< Status Register
	/// @}

	/// flag che ricorda se il tasto shift risulta ancora premuto
	extern bool shift;
	/// tabella dei codici di scansione riconosciuti
	extern natb tab[MAX_CODE];
	/// tabella dei corrispondenti codici ASCII quando shift non è premuto
	extern char tabmin[MAX_CODE];
	/// tabella dei corrispondenti codici ASCII quando shift è premuto
	extern char tabmai[MAX_CODE];

	/// reset dell'interfaccia
	void reset();

	/// @brief riavvia la macchina
	///
	/// un piedino del microcontrollore della tastiera comanda la circuiteria
	/// di reset, quindi la funzione reboot() è definita qui.
	/// 
	/// @note lo script boot imposta la macchina QEMU per eseguire lo shutdown
	/// invece del riavvio.
	void reboot();
/// @}
}

namespace vid {
/// @addtogroup vid
/// @{

	/// numero di colonne in modalità testo
	const natl COLS = 80;
	/// numero di righe in modalità testo
	const natl ROWS = 25;
	/// numero di caratteri visibili in modalità testo
	const natl VIDEO_SIZE = COLS * ROWS;
	/// Indirizzo interno per la posizione del cursore hardware (parte alta)
	const natb CUR_HIGH = 0x0e;
	/// Indirizzo interno per la posizione del cursore hardware (parte bassa)
	const natb CUR_LOW = 0x0f;

	/// @name registri del controllore della scheda video
	/// @{
	const ioaddr iIND = 0x03D4;	///< selezione del registro interno
	const ioaddr iDAT = 0x03D5;	///< finestra sul registro selezionato
	/// @}

	/// Puntatore alla memoria video (modalità testo)
	extern volatile natw* video;
	/// coordinata x del cursore
	extern natb x;
	/// coordinata y del cursore
	extern natb y;
	/// attributo colore
	extern natw attr;

	/// aggiorna la posizione del cursore hardware
	void cursor();
	/// esegue lo scorrimento di una riga verso l'alto
	void scroll();
/// @}
}

namespace svga {
/// @addtogroup svga
/// @{

	extern volatile void* framebuffer;
	extern volatile natw* vgareg;
	extern volatile natw* vbeext;

	//indici per vbeext
	static const natw VBE_DISPI_INDEX_ID = 0x0;
	static const natw VBE_DISPI_INDEX_XRES = 0x1;
	static const natw VBE_DISPI_INDEX_YRES = 0x2;
	static const natw VBE_DISPI_INDEX_BPP = 0x3;
	static const natw VBE_DISPI_INDEX_ENABLE = 0x4;
	static const natw VBE_DISPI_INDEX_BANK = 0x5;
	static const natw VBE_DISPI_INDEX_VIRT_WIDTH = 0x6;
	static const natw VBE_DISPI_INDEX_VIRT_HEIGHT = 0x7;
	static const natw VBE_DISPI_INDEX_X_OFFSET = 0x8;
	static const natw VBE_DISPI_INDEX_Y_OFFSET = 0x9;

	//valori possibili per VBE_DISPI_INDEX_BPP
	static const natw VBE_DISPI_BPP_4 = 0x04;
	static const natw VBE_DISPI_BPP_8 = 0x08;
	static const natw VBE_DISPI_BPP_15 = 0x0F;
	static const natw VBE_DISPI_BPP_16 = 0x10;
	static const natw VBE_DISPI_BPP_24 = 0x18;
	static const natw VBE_DISPI_BPP_32 = 0x20;

	//bit settabili su VBE_DISPI_INDEX_ENABLE
	static const natw VBE_DISPI_DISABLED = 0x00;
	static const natw VBE_DISPI_ENABLED = 0x01;
	static const natw VBE_DISPI_GETCAPS = 0x02;
	static const natw VBE_DISPI_8BIT_DAC = 0x20;
	static const natw VBE_DISPI_LFB_ENABLED = 0x40;
	static const natw VBE_DISPI_NOCLEARMEM = 0x80;

	bool init();

/// @}
}

namespace hd {
/// @addtogroup hd
/// @{

	/// @name registri del controllore ATA (primario)
	/// @{
	const ioaddr iBR  = 0x01F0;	///< Buffer Register
	const ioaddr iERR = 0x01F1;	///< Error Register
	const ioaddr iSCR = 0x01F2;	///< Sector Count Register
	const ioaddr iSNR = 0x01F3;	///< Sector Number Register
	const ioaddr iCNL = 0x01F4;	///< Cylinder Number Low
	const ioaddr iCNH = 0x01F5;	///< Cylinder Number High
	const ioaddr iHND = 0x01F6;	///< Head Number and Device
	const ioaddr iCMD = 0x01F7;	///< Command Register
	const ioaddr iSTS = 0x01F7;	///< Status Register
	const ioaddr iDCR = 0x03F6;	///< Device Control Register
	const ioaddr iASR = 0x03F6;	///< Alternate Status Register
	/// @}

	/**
	 * @brief Scompone un Logical Block Address e lo scrive in SNR, CNL, CNH e HND
	 *
	 * @param lba	logical block address da scrivere
	 */
	void set_lba(natl lba);

	/**
	 * @brief Attende che il registro BR sia disponibile
	 */
	void wait_for_br();

/// @}
}

namespace bm {
/// @addtogroup bm
/// @{

	/// @name registri del controllore Bus Master ATA
	/// @{
	extern ioaddr iBMCMD;	///< Bus Master Command
	extern ioaddr iBMSTR;	///< Bus Master Status Register
	extern ioaddr iBMDTPR;	///< Bus Master Descriptor Table Pointer
	/// @}

/// @}
}

namespace pci {
/// @addtogroup pci
/// @{

	/// @name registri per l'accesso allo spazio di configurazione PCI
	/// @{
	const ioaddr iCAP = 0x0CF8; ///< Configuration Address Port
	const ioaddr iCDP = 0x0CFC; ///< Configuration Data Port
	/// @}

	/**
	 * @brief Codifica un valore da scrivere in CAP
	 *
	 * @param bus	numero di bus
	 * @param dev	numero di dispositivo
	 * @param fun	numero di funzione
	 * @param off	offset del registro a cui accedere
	 *
	 * @return 	valore da scrivere in CAP
	 */
	natl make_CAP(natb bus, natb dev, natb fun, natb off);

/// @}
}

namespace timer {
/// @addtogroup libtimer
/// @{

	/// @name registri del controllore del timer
	/// @{
	static const ioaddr iCWR = 0x43;	///< Configuration Word Register

	static const ioaddr iCTR0_LOW = 0x40;	///< Counter 0, Low
	static const ioaddr iCTR0_HIG = 0x40;	///< Counter 0, High
	static const ioaddr iSTS0_LOW = 0x40;	///< Status 0, Low
	static const ioaddr iSTS0_HIG = 0x40;	///< Status 0, High

	static const ioaddr iCTR1_LOW = 0x41;   ///< Counter 1, Low
	static const ioaddr iCTR1_HIG = 0x41;   ///< Counter 1, High
	static const ioaddr iSTS1_LOW = 0x41;   ///< Status 1, Low
	static const ioaddr iSTS1_HIG = 0x41;   ///< STatus 1, High

	static const ioaddr iCTR2_LOW = 0x42;   ///< Counter 2, Low
	static const ioaddr iCTR2_HIG = 0x42;   ///< Counter 2, High
	static const ioaddr iSTS2_LOW = 0x42;   ///< Status 2, Low
	static const ioaddr iSTS2_HIG = 0x42;   ///< Status 2, High
	/// @}

	/// Speaker Port
	static const ioaddr iSPR = 0x61;
/// @}
}

/// @addtogroup piix3 South Bridge (PIIX3)
///
/// Ci interessa accedere al South Bridge solo perché è tramite di esso
/// che si accede all'APIC nella macchina QEMU. In particolare, è il
/// PIIX3 che decodifica gli indirizzi dei registri dell'APIC, ma questa
/// funzione va prima abilitata. Inoltre, il PIIX3 permette di configurare
/// (parzialmente) questi indirizzi.
/// @{

/// namespace per le risorse del South Bridge
namespace piix3 {

	/// VendorID del PIIX3
	const natw VENDOR_ID = 0x8086;
	/// Device ID del PIIX3
	const natw DEVICE_ID = 0x7000;

	/// @brief offset del registro XBCS
	///
	/// Il registro XBCS (X-Bus Chip Select) contiene alcuni bit che
	/// abilitano/disabiliano l'accesso ad alcune periferiche, come
	/// il controllore della tastiera (che è emulato nel PIIX3) e l'APIC.
	/// Il registro si trova nello spazio di configurazione PCI del PIIX3,
	/// all'offset 0x4e.
	const natb XBCS = 0x4e;
	/// Bit di XBCS che controlla l'accesso all'APIC
	const natl XBCS_ENABLE = (1U << 8);

	/**
	 * @brief Abilita l'accesso all'APIC tramite il PIIX3
	 *
	 * @param bus	numero del bus del PIIX3
	 * @param dev	numero di device del PIIX3
	 * @param fun	numero di funzione del PIIX3
	 *
	 * @return true in caso di successo e false in caso di errore
	 */
	bool ioapic_enable(natb bus, natb dev, natb fun);

	/**
	 * @brief Disabilita l'accesso all'APIC tramite il PIIX3
	 *
	 * @param bus	numero del bus del PIIX3
	 * @param dev	numero di device del PIIX3
	 * @param fun	numero di funzione del PIIX3
	 *
	 * @return true in caso di successo e false in caso di errore
	 */
	bool ioapic_disable(natb bus, natb dev, natb fun);
}
/// @}

namespace serial {
/// @addtogroup serial
/// @{

	const ioaddr iCOM1_RBR = 0x03F8;
	const ioaddr iCOM1_THR = 0x03F8;
	const ioaddr iCOM1_LSR = 0x03FD;
	const ioaddr iCOM1_LCR = 0x03FB;
	const ioaddr iCOM1_DLR_LSB = 0x03F8;
	const ioaddr iCOM1_DLR_MSB = 0x03F9;
	const ioaddr iCOM1_IER = 0x03F9;
	const ioaddr iCOM1_MCR = 0x03FC;
	const ioaddr iCOM1_IIR = 0x03FA;

	const ioaddr iCOM2_RBR = 0x02F8;
	const ioaddr iCOM2_THR = 0x02F8;
	const ioaddr iCOM2_LSR = 0x02FD;
	const ioaddr iCOM2_LCR = 0x02FB;
	const ioaddr iCOM2_DLR_LSB = 0x02F8;
	const ioaddr iCOM2_DLR_MSB = 0x02F9;
	const ioaddr iCOM2_IER = 0x02F9;
	const ioaddr iCOM2_MCR = 0x02FC;
	const ioaddr iCOM2_IIR = 0x02FA;

/// @}
}


/// @}

namespace heap {
/// @addtogroup heap
/// @{

	/// @brief descrittore di una zona dello heap
	///
	/// Lo heap è composto da zone di memoria libere e occupate. Ogni zona di memoria
	/// è identificata dal suo indirizzo di partenza e dalla sua dimensione.
	/// Ogni zona di memoria contiene, nei primi byte, un descrittore di zona di
	/// memoria (di tipo des_mem) con un campo "dimensione" (dimensione in byte
	/// della zona, escluso il descrittore stesso) e un campo "next", significativo
	/// solo se la zona è libera, nel qual caso il campo punta alla prossima zona
	/// libera. Si viene quindi a creare una lista delle zone libere, la cui testa
	/// è puntata dalla variabile @ref memlibera (allocata staticamente). La lista è
	/// mantenuta ordinata in base agli indirizzi di partenza delle zone libere.
	struct alignas(sizeof(natq)) des_mem {
		natq dimensione;
		natq next;
	};

	/// testa della lista delle zone libere
	extern des_mem* memlibera;

	/// @brief converte da intero a puntatore a descrittore
	/// @param p	intero da convertire
	/// @return	puntatore alla zona di memoria
	static inline des_mem* natq2desmem(natq p)
	{
		return ptr_cast<des_mem>(p);
	}

	/// @brief converte da puntatore a descrittore a intero
	/// @param	p puntatore al descrittore
	/// @return	intero corrispondente
	static inline natq desmem2natq(des_mem *p)
	{
		return int_cast<natq>(p);
	}

	/// @brief aggiunge memoria allo heap
	/// @param indirizzo	indirizzo di partenza della memoria da aggiungere
	/// @param quanti	quanti byte aggiungere
	/// @return		false se non è stato possibile aggiungere la memoria
	bool free_interna(void* indirizzo, size_t quanti);
/// @}
}

namespace rnd {
	extern natl seed;
};

struct exit_rec {
	void (*fun)(void *);
	void *arg;
};

static const int MAX_EXIT_FUNCS = 16;
extern exit_rec exit_funcs[MAX_EXIT_FUNCS];
extern int next_exit_func;
