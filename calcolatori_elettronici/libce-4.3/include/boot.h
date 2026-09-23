/// @file boot.h
/// @brief strutture dati che boot64 passa al primo modulo

/// @addtogroup boot
/// @{

/// Numero massimo di moduli
#define MAX_MODULES 3
/// Numero massimo di segmenti ELF
#define MAX_SEGMENTS 4

/// Descrittore di segmento ELF
struct alignas(8) boot64_segment {
	/// offset del segmento all'iterno del modulo
	natq offset;
	/// indirizzo che dovrà avere il segmento in memoria
	natq vaddr;
	/// dimensione che il segmento dovrà avere in memoria
	natq size;
	/// dimensione del segmento nel modulo
	natq memsize;
	/// flag con cui mappare il segmento
	natq flags;
};

/// Descrittore di modulo
struct alignas(8) boot64_modinfo {
	/// indirizzo del modulo in memoria fisica
	natq mod_start;
	/// limite del modulo in memoria fisica
	natq mod_end;
	/// punto di ingresso del modulo
	natq entry_point;
	/// base della tabella delle eccezioni del modulo
	natq eh_frame_base;
	/// dimensione della tabella delle eccezioni del modulo
	natq eh_frame_size;
	/// numero di segmenti ELF contenuti nel modulo
	natq numseg;
	/// descrittori dei segmenti ELF
	boot64_segment segments[MAX_SEGMENTS];
};

/// Informazioni dal boot loader
struct alignas(8) boot64_info {
	/// primo chunk libero nello heap del boot loader
	natq memlibera;
	/// indirizzo di punt_nucleo nel TSS
	natq tss_punt_nucleo;
	/// numero di moduli caricati
	natq nummod;
	/// descrittori dei moduli
	boot64_modinfo mod[MAX_MODULES];
};

/// variabile globale in cui _start copia l'indirizzo del boot64_info ricevuto
extern boot64_info *boot_info;
/// variabile globale in cui _start copia l'indirizzo di punt_nucleo nel TSS
extern natq tss_punt_nucleo;
/// @}
