/*! @file libce.h
 * @brief File che va incluso sempre per primo
 */

//////////////////////////////////////////////////////////////////////////////
/// @addtogroup const Costanti
///
/// Alcune costanti di uso comune. Usiamo delle macro in modo da poterle
/// usare anche in assembler.
/// @{
//////////////////////////////////////////////////////////////////////////////

/// kibibyte
#define KiB			1024ULL
/// mibibyte
#define MiB			(1024*KiB)
/// gibibyte
#define GiB			(1024*MiB)
/// dimensione in byte di una pagina o di un frame
#define DIM_PAGINA		(4*KiB)
/// dimensione in byte di un blocco dell'hard disk
#define DIM_BLOCK		512ULL
/// DPL del livello sistema
#define LIV_SISTEMA		0
/// DPL del livello utente
#define LIV_UTENTE		3
/// @}


#ifndef __ASSEMBLER__
//////////////////////////////////////////////////////////////////////////////
/// @addtogroup types Tipi base
///
/// Alcuni tipi di uso comune, più altri la cui definizione è richiesta dallo
/// standard.
/// @{
/////////////////////////////////////////////////////////////////////////////

/// indirizzo nello spazio di I/O
using ioaddr = unsigned short;
/// naturale su un byte
using natb   = unsigned char;
/// naturale su due byte
using natw   = unsigned short;
/// naturale su 4 byte
using natl   = unsigned int;

/// @addtogroup archtypes Tipi dipendenti dall'architettura
///
/// Questi tipi sono definiti in modo diverso se la compilazione è a 32 bit
/// (usata dal boot loader) o a 64 bit. Per `natq`, `vaddr` e `paddr` vogliamo che
/// la dimensione in byte sia sempre 8, indipendentemente dalla modalità a 32 o
/// 64 bit. Gli altri tipi (`size_t`, `ssize_t`, ...) hanno definizione diverse
/// decise dallo standard.
/// @{
#if defined(__x86_64__) || defined(CE_UTILS)
/// naturale su 8 byte (64bit)
using natq   = unsigned long;
/// indirizzo virtuale (64bit)
using vaddr  = unsigned long;
/// indirizzo fisico (64bit)
using paddr  = unsigned long;
/// @name Tipi richiesti dallo standard (64bit)
/// @{

/// tipo restituito da sizeof
typedef unsigned long size_t;
/// tipo che può contenere una dimensione o un errore
typedef long ssize_t;
/// tipo che può contenere il risultato della sottrazione tra due puntatori
typedef long ptrdiff_t;
/// tipo intero più capiente supportato dal sistema
typedef long intmax_t;
/// tipo intero senza segno più capiente supportato dal sistema
typedef unsigned long uintmax_t;
/// tipo senza segno che può contenere il valore di un puntatore
typedef unsigned long uintptr_t;
/// @}
#else
/// naturale su 8 byte (32bit)
using natq   = unsigned long long;
/// indirizzo virtuale (32bit)
using vaddr  = unsigned long long;
/// indirizzo fisico (32bit)
using paddr  = unsigned long long;
/// @name Tipi richiesti dallo standard (32bit)
/// @{

/// tipo restituito da sizeof
typedef unsigned int size_t;
/// tipo che può contenere una dimensione o un errore
typedef int ssize_t;
/// tipo che può contenere il risultato della sottrazione tra due puntatori
typedef int ptrdiff_t;
/// tipo intero più capiente supportato dal sistema
typedef int intmax_t;
/// tipo intero senza segno più capiente supportato dal sistema
typedef unsigned int uintmax_t;
/// tipo senza segno che può contenere il valore di un puntatore
typedef unsigned int uintptr_t;
/// @}
#endif
/// @}
/// @}


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup util Funzioni di utilità generale
///
/// Le funzioni non inline sono definite nella directory `util`, ciascuna
/// nel proprio file.
/// @{
//////////////////////////////////////////////////////////////////////////////

/*! @brief Restituisce il massimo tra due valori confrontabili.
 * @tparam T	tipo dei valori (deve definire `<`) 
 * @param a	primo valore da confrontare
 * @param b	secondo valore da confrontare
 * @return	massimo tra _a_ e _b_
 */
template<typename T> T max(T a, T b) { return a < b ? b : a; }

/*! @brief Restituisce il minimo tra due valori confrontabili.
 * @tparam T	tipo dei valori (deve definire `<`)
 * @param a	primo valore da confrontare
 * @param b	secondo valore da confrontare
 * @return	minimo tra _a_ e _b_
 */
template<typename T> T min(T a, T b) { return a < b ? a : b; }

/*! @brief Scrive lo stesso valore in tutti i byte di un intervallo.
* @param dest	indirizzo base dell'intervallo
* @param c	valore da scrivere
* @param n	dimensione in byte dell'intervallo
* @return	_dest_
*/
void *memset(void *dest, int c, size_t n);

/*! @brief Copia un intervallo di memoria su un altro (i due intervalli
* 	   non possono sovrapporsi).
* @param dest	base dell'intervallo di destinazione
* @param src	base dell'intervallo sorgente
* @param n	dimensione in byte dei due intervalli
* @return	_dest_
*/
void *memcpy(void *dest, const void *src, size_t n);

/*! @brief Calcola la lunghezza di una stringa.
* @param str	stringa terminata con `\0`
* @return	lunghezza della stringa (escluso il terminatore)
*/
size_t strlen(const char str[]);

