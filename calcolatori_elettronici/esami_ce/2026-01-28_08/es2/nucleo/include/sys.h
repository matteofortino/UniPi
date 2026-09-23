/// @file sys.h
/// @brief Primitive comuni definite dal modulo sistema
///
/// Queste primitive possono essere usate sia dal modulo utente che
/// dal modulo I/O.

/// @name Primitive per la gestione dei processi
/// @{

/**
 * @brief Crea un nuovo processo
 *
 * Il nuovo processo eseguirà _f(a)_ con priorità _prio_ e a livello _liv_.
 *
 * Un processo non può usare questa primitiva per creare un processo a priorità
 * o livello maggiori dei propri.
 *
 * @param f		corpo del processo
 * @param a		parametro per il corpo del processo 
 * @param prio		priorità del processo 
 * @param liv		livello del processo (LIV_UTENTE o LIV_SISTEMA)
 *
 * @return 		id del nuovo processo, o 0xFFFFFFFF in caso di errore
 */
extern "C" natl activate_p(void f(natq), natq a, natl prio, natl liv);

/**
 * @brief Termina il processo corrente.
 *
 * I processi devono invocare questa primitiva per poter terminare.
 */
extern "C" void terminate_p();

/**
 * @brief Abortisce il processo corrente.
 *
 * Come terminate_p(), ma mostra lo stato del processo sul log.
 */
extern "C" void abort_p();

/// @}

/// @name Primitive per l'utilizzo dei semafori
/// @{

/**
 * @brief Crea un nuovo semaforo.
 *
 * @param val		numero di gettoni iniziali
 *
 * @return 		id del nuovo semaforo, o 0xFFFFFFFF in caso di errore
 */
extern "C" natl sem_ini(int val);

/**
 * @brief Estrae un gettone da un semaforo.
 *
 * @param sem		id del semaforo.
 */
extern "C" void sem_wait(natl sem);

/**
 * @brief Inserisce un gettone in un semaforo.
 *
 * @param sem		id del semaforo
 */
extern "C" void sem_signal(natl sem);

/// @}

/// @name Primitive per l'utilizzo del timer
/// @{

/**
 * @brief Sospende il processo corrente.
 *
 * @param n		numero di intervalli di tempo
 */
extern "C" void delay(natl n);

/// @}

/// @name Primitive di supporto al debugging
/// @{

/**
 * @brief Informazioni di debug
 *
 * Questa struttura contiene delle informazioni che sono usate nei testi d'esame
 * per eseguire alcuni controlli.
 */
struct meminfo {
	/// numero di byte liberi nello heap di sistema
	natl heap_libero;
	/// numero di frame liberi in M2
	natl num_frame_liberi;
	/// id del processo corrente
	natl pid;
};

/**
 * @brief Estrae informazioni di debug.
 *
 * @return		struttura contenente le informazioni
 */
extern "C" meminfo getmeminfo();

/**
 * @brief Invia un messaggio al log.
 *
 * Questa primitiva è usata dai moduli I/O e utente per inviare i propri
 * messaggi al log di sistema. 
 *
 * @note Il modulo sistema usa direttamente la do_log() definita in libce.
 *
 * @param sev		severità del messaggio
 * @param buf		buffer contenente il messaggio
 * @param quanti	lunghezza del messaggio
 */
extern "C" void do_log(log_sev sev, const char* buf, natl quanti);
/// @}

// ( ESAME 2026-01-28
/// @addtogroup esame
/// @{

/**
 * @brief Inizializza un nuovo RW.
 * @return id del RW, 0xFFFFFFFF in caso di errore
 */
extern "C" natl rw_init();

/**
 * @brief Tenta di acquisire il diritto di scrittura su un RW.
 *
 * Se ci sono già processi che hanno acquisito un diritto sul RW
 * e non lo hanno ancora rilasciato, sospende il processo in attesa
 * che le condizioni permettano l'acquisizione del diritto di scrittura.
 *
 * @pre Il processo non deve avere già qualche diritto sul RW.
 *
 * @param rw id del RW
 */
extern "C" void rw_acq_write(natl rw);

/**
 * @brief Tenta di acquisire il diritto di lettura su un RW.
 *
 * Se c'è un processo che ha acquisito il diritto di scrittura sul RW
 * e non lo ha ancora rilasciato, oppure se ci sono già @ref MAX_RW_READERS
 * processi che hanno acquisito il diritto di lettura e non lo hanno
 * ancora rilasciato, sospende il processo in attea che le condizioni
 * permettano l'acquisizione del diritto di lettura.
 *
 * @pre Il processo non deve avere già qualche diritto sul RW.
 *
 * @param rw id del RW
 */
extern "C" void rw_acq_read(natl rw);

/**
 * @brief Cede il diritto di scrittura su un RW.
 *
 * Se vi sono processi in attesa di acquisire un diritto di lettura
 * o scrittura sul RW, lo concede dando precedenza ai lettori
 * e cercando di concedere il diritto a più processi possibile,
 * nel rispetto dei vincoli.
 *
 * @pre Il processo deve avere il diritto di scrittura sul RW
 *
 * @param rw id del RW
 */
extern "C" void rw_rel_write(natl rw);

/**
 * @brief Cede il diritto di lettura su un RW.
 *
 * Se vi sono processi in attesa di acquisire un diritto di lettura
 * o scrittura sul RW, lo concede dando precedenza agli scrittori
 * e cercando di concedere il diritto a più processi possibile,
 * nel rispetto dei vincoli.
 *
 * @pre Il processo deve avere il diritto di lettura sul RW
 *
 * @param rw id del RW
 */
extern "C" void rw_rel_read(natl rw);

/// @}
// ESAME 2026-01-28 )