#include <stdarg.h> // file 'magico' che ci permette di usare le funzioni variadiche
/*! @brief Scrittura formattata su schermo.
 *
 * Segue la stessa sintassi della `printf(3)` della libreria standard del C,
 * con l'esclusione dei parametri di tipo `float` e `double`.
 *
 * @param fmt	la stringa di formato
 * @param ...	argomenti richiesti da _fmt_
 * @return	il numero di caratteri stampati
 *
 * @note La strana stringa `__attribute__((format(printf, 1, 2)))` introduce
 * una estensione di gcc.  In questo caso dice al compilatore che _fmt_
 * (argomento 1) segue la sintassi della `printf(3)` standard e che la funzione
 * si aspetta un numero variabile di argomenti a partire da quello in seconda
 * posizione . Ad ogni invocazione di questa funzione con una stringa _fmt_
 * nota a tempo di compilazione, il compilatore controllerà che gli argomenti
 * attuali corrispondano in tipo e numero con quelli richiesti da _fmt_,
 * emettendo dei warning in caso contrario.
 */
int printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/*! @brief Scrittura formattata su un buffer in memoria.
 *
 * Segue la stessa sintassi della `snprintf(3)` della libreria standard del C,
 * con l'esclusione dei parametri di tipo `float` e `double`.
 *
 * @param buf	buffer di destinazione
 * @param n	dimensione in byte del buffer di destinazione
 * @param fmt	stringa di formato
 * @param ...	argomenti richiesti da _fmt_
 * @return	il numero di caratteri dell'output completo (se >= _n_ l'output
 * 		è stato troncato)
 *
 * @note si veda @ref printf per il significato di `__attribute__`
 */
int snprintf(char *buf, natl n, const char *fmt, ...) __attribute__((format(printf, 3, 4)));

/*! @brief Livello di severità del messaggio inviato al log
 *   (usato per colorare i messaggi ove previsto)
 */
enum log_sev {
	LOG_DEBUG, 	//!< debugging
	LOG_INFO,	//!< informazione
	LOG_WARN,	//!< avviso
	LOG_ERR,	//!< errore
	LOG_USR		//!< messaggio proveniente da livello utente
};

/*! @brief Invio di un messaggio formattato sul log.
 *
 * Si possono usare gli stessi operatori di @ref printf.
 *
 * Nella configurazione di default il messaggio viene visualizzato sul terminale
 * dal quale è stato lanciato QEMU. Il messaggio sarà preceduto dall'id del processo
 * che lo ha inviato (se disponibile) e da una stringa di tre lettere che identifica
 * la severità del messaggio.
 *
 * @param sev	severità del messaggio
 * @param fmt	stringa di formato
 * @param ...   argomenti richiesti da _fmt_
 *
 * @note si veda @ref printf per il significato di `__attribute__`
 */
extern "C" void flog(log_sev sev, const char* fmt, ...) __attribute__((format(printf, 2, 3)));

/*! @brief Stampa un messaggio e attende che venga premuto il tasto ESC.
 */
void pause();

/*! @brief Invia sul log lo stato di tutti i registri e lo stack backtrace al momento della
 * chiamata
 *
 * @param sev	severità dei messaggi da inviare al log
 */
extern "C" void dump_status(log_sev sev);

/*! @brief Invia un messaggio sul log (severità ERR) ed esegue lo shutdown.
 *
 * Si possono usare gli stessi operatori di @ref printf.
 *
 * @param fmt	stringa di formato
 * @param ...   argomenti richiesti da _fmt_
 *
 * @note si veda @ref printf per il significato di `__attribute__`
 */
[[noreturn]] void fpanic(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/*! @brief Converte da intero a puntatore non void.
 *
 * L'intero deve essere allineato correttamente e deve poter essere contenuto
 * in un puntatore (4 byte a 32 bit, 8 byte a 64 bit) In caso contrario la
 * funzione chiama @ref fpanic().
 *
 * @tparam To	tipo degli oggetti puntati
 * @tparam From	un tipo intero
 * @param v 	valore (di tipo _From_) da convertire
 * @return	puntatore a _To_
 */
template<typename To, typename From>
static inline __attribute__((always_inline)) To* ptr_cast(From v)
{
	uintptr_t vp = static_cast<uintptr_t>(v);
	unsigned long long v_ = static_cast<unsigned long long>(v);

	if constexpr (From(-1) < From(0)) {
		if (v < 0)
			fpanic("Conversione di %lld a puntatore: intero negativo",
					static_cast<long long>(v));
	}

	if (vp & (alignof(To) - 1)) {
		fpanic("Conversione di %llx a puntatore: non allineato a %zu", v_, alignof(To));
	}

	if (vp != v_) {
		fpanic("Conversione di %llx a puntatore: perdita di precisione", v_);
	}

	return reinterpret_cast<To*>(vp);
}

/*! @brief Converte da intero a puntatore a void.
 *
 * L'intero deve poter essere contenuto in un puntatore (4 byte a 32 bit, 8
 * byte a 64 bit) In caso contrario la funzione chiama @ref fpanic().
 *
 * @tparam From un tipo intero
 * @param v 	valore (di tipo _From_) da convertire
 * @return	puntatore a void
 */
template<typename From>
static inline __attribute__((always_inline)) void* voidptr_cast(From v)
{
	uintptr_t vp = static_cast<uintptr_t>(v);

	if (vp != v) {
		fpanic("Conversione di %llu a puntatore: perdita di precisione",
				static_cast<unsigned long long>(v));
	}

	return reinterpret_cast<void*>(vp);
}

/*! @brief Converte da puntatore a intero.
 *
 * Il tipo _To_ deve essere sufficientemente grande per contenere l'indirizzo.
 * In caso contrario la funzione chiama @ref fpanic().
 *
 * @tparam To	un tipo intero
 * @tparam From tipo degli oggetti puntati
 * @param p	puntatore da convertire
 * @return	valore di _p_ convertito a intero
 */
template<typename To, typename From>
static inline __attribute__((always_inline)) To int_cast(From* p)
{
	uintptr_t vp = reinterpret_cast<uintptr_t>(p);
	To v = static_cast<To>(vp);

	if (vp != v) {
		fpanic("Conversione di %p a intero: perdita di precisione", p);
	}

	return v;
}

/*! @brief Restituisce il più piccolo multiplo di _a_ maggiore o uguale a _v_.
 */
static inline natq allinea(natq v, natq a) {
	v = (v % a == 0 ? v : ((v + a - 1) / a) * a);
	return v;
}

/*! @brief Restituisce il più piccolo puntatore a _T_ allineato ad _a_ e maggiore
 *         o uguale a _p_.
 */
template<typename T>
static inline T* allinea_ptr(T* p, natq a) {
	natq v = int_cast<natq>(p);
	v = allinea(v, a);
	return ptr_cast<T>(v);
}

/*! @brief Restituisce un intero pseudo-causale.
 */
long int random();

/*! @brief Imposta il seme iniziale del generatore di numeri pseudo-casuali.
 *
 * @param seed 	valore del seme
 */
void setseed(natl seed);


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup heap Memoria dinamica
///
/// Queste funzioni definiscono un semplice allocatore di memoria che può
/// essere usato per implementare gli operatori `new` e `delete`.  La zona di
/// memoria da usare come heap va prima inizializzata con @ref heap_init().
///
/// Le funzioni sono definite nella directory `heap`.
///
/// @{
//////////////////////////////////////////////////////////////////////////////

/*! @brief Inizializza un intervallo di memoria in modo che possa essere
 *	   usata con @ref alloc(), @ref alloc_aligned() e @ref dealloc().
 *
 * L'intervallo deve essere accessibile in lettura e scrittura.  L'allocatore
 * mantiene una lista di chunk liberi e i descrittori dei chunk sono allocati
 * nell'intervallo stesso.
 *
 * @param start		base dell'intervallo
 * @param size		dimensione in byte dell'intervallo
 */
void heap_init(void *start, size_t size);

/*! @brief Alloca una zona di memoria nello heap.
 *
 * @param dim		dimensione in byte della zona da allocare
 * @return		puntatore alla zona allocata se disponibile,
 * @return		`nullptr` altrimenti
 */
void* alloc(size_t dim);

#ifndef CE_UTILS
namespace std {
/*! @brief Tipo standard per la definzione della new con allineamento.
 */
enum class align_val_t : size_t {};
}
#endif

/*! @brief Alloca una zona di memoria nello heap, con vincoli di allineamento.
 *
 * L'indirizzo restituito sarà multiplo dell'allineamento richiesto.
 *
 * @param dim		dimensione in byte della zona da allocare
 * @param align		allineamento richiesto
 * @return		puntatore alla zona allocata se disponibile,
 * 			`nullptr` altrimenti
 */
void* alloc_aligned(size_t dim, std::align_val_t align);

/*! @brief Dealloca una zona di memoria, restituendola allo heap.
 *
 * @param p		puntatore alla zona da deallocare.
 *
 * Il puntatore deve essere stato precedentemente ottenuto tramite
 * @ref alloc() o @ref alloc_aligned().
 */
void dealloc(void *p);

/*! @brief	Memoria libera nello heap.
 *
 * @return quantità di memoria attualmente libera nello heap
 */
size_t disponibile();

/*!
 * @brief Accedi alla testa della lista dei chunk liberi
 *
 * @return indirizzo della testa della lista dei chunk liberi
 */
natq heap_getinitmem();


/// @name Overload standard.
/// Overloading degli operatori di default normalmente forniti dalla dalla
/// libreria standard del C++. Si limitano a richiamare in modo appropriato
/// `operator new` e `operator delete`, che devono esssere definiti a parte.
/// @{

/// versione di new per l'allocazione di array
void *operator new[](size_t s);
/// versione di new per l'allocazione di array con allineamento
void *operator new[](size_t s, std::align_val_t a);
/// versione di delete con dimensione esplicita
void operator delete(void *p, size_t s);
/// versione di delete con dimensione esplicita e allineamento
void operator delete(void *p, size_t s, std::align_val_t);
/// versione di delete per la deallocazione degli array
void operator delete[](void *p);
/// versione di delete per la deallocazione degli array con dimensione esplicita
void operator delete[](void *p, size_t s);
/// @}

/// @name Allocazione e deallocazione per i programmi bare
/// I programmi bare possono usare le definizioni di `operator new` e `operator
/// delete` fornite dalla libce. Queste si limitano ad usare alloc(),
/// alloc_aligned() e dealloc().
/// @{

void *operator new(size_t s);
void *operator new(size_t, std::align_val_t a);
void operator delete(void *p);
/// @}
/// @}
/// @}


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup IO Interazione con le interfaccie di I/O
///
/// Queste sono le funzioni definite e usate in esempiIO e nei moduli sistema e
/// IO del nucleo. Le funzioni di ogni interfaccia sono raggruppate nel proprio
/// namespace e definite in una directory che ha lo stesso nome del namespace.
/// @{
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup rw Lettura e scrittura nello spazio di I/O
///
/// Funzioni generiche per l'accesso allo spazio di I/O. Dal momento che usano
/// le istruzioni IN e OUT sono definite in assembler. Le definizioni si
/// trovano nella directory `as64` (versione a 64 bit) e `as32` (versione a 32bit)
/// @{
//////////////////////////////////////////////////////////////////////////////
/*! @brief Legge un byte da una porta di I/O.
 * 
 * @param reg	indirizzo della porta nello spazio di I/O
 * @return 	byte letto
 */
extern "C" natb inputb(ioaddr reg);

/*! @brief Legge una word (2 byte) da una porta di I/O.
 * 
 * @param reg	indirizzo della porta nello spazio di I/O
 * @return 	word letta
 */
extern "C" natw inputw(ioaddr reg);

/*! @brief Legge un long (4 byte) da una porta di I/O.
 * 
 * @param reg	indirizzo della porta nello spazio di I/O
 * @return 	long letto
 */
extern "C" natl inputl(ioaddr reg);

/*! @brief Legge una successione di word (2 byte) da una porta di I/O.
 * 
 * @param reg	indirizzo della porta nello spazio di I/O
 * @param vetti	buffer in cui ricevere le word lette
 * @param n	numero di word da leggere
 */
extern "C" void inputbw(ioaddr reg, natw vetti[], int n);

/*! @brief Invia un byte ad una porta di I/O.
 *
 * @param a	byte da inviare
 * @param reg	indirizzo della porta nello spazio di I/O
 */
extern "C" void outputb(natb a, ioaddr reg);

/*! @brief Invia una word (2 byte) ad una porta di I/O.
 *
 * @param a	word da inviare
 * @param reg	indirizzo della porta nello spazio di I/O
 */
extern "C" void outputw(natw a, ioaddr reg);

/*! @brief Invia un long (4 byte) ad una porta di I/O.
 *
 * @param a	long da inviare
 * @param reg	indirizzo della porta nello spazio di I/O
 */
extern "C" void outputl(natl a, ioaddr reg);

/*! @brief Invia una successione di word (2 byte) ad una porta di I/O.
 * 
 * @param vetto	buffer contenente le word da inviare
 * @param n	numero di word da inviare
 * @param reg	indirizzo della porta nello spazio di I/O
 */
extern "C" void outputbw(natw vetto[], int n, ioaddr reg);
/// @}


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup kbd Tastiera
///
/// Dispensa: <https://calcolatori.iet.unipi.it/resources/periferiche.pdf>
///
/// EsempiIO: tastiera-1, tastiera-2
/// @{
//////////////////////////////////////////////////////////////////////////////

/// namespace per le risorse della tastiera
namespace kbd {

/*! @brief Restituisce l'ultimo codice di scansione ricevuto dalla tastiera
 * 	   (esegue una attesa attiva fino a quando il codice non è disponibile).
 *
 * @return codice di scansione
 */
natb get_code();

/*! @brief Converte un codice di scansione nel corrispondente codice ASCII.
 *
 * @param c	codice di scansione da convertire
 * @return	codice ASCII corrispondente; 0 se il codice è sconosciuto
 */
char conv(natb c);

/*! @brief Restituisce il codice ASCII dell'ultimo carattere letto dalla tastiera
 *	   (esegue una attesa attiva fino a quando il carattere non è disponibile).
 *
 * @return 	carattere ricevuto; 0 se è stato ricevuto un codice sconosciuto
 */
char char_read();

/*! @brief Restituisce il codice ASCII corrispondente al codice di scansione contenuto in RBR
 *         (non esegue una attesa attiva).
 *
 * La funzione assume che sia già noto che RBR contiene un nuovo valore,
 * per esempio perché è stata ricevuta una richiesta di interruzione dalla tastiera.
 *
 * @return	carattere letto; 0 se RBR conteneva un codice sconosciuto
 */
char char_read_intr();

/*! @brief Abilita l'interfaccia della tastiera a generare richieste di interruzione.
 */
void enable_intr();

/*! @brief Disabilita l'interfaccia della tastiera a generare richieste di interruzione.
 */
void disable_intr();

/*! @brief Svuota il buffer della tastiera.
 */
void drain();

}
/// @}


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup vid Video in modalità testo
///
/// Dispensa: <https://calcolatori.iet.unipi.it/resources/periferiche.pdf>
///
/// EsempiIO: video-testo-1, video-testo-2
/// @{
//////////////////////////////////////////////////////////////////////////////

/// namespace per le risorse del video in modalità testo
namespace vid {

/*! @brief Ripulisce il video.
 */
void clear();

/*! @brief Ripulisce il video e setta l'attributo colore.
 *
 * @param col	attributo colore
 */
void clear(natb col);

/*! @brief Scrive un carattere nella posizione corrente del cursore.
 *
 * @param c	codice ASCII del carattere da scrivere
 */
void char_write(char c);

/*! @brief Scrive una stringa (null-terminated) a partire dalla posizione corrente del cursore.
 *
 * @param str	stringa da scrivere
 */
void str_write(const char str[]);

/*! @brief Scrive un carattere in una posizione qualsiasi dello schermo.
 *
 * @param c	codice ASCII del carattere da scrivere
 * @param x	colonna in cui scrivere
 * @param y	riga in cui scrivere
 */
void char_put(char c, natw x, natw y);

/*! @brief Restituisce un riferimento ad una posizione dello schermo, date
 * 	   le coordinate.
 *
 * @param x	colonna della posizione richiesta
 * @param y	riga della posizione richiesta
 * @return	riferimento alla corrispondente word
 */
volatile natw& pos(natw x, natw y);

/*! @brief Restituisce il numero di colonne del video (modalità testo).
 *
 * @return 	numero di colonne
 */
natl cols();

/*! @brief Restituisce il numero di righe del video (modalità testo).
 *
 * @return	numero di righe
 */
natl rows();

}
/// @}


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup svga Video in modalità grafica
///
/// Dispensa: <https://calcolatori.iet.unipi.it/resources/periferiche.pdf>
///
/// EsempiIO: svga-1, svga-2, svga-2
/// @{
//////////////////////////////////////////////////////////////////////////////

/// namespace per le risorse legate al video in modalità grafica
namespace svga {

/*! @brief Configura la modalità grafica a 256 colori.
 *
 * @param max_screenx	pixel orizzontali
 * @param max_screeny	pixel verticali
 * @return		indirizzo del framebuffer
 */
volatile natb* config(natw max_screenx, natw max_screeny);

}
/// @}

//////////////////////////////////////////////////////////////////////////////
/// @addtogroup libtimer Timer
///
/// Dispensa: <https://calcolatori.iet.unipi.it/resources/periferiche.pdf>
///
/// EsempiIO: timer
/// @{
//////////////////////////////////////////////////////////////////////////////

/// namespace per le risorse legate al timer
namespace timer {

/*! @brief Programma il timer 0 in modo che invii una richiesta di interruzione
 * 	   periodica.
 *
 * @param N	periodo delle richieste di interruzione
 */
void start0(natw N);

/*! @brief Programma il timer 2 in modo che produca un'onda quadra.
 *
 * @param N	periodo dell'onda quadra
 */
void start2(natw N);

/*! @brief Abilita lo speaker a ricevere il segnale proveniente dal timer 2.
 */
void enable_spk();

/*! @brief Disabilita lo speaker a ricevere il segnale proveniente dal timer 2.
 */
void disable_spk();

}
/// @}

//////////////////////////////////////////////////////////////////////////////
/// @addtogroup hd Hard disk
///
/// Dispensa: <https://calcolatori.iet.unipi.it/resources/periferiche.pdf>
///
/// EsempiIO: hard-disk-1, hard-disk-2
/// @{
//////////////////////////////////////////////////////////////////////////////

/// namespace per le risorse legate all'hard disk
namespace hd {

/*! @brief Possibili valori da scrivere nel registro di comando dell'hard disk.
 */
enum cmd {
	WRITE_SECT = 0x30,	//!< scrittura di settori
	READ_SECT  = 0x20,	//!< lettura di settori
	WRITE_DMA  = 0xCA,	//!< scrittura di settori in DMA
	READ_DMA   = 0xC8	//!< lettura di settori in DMA
};

/*! @brief Avvia una operazione sull'hard disk.
 *
 * @param lba		logical block address del primo settore
 * @param quanti	numero di settori
 * @param cmd		codice del comando
 */
void start_cmd(natl lba, natb quanti, cmd cmd);

/*! @brief Scrive un settore nel buffer interno dell'interfaccia dell'hard disk
 *
 * @param buf		buffer contenente il settore da scrivere
 */
void output_sect(natb *buf);

/*! @brief Legge un settore dal buffer interno dell'interfaccia dell'hard disk
 *
 * @param buf		buffer che dovrà ricevere il settore
 */
void input_sect(natb *buf);

/*! @brief Abilita l'interfaccia dell'hard disk a generare richieste di interruzione.
 */
void enable_intr();

/*! @brief Disabilita l'interfaccia dell'hard disk a generare richieste di interruzione.
 */
void disable_intr();

/*! @brief Azione di risposta alla richiesta di interruzione dell'interfaccia dell'hard disk.
 */
void ack();

}
/// @}

//////////////////////////////////////////////////////////////////////////////
/// @addtogroup pci Bus PCI
///
/// Dispensa: <https://calcolatori.iet.unipi.it/resources/pci.pdf>
///
/// EsempiIO: pci
/// @{
//////////////////////////////////////////////////////////////////////////////

/// namespace per le risorse legate al PCI
namespace pci {

/*! @brief Legge un byte dallo spazio di configurazione PCI.
 *
 * @param bus	numero di bus
 * @param dev	numero di dispositivo
 * @param fun	numero di funzione
 * @param regn	offset del byte
 * @return	byte letto
 */
natb read_confb(natb bus, natb dev, natb fun, natb regn);

/*! @brief Legge una word (2 byte) dallo spazio di configurazione PCI.
 *
 * @param bus	numero di bus
 * @param dev	numero di dispositivo
 * @param fun	numero di funzione
 * @param regn	offset della word
 * @return	word letta
 */
natw read_confw(natb bus, natb dev, natb fun, natb regn);

/*! @brief Legge un long (4 byte) dallo spazio di configurazione PCI.
 *
 * @param bus	numero di bus
 * @param dev	numero di dispositivo
 * @param fun	numero di funzione
 * @param regn	offset del long
 * @return	long letto
 */
natl read_confl(natb bus, natb dev, natb fun, natb regn);

/*! @brief Scrive un byte nello spazio di configurazione PCI.
 *
 * @param bus	numero di bus
 * @param dev	numero di dispositivo
 * @param fun	numero di funzione
 * @param regn	offset del byte su cui scrivere
 * @param data	valore da scrivere
 */
void write_confb(natb bus, natb dev, natb fun, natb regn, natb data);

/*! @brief Scrive una word (2 byte) nello spazio di configurazione PCI.
 *
 * @param bus	numero di bus
 * @param dev	numero di dispositivo
 * @param fun	numero di funzione
 * @param regn	offset della word su cui scrivere
 * @param data	valore da scrivere
 */
void write_confw(natb bus, natb dev, natb fun, natb regn, natw data);

/*! @brief Scrive un long (4 byte) nello spazio di configurazione PCI.
 *
 * @param bus	numero di bus
 * @param dev	numero di dispositivo
 * @param fun	numero di funzione
 * @param regn	offset del long su cui scrivere
 * @param data	valore da scrivere
 */
void write_confl(natb bus, natb dev, natb fun, natb regn, natl data);

/*! @brief Trova una funzione PCI dato il vendor ID e il device ID.
 *
 * In ingresso, _bus_ / _dev_ / _fun_ devono contenere le coordinate da cui iniziare la
 * ricerca. Al ritorno, se la ricerca ha avuto successo, contengono le
 * coordinate della funzione.
 *
 * @param[in,out] bus	numero di bus
 * @param[in,out] dev	numero di dispositivo
 * @param[in,out] fun	numero di funzione
 * @param vendorID	vendor ID della funzione cercata
 * @param deviceID	device ID della funzione cercata
 * @return		true se trovato, false altrimenti
 */
bool find_dev(natb& bus, natb& dev, natb& fun, natw vendorID, natw deviceID);

/*! @brief Trova una funzione PCI dato il Class Code.
 *
 * In ingresso, _bus_ / _dev_ / _fun_ devono contenere le coordinate da cui iniziare la
 * ricerca. Al ritorno, se la ricerca ha avuto successo, contengono le
 * coordinate della funzione.
 *
 * _code_ deve essere un array di 3 byte. In ingresso, uno o più di questi
 * byte possono contenere il valore 0xFF che vale come "jolly". In uscita,
 * se la ricerca ha avuto successo, i valori jolly saranno sostituiti dai
 * byte effettivamente contenuti nella funzione trovata.
 *
 * @param[in,out] bus	numero di bus
 * @param[in,out] dev	numero di dispositivo
 * @param[in,out] fun	numero di funzione
 * @param[in,out] code	il class code
 * @return		true se trovato, false altrimenti
 */
bool find_class(natb& bus, natb& dev, natb& fun, natb code[]);

/*! @brief Calcola le prossime coordinate nel bus PCI.
 *
 * Assumimiamo che le coordinate bus/dev/fun siano ordinate nel
 * modo seguente:
 *
 *     0/0/0, 0/0/1, 0/0/2, ..., 0/0/7,
 *     0/1/0, 0/1/1, 0/0/2, ..., 0/1/7,
 *     0/2/0, ...,               0/2/7,
 *     ...,
 *     0/31/0, ...,              0/31,7,
 *     1/0/0,
 *     ...,
 *     255/0/0, ...,           255/31/7.
 *
 * In ingresso _bus_ / _dev_ / _fun_ devono contenere delle coordinate valide.
 * Al ritorno, se le coordinate in ingresso non erano le ultime, _bus_ / _dev_ / _fun_
 * conterranno le coordinate successive.
 *
 * @param[in,out] bus	numero di bus
 * @param[in,out] dev	numero di dispositivo
 * @param[in,out] fun	numero di funzione
 * @return		false se la coordinata in ingresso era l'ultima,
 * 			true altrimenti
 */
bool next(natb& bus, natb& dev, natb& fun);

/*! @brief Decodifica il primo byte di un Class Code
 *
 * @param class_code	byte meno significativo di un Class Code
 * @return		stringa che descrive il Class Code
 */
const char* decode_class(natb class_code);

}
/// @}


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup bm PCI BUS Mastering ATA
///
/// Dispensa: <https://calcolatori.iet.unipi.it/resources/dma.pdf>
///
/// Specifiche: <https://calcolatori.iet.unipi.it/deep/idems100.pdf>
///
/// EsempiIO: bm-hard-disk
/// @{
//////////////////////////////////////////////////////////////////////////////

/// namespace per le risorse legate al PCI BUS Mastering
namespace bm {

/*! @brief cerca il prossimo bus master ATA.
 *
 * In ingresso _bus_ / _dev_ / _fun_ devono contenere le coordinate da cui iniziare la
 * ricerca. Al ritorno, se la ricerca ha avuto successo, contengono le
 * coordinate del bus master.
 *
 * @param[in,out] bus	numero del bus
 * @param[in,out] dev	numero del dispositivo
 * @param[in,out] fun	numero di funzione
 * @return	true se trovato, false altrimenti
 */
bool find(natb& bus, natb& dev, natb& fun);

/*! @brief inizializza un bus master.
 *
 * @param bus	numero di bus del bus master
 * @param dev	numero di dispositivo del bus master
 * @param fun	numero di funzione del bus master
 */
void init(natb bus, natb dev, natb fun);

/*! @brief prepara una operazione di bus mastering.
 *
 * @param prd	indirizzo fisico dell'array di descrittori
 * @param write	operazione di scrittura (true) o lettura (false)
 */
void prepare(paddr prd, bool write);

/*! @brief avvia l'operazione di bus mastering precedentemente preparata.
 */
void start();

/*! @brief azione di risposta alle richieste di interruzione del bus master
 */
void ack();

}
/// @}


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup serial Porte seriali
///
/// @note Le porte seriali non sono più trattate a lezione, ma sono usate per
/// implementare il log. Ci sono comunque due esempi in esempiIO: seriale-1
/// e seriale-2.
/// @{
//////////////////////////////////////////////////////////////////////////////

/// namespace per le risorse legate alle porte seriali
namespace serial {

/*! @brief inizializza la porta seriale 1.
 */
void init1();

/*! @brief invia un byte sulla porta seriale 1.
 *
 * @param c	byte da inviare
 */
void out1(char c);

/*! @brief riceve un byte dalla porta seriale 1.
 *
 * @return	byte ricevuo
 */
natb in1();

/*! @brief inizializza la porta seriale 2.
 */
void init2();

/*! @brief invia un byte sulla porta seriale 2.
 *
 * @param c	byte da inviare
 */
void out2(char c);

/*! @brief ricevi un byte dalla porta seriale 2.
 *
 * @return	byte ricevuto
 */
natb in2();

}
/// @}
/// @}


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup intr Interruzioni
///
/// Dispensa: https://calcolatori.iet.unipi.it/resources/interruzioni.pdf
/// @{
//////////////////////////////////////////////////////////////////////////////

/// @brief Interrupt Flag
///
/// Flag del registro RFLAGS. Se attivo abilita il processore ad accettare le
/// richieste di interruzione esterne mascherabili.
const natq BIT_IF = 1UL << 9;

/// @brief Trap Flag
///
/// Flag del registro RFLAGS. Se attivo, il processore solleva una eccezione
/// di tipo 1 (debug) dopo l'esecuzione di ogni istruzione.
const natq BIT_TF = 1UL << 8;


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup idt Tabella IDT
///
/// Queste funzioni sono definite nella directory `util`.
/// @{
//////////////////////////////////////////////////////////////////////////////

/*! @brief Inizializza un gate della IDT.
 *
 * Il gate viene inizializzato in ogni caso per portare il processore a
 * livello sistema.
 *
 * @param num		numero del gate (0-255)
 * @param routine	funzione da associare al gate
 * @param trap		se true, crea un gate di tipo trap
 * @param liv		DPL del gate (LIV_UTENTE o LIV_SISTEMA)
 */
extern "C" void gate_init(natb num, void routine(), bool trap = false, int liv = LIV_UTENTE);

/*! @brief Controlla che un gate non sia già occupato.
 *
 * @param num		numero del gate
 * @return		true se il gate è occupato (P==1), false altrimenti
 */
extern "C" bool gate_present(natb num);
/// @}


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup apic APIC
/// 
/// Dispensa: https://calcolatori.iet.unipi.it/resources/interruzioni.pdf
///
/// esempiIO: interrupt-1, interrupt-2, interrupt-3
/// @{
//////////////////////////////////////////////////////////////////////////////

/// namespace per le risorse legate all'APIC
namespace apic {

/*! @brief Massimo numero di IRQ supportati dall'APIC
 */
static const int MAX_IRQ = 24;

/*! @brief Inizializza l'APIC.
 *
 * Abilita l'APIC, quindi maschera tutti gli IRQ e azzera gli altri campi.
 *
 * @return		false in caso di errore, true altrimenti
 */
bool init();

/*! @brief Invia l'End Of Interrupt.
 */
void send_EOI();

/*! @brief Maschera/smaschera una sorgente di richieste di interruzione.
 *
 * @param irq		irq da mascherare/smascherare
 * @param enable	maschera se true, smaschera se false
 */
void set_MIRQ(natl irq, bool enable);

/*! @brief Setta la modalità di riconoscimento di una sorgente di richieste.
 * 	   di interruzione.
 *
 * @param irq		irq su cui agire
 * @param enable	true  -> riconoscimento sul livello
 * 			false -> riconoscimento sul fronte
 */
void set_TRGM(natl irq, bool enable);

/*! @brief Imposta il vettore di interruzione associato ad una sorgente di
 *         richieste di interruzione.
 *
 * @param irq		irq a cui associare il vettore
 * @param vec		vettore di interruzione
 */
void set_VECT(natl irq, natb vec);

}

/*! @brief Invia l'End Of Interrupt.
 *
 * Funzione analoga a apic::send_EOI, ma invocabile dall'assembly più comodamente
 */
extern "C" void apic_send_EOI();
/// @}
/// @}


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup libexc Eccezioni
///
/// Dispensa: https://calcolatori.iet.unipi.it/resources/eccezioni.pdf
/// @{
//////////////////////////////////////////////////////////////////////////////

///  @name Eccezione di Page Fault (14)
///
///  Il microprogramma di gestione delle eccezioni di page fault lascia in cima
///  alla pila (oltre ai valori consueti) una parola quadrupla i cui 4 bit meno
///  significativi specificano più precisamente il motivo per cui si è
///  verificato un page fault.
///
///  @{

/// @brief Page fault causato da errore di protezione.
///
///  Se questo bit vale 1 la traduzione era presente, ma c'è stato un errore
///  diverso, per es. il processore si trovava a livello utente e la pagina era
///  di livello sistema (bit US = 0 in una qualunque delle tabelle dell'albero
///  che porta al descrittore della pagina). Se invece il bit PF_PROT è zero,
///  la pagina o una delle tabelle erano assenti (bit P = 0)
static const natq PF_PROT  = 1U << 0;

/// @brief Page fault con accesso in scrittura.
///
///  L'accesso che ha causato il page fault era in scrittura.
///
///  @note Questo non implica che la pagina non fosse scrivibile: il page fault
///  potrebbe essere stato causato da altro (per es. traduzione assente o
///  accesso da livello utente a pagina di livello sistema).
static const natq PF_WRITE = 1U << 1;

/// @brief Page fault con accesso da livello utente.
///
///  L'accesso che ha causato il fault è avvenuto mentre il processore si
///  trovava a livello utente.
///
///  @note Questo non implica che la pagina fosse invece di livello sistema:
///  il page fault potrebbe essere stato causato da altro (per es. traduzione
///  assente, o accesso in scrittura su pagina di sola lettura).
static const natq PF_USER  = 1U << 2;

/// @brief Page fault con bit riservati non validi.
///
///  Uno dei bit riservati nel descrittore di pagina o di tabella non
///  avevano il valore richiesto (il bit D deve essere 0 per i descrittori di
///  tabella e il bit PS deve essere 0 per i descrittori di pagina).
static const natq PF_RES   = 1U << 3;
/// @}

/// @name Eccezioni con codice di errore.
///
/// Alcune eccezioni, per lo più legate al meccanismo della segmentazione,
/// lasciano in pila un codice di errore che è l'offset all'interno di una delle
/// tre tabelle IDT, GDT o LDT (quest'ultima non usata da noi) e di tre bit che
/// specificano meglio il tipo di errore.
/// @{

/// @brief Eccezione causata da evento esterno.
///
/// L'eccezione è stata sollevata mentre il processore cercava di gestire un
/// evento esterno al programma (per esempio, una richiesta di interruzione
/// esterna).
static const natq SE_EXT   = 1U << 0;

/// @brief Eccezione durante accesso alla IDT.
///
/// L'eccezione è stata sollevata durante un accesso a un gate della IDT. In
/// questo caso il resto del codice di errore è l'offset del gate all'interno
/// della IDT.
static const natq SE_IDT   = 1U << 1;

/// @brief Table Indicator dell'eccezione.
///
/// Se @ref SE_IDT è 0, il bit SE_IDT indica se l'eccezione è stata sollevata
/// durante un accesso a un descrittore della LDT (SE_TI==1) o alla GDT
/// (SE_TI==0). In questo caso il resto del codice di errore è l'offset del
/// descrittore all'interno della tabella interessata.
static const natq SE_TI    = 1U << 2;
/// @}

/// @brief Decodifica delle eccezioni.
///
/// Invia sul log alcuni messaggi che mostrano i dettagli associati ad una
/// eccezione.
///
/// @param tipo	tipo dell'eccezione
/// @param errore eventuale codice di errore salvato in pila dal
/// 		 processore (0 se assente)
/// @param rip	istruction pointer salvato in pila dal processore
void log_exception(int tipo, natq errore, vaddr rip);
/// @}


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup autocorr Supporto per l'autocorrezione
///
/// Per permettere al sito dell'autocorrezione di eseguire i programmi in
/// maniera non interattiva e controllarne l'output, cambiamo il significato
/// di alcune funzioni quando è definita la macro AUTOCORR. In particolare,
/// redirigiamo printf() su autocorr_printf(), che invia i messaggi sul log
/// invece che sullo schermo della macchina QEMU, e redirigiamo pause()
/// suo autocorr_pause(), che ritorna immediatamente.
/// @{
//////////////////////////////////////////////////////////////////////////////

#ifdef AUTOCORR
int autocorr_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void autocorr_pause();
#define printf(fmt_, ...) 	autocorr_printf(fmt_, ##__VA_ARGS__)
#define pause() 		autocorr_pause()
#endif /*AUTOCORR */
/// @}


//////////////////////////////////////////////////////////////////////////////
/// @addtogroup kern Funzioni di uso meno generale, usate dal nucleo
///
/// Questa sezione raggruppa alcune funzioni che sono realizzate dalla libce
/// per comodità, ma è improbabile che sia necessario usarle al di fuori
/// degli usi specifici all'interno del nucleo.
/// @{
//////////////////////////////////////////////////////////////////////////////

/*! @brief Come @ref snprintf, ma usa una `va_list` esplicita invece di essere variadica.
 *
 * Questa funzione contiene l'implementazione vera e propria del parser delle
 * stringhe di formato ed è chiamata sia da @ref printf che da @ref snprintf.
 *
 * @param buf	buffer di destinazione
 * @param size	dimensione di _buf_
 * @param fmt	stringa di formato
 * @param ap	lista degli argomenti per _fmt_
 * @return	il numero di caratteri necessari per scrivere l'intera stringa
 */
int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);

/*! @brief Trova l'exception header all'interno di un file ELF caricato in memoria.
 *
 * L'exception header è generato dal compilatore e serve a recuperare le informazioni
 * per lo stack-unwinding. Lo usiamo per implementare il backtrace in caso di errori.
 *
 * @param elf			indirizzo virtuale dell'header ELF caricato in memoria
 * @param[out] eh_frame		indirizzo virtuale dell'exception header
 * @param[out] eh_frame_len	lunghezza dell'exception header
 * @return			true se trovata, false altrimenti
 */
bool find_eh_frame(vaddr elf, vaddr& eh_frame, natq& eh_frame_len);

/*! @brief Riavvia il sistema.
 *
 * Nella configurazione standard QEMU è stato impostato per fare lo shutdown
 * invece di riavviare, quindi questa funzione ha l'effetto di spegnere la
 * macchina virtuale.
 */
extern "C" void reboot();

/*! @brief Esegue l'istruzione `hlt`.
 *
 *  Mette in pausa il processore in attesa di una richiesta di interruzione
 *  esterna
 */
extern "C" void halt();

/*! @brief Esegue lo shutdown del sistema.
 *
 * Chiama reboot() ed esegue `hlt` disabilitando le interruzioni.
 */
extern "C" [[noreturn]] void end_program();

/*! @brief Funzione di basso livello per la scrittura sul log.
 *
 * @ref flog() formatta il messaggio e poi chiama do_log() per inviarlo effettivamente
 * (i programmi 'bare' e il nucleo possono usare la do_log fornita dalla libce,
 * che scrive direttamente sulla porta seriale, ma nei moduli I/O e utente do_log
 * è ridefinita in modo da invocare una primitiva di sistema)
 *
 * @param sev	severità del messaggio
 * @param buf	buffer contenente il messaggio
 * @param quanti dimensione in byte del messaggio
 */
extern "C" void do_log(log_sev sev, const char* buf, natl quanti);

extern char stack[], stack_end[];
/// @}

#else /* __ASSEMBLER__ */

//! @brief salva in pila tutti i registri generali.
.macro salva_registri
	pushq %rax
	pushq %rcx
	pushq %rdx
	pushq %rbx
	pushq %rsi
	pushq %rdi
	pushq %rbp
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
.endm

//! @brief ripristina tutti i registri generali dalla pila.
.macro carica_registri
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rbp
	popq %rdi
	popq %rsi
	popq %rbx
	popq %rdx
	popq %rcx
	popq %rax
.endm

#endif /* __ASSEMBLER__ */

#define CE_FAULT 0x0

//////////////////////////////////////////////////////////////////////////////
/// @addtogroup boot
/// @{
/// @addtogroup boothw
///
/// @{
//////////////////////////////////////////////////////////////////////////////

/// dimensione in byte di una entrata della GDT
#define DIM_GDT_ENTRY		8
/// @brief numero di righe della GDT
///
/// 5 entrate compresa la 0 che deve essere nulla, ma l'entrata del TSS occupa
/// due righe.
#define NUM_GDT_ENTRIES		6
/// dimensione in byte del segmento TSS
#define DIM_TSS			104

/// @name Indici all'interno della GDT
/// @{

/// segmento codice/sistema
#define ID_CODE_SYS		1
/// segmento codice/utente
#define ID_CODE_USR		2
/// segmento dati/utente
#define ID_DATA_USR		3
/// segmento TSS
#define ID_SYS_TSS		4
/// @}

/// @name Selettori di segmento
/// @{

/// selettore nullo
#define SEL_NULLO		0
/// selettore del segmento codice di livello sistema
#define SEL_CODICE_SISTEMA	((ID_CODE_SYS << 3) | LIV_SISTEMA)
/// selettore del segmento codice di livello utente
#define SEL_CODICE_UTENTE	((ID_CODE_USR << 3) | LIV_UTENTE)
/// selettore del segmento dati scrivibili di livello utente
#define SEL_DATI_UTENTE 	((ID_DATA_USR << 3) | LIV_UTENTE)
/// selettore del segmento TSS
#define SEL_SYS_TSS 		((ID_SYS_TSS  << 3) | LIV_SISTEMA)
/// @}
/// @}
/// @}
