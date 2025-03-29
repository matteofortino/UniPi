/*! @file vm.h
 * @brief file da includere (dopo libce.h) per usare le funzioni della memoria virtuale
 *
 */

/// @defgroup vm Funzioni per la memoria virtuale
///
/// Dispensa: <https://calcolatori.iet.unipi.it/resources/paginazione-libreria.pdf>
/// @{

/// @brief numero di livelli del TRIE (4 o 5)
/// @note Per usare i TRIE a 5 livelli basta modificare questa costante,
/// ricompilare e reinstallare la libreria, quindi ricompilare i programmi
/// che la usano (nucleo, esempiIO, etc.)
static const int MAX_LIV = 4;
/// massimo livello supportato per le pagine di grandi dimensioni
static const int MAX_PS_LVL = 2;
/// @brief ultimo bit significativo di un indirizzo virtuale
///
/// bit 47 se @ref MAX_LIV == 4, bit 56 se @ref MAX_LIV == 5.
static const natq VADDR_MSBIT = (1ULL << (12 + 9 * MAX_LIV - 1));
/// maschera per selezionare i bit significativi di un indirizzo virtuale
static const natq VADDR_MASK = VADDR_MSBIT - 1;

/*! @brief Normalizza un indirizzo.
 *
 * Se @ref MAX_LIV == 4 (TRIE a 4 livelli) pone i 16 bit più significativi
 * uguali al bit 47;
 * Se @ref MAX_LIV == 4 (TRIE a 5 livelli) pone i 7 bit più significativi
 * uguali al bit 56.
 *
 * @param a	indirizzo virtuale da normalizzare
 * @return	indiririzzo virtuale normalizato
 */
static inline constexpr vaddr norm(vaddr a)
{
	return (a & VADDR_MSBIT) ?	// se il bit più sign. è 1
		(a | ~VADDR_MASK) :	// setta tutti bit alti
		(a & VADDR_MASK);	// altrimenti resettali
}

/*! @brief Calcola la dimensione di una regione del TRIE dato il livello.
 *
 * @param liv	livello del TRIE (0 - @ref MAX_LIV)
 * @return	dimensione in byte della regione
 */
static inline constexpr natq dim_region(int liv)
{
	natq v = 1ULL << (liv * 9 + 12);
	return v;
}

/*! @brief Calcola la base della regione di un dato livello
 *         a cui appartiene un dato indirizzo virtuale.
 *
 * @param v	indirizzo virtuale
 * @param liv	livello della regione (0 - @ref MAX_LIV)
 * @return	base della regione
 */
static inline constexpr vaddr base(vaddr v, int liv)
{
	natq mask = dim_region(liv) - 1;
	return v & ~mask;
}

/*! @brief Calcola la base della prima regione di un dato
 * 	   livello che giace a destra di un dato indirizzo virtuale.
 *
 * @param v	indirizzo virtuale
 * @param liv	livello della regione (0 - @ref MAX_LIV)
 * @return	base della regione
 */
static inline constexpr vaddr limit(vaddr v, int liv)
{
	natq dr = dim_region(liv);
	natq mask = dr - 1;
	return (v + dr - 1) & ~mask;
}

/// descrittore di pagina o tabella
using tab_entry = natq;

/// @name Bit del byte di accesso
/// @{

/// bit di presenza
const natq BIT_P    = 1U << 0;
/// bit di lettura/scrittura
const natq BIT_RW   = 1U << 1;
/// bit utente/sistema
const natq BIT_US   = 1U << 2;
/// bit Page Wright Through
const natq BIT_PWT  = 1U << 3;
/// bit Page Cache Disable
const natq BIT_PCD  = 1U << 4;
/// bit di accesso
const natq BIT_A    = 1U << 5;
/// bit "dirty"
const natq BIT_D    = 1U << 6;
/// bit "page size"
const natq BIT_PS   = 1U << 7;
/// maschera per il byte di accesso
const natq ACCB_MASK  = 0x00000000000000FF;
/// maschera per l'indirizzo
const natq ADDR_MASK  = 0x7FFFFFFFFFFFF000;
/// @}

/*! @brief Estrae l'indirizzo fisico da un descrittore di pagina o tabella.
 *
 * @param e	descrittore di pagina o tabella
 * @return	indirizzo fisico contenuto in _e_
 */
static inline constexpr paddr extr_IND_FISICO(tab_entry e)
{
	return e & ADDR_MASK;
}

/*! @brief Imposta l'indirizzo fisico in un descrittore di
 *         pagina o tabella.
 *
 * @param e	riferimento a un descrittore di pagina o tabella
 * @param f	indirizzo fisico da impostare in _e_
 */
static inline void  set_IND_FISICO(tab_entry& e, paddr f)
{
	e &= ~ADDR_MASK;
	e |= f & ADDR_MASK;
}

/*! @brief Indice nelle tabelle.
 *
 * @param v	indirizzo virtuale
 * @param liv	livello della tabella (1 - @ref MAX_LIV)
 * @return	indice di _v_ nelle tabelle di livello _liv_
 */
static inline constexpr int i_tab(vaddr v, int liv)
{
	int shift = 12 + (liv - 1) * 9;
	natq mask = 0x1ffULL << shift;
	return (v & mask) >> shift;
}

/*! @brief Accesso ad un'entrata di una tabella.
 *
 * La funzione restituisce un riferimento tramite il quale è possibile
 * modificare il descrittore. Se viene modificato il bit di presenza, è compito
 * del chiamante di aggiornare anche il contatore delle entrate valide nella
 * tabella.
 *
 * @param tab	indirizzo fisico della tabella
 * @param i	indice in _tab_ (0 - 511)
 * @return	riferimento al descrittore _i_-esimo di _tab_
 */
static inline tab_entry& get_entry(paddr tab, natl i)
{
	tab_entry *pd = ptr_cast<tab_entry>(tab);
	return  pd[i];
}

/*! @brief	Scrive una entrata di una tabella.
 *
 * La fuzione usa @ref inc_ref() o @ref dec_ref() per aggiornare opportunamente il
 * contatore delle entrate valide della tabella, nel caso in cui il bit di
 * presenza cambi per effetto della scrittura.
 *
 * @param tab	indirizzo fisico della tabella
 * @param j	indice in _tab_ (0 - 511)
 * @param se	nuovo contenuto del descrittore _j_-esimo di _tab_
 */
void set_entry(paddr tab, natl j, tab_entry se);

/*! @brief	Copia descrittori da una tabella ad un'altra.
 *
 * La fuzione usa @ref inc_ref() o @ref dec_ref() per aggiornare opportunamente il
 * contatore delle entrate valide della tabella di destinazione, nel caso in
 * cui qualche bit di presenza cambi per effetto della copia.
 *
 * @param src	indirizzo fisico della tabella sorgente
 * @param dst	indirizzo fisico della tabella di destinazione
 * @param i	indice del primo descrittore da copiare (0 - 511)
 * @param n	numero di descrittori da copiare (0 - 512)
 */
void copy_des(paddr src, paddr dst, natl i, natl n);

/*! @brief	Inizializza (parte dei) descrittori di una tabella.
 *
 * La funzione inizializza uno o più descrittori della tabella con
 * lo stesso valore. È utile principalmente quanto questo valore è zero,
 * per resettare alcune entrate di una tabella.
 *
 * La fuzione usa @ref inc_ref() o @ref dec_ref() per aggiornare opportunamente il
 * contatore delle entrate valide della tabella, nel caso in cui qualche bit di
 * presenza cambi per effetto della scrittura.
 *
 * @param dst	indirizzo fisico della tabella
 * @param i	indice della prima entrata da sovrascrivere (0 - 511)
 * @param n	numero di entrate da sovrascrivere (0 - 512)
 * @param e	nuovo valore delle entrate (uguale per tutte)
 */
void set_des(paddr dst, natl i, natl n, tab_entry e);

/*! @brief Iteratore per la visita di un TRIE.
 */
class tab_iter {
	// ogni riga dello stack, partendo dalla 0, traccia la posizione
	// corrente all'interno dell'albero di traduzione.  La riga 0 traccia
	// la posizione nell'albero al livello massimo, la riga 1 nel livello
	// subito inferiore, e così via. Il livello corrente è contenuto
	// nella variabile 'l'.
	//
	// Ogni riga dello stack contiene l'indirizzo fisico (tab) della
	// tabella, del livello corrispondente, attualmente in corso di visita.
	// La riga contiene anche un intervallo [cur, end) di indirizzi ancora
	// da visitare in quella tabella. I campi cur e end della riga MAX_LIV,
	// altrimenti inutilizzati, vengono usati per contenere gli estremi
	// dell'intervallo completo passato al costruttore dell'iteratore.
	//
	// La terminazione della visita si riconosce da p->tab == 0.

	struct stack {
		vaddr cur, end;
		paddr tab;
	} s[MAX_LIV + 1];

	// livello corrente
	int l;

	// puntatore alla riga corrente dello stack
	stack *sp() { return &s[l - 1]; }
	stack const *sp() const { return &s[l - 1]; }

	// puntatore alla riga dello stack relativa al livello 'lvl'
	stack *sp(int lvl) { return &s[lvl - 1]; }

	// puntatore alla riga dello stack contenente gli estremi
	// dell'intervallo completo
	stack *pp() { return &s[MAX_LIV]; }
	stack const *pp() const { return &s[MAX_LIV]; }

	// true sse la visita è finita
	bool done() const { return !sp()->tab; }

public:
	/*! @brief controlla che un intervallo sia valido.
	 *
	 * Un intervallo valido se è interamente contenuto nella parte bassa o
	 * alta dello spazio di indirizzamento, oppure se è vuoto.
	 *
	 * @param beg	base dell'intervallo
	 * @param dim	dimensione in byte dell'intervallo (>= 0)
	 * @return	true se l'intervallo è valido, false altrimenti
	 */
	static bool valid_interval(vaddr beg, natq dim)
	{
		// ultimo indirizzo interno all'intervallo (se dim > 0)
		vaddr end = beg + dim - 1;
		return !dim || (
			// non inizia nel buco
			     norm(beg) == beg
			// non termina nel buco
			&&   norm(end) == end
			// estremi entrambi dalla stessa parte rispetto al buco
			&&   (beg & VADDR_MSBIT) == (end & VADDR_MSBIT)
			// non fa wrap-around
			&&   end >= beg);
	}

	/*! @brief inzializza un tab_iter per una visita in ordine anticipato.
	 *
	 * L'interatore si fermerà su tutte le entrate, di tutti i livelli,
	 * coninvolte nella traduzione degli indirizzi in [_beg_, _beg_ + _dim_).
	 *
	 * @param tab	indirizzo fisico della tabella radice del TRIE
	 * @param beg	base dell'intervallo
	 * @param dim	dimensione in byte dell'intervallo
	 */
	tab_iter(paddr tab, vaddr beg, natq dim = 1);

	/*! @brief visita terminata?
	 *
	 * @return true se la visita è terminata, false altrimenti
	 */
	operator bool() const {
		return !done();
	}

	/*! @brief livello corrente
	 *  @return livello su cui si trova l'iteratore
	 */
	int get_l() const {
		return l;
	}

	/*! @brief fermo su una foglia?
	 *  @return true se l'iteratore si trova su una foglia,
	 * 	    false altrimenti
	 */
	bool is_leaf() const {
		tab_entry e = get_e();
		return !(e & BIT_P) || (e & BIT_PS) || l == 1;
	}

	/*! @brief indirizzo virtuale corrente
	 *  @return il più piccolo indirizzo virtuale appartenente
	 *          all'intervallo e la cui traduzione passa dal
	 *          descrittore corrente
	 */
	vaddr get_v() const {
		return max(pp()->cur, sp()->cur);
	}

	/*! @brief descrittore corrente
	 *  @return riferimento al descrittore (di tabella o di pagina) corrente
	 */
	tab_entry& get_e() const {
		return get_entry(sp()->tab, i_tab(sp()->cur, l));
	}

	/*! @brief tabella corrente
	 *  @return indirizzo fisico della tabella che contiene il
	 *          descrittore corrente
	 */
	paddr get_tab() const {
		return sp()->tab;
	}

	/*! @brief prova a spostare l'iteratore di una posizione in basso nell'albero,
	 *         se possibile, altrimenti non fa niente.
	 *
	 * @return true se lo spostamento è avvenuto, false altrimenti
	 */
	bool down();
	/*! @brief prova a spostare l'iteratore di una posizione in alto nell'albero,
	 *         se possibile, altrimenti non fa niente.
	 *
	 * @return true se lo spostamento è avvenuto, false altrimenti
	 */
	bool up();
	/*! @brief prova a spostare l'iteratore di una posizione a destra nell'albero
	 *         (rimanendo nel livello corrente), se possibile, altrimenti non fa
	 *         niente.
	 * @return true se lo spostamento è avvenuto, false altrimenti
	 */
	bool right();

	/*! @brief porta l'iteratore alla prossima posizione della visita in ordine
	 *         anticipato
	 */
	void next();

	/*! @brief inizia una visita in ordine posticipato
	 */
	void post();

	/*! @brief porta l'iteratore alla prossima posizione della visita in ordine
	 *        posticipato
	 */
	void next_post();
};

/*! @brief Traduzione di un indirizzo virtuale in fisico.
 *
 * La funzione percorre un TRIE in modo analogo a quanto la MMU
 * fa in hardware, e restituisce la traduzione di un dato
 * indirizzo virtuale (o zero se la traduzione è assente).
 *
 * @param root_tab	indirizzo fisico della tabella radice del TRIE
 * @param v		indirizzo virtuale da tradurre
 * @return		indirizzo fisico corrispondente, o zero se _v_
 * 			non è mappato
 */
paddr trasforma(paddr root_tab, vaddr v);

/*! @brief Carica un nuovo valore in cr3.
 *
 * @param dir	indirizzo fisico della tabella radice del TRIE
 */
extern "C" void loadCR3(paddr dir);

/*! @brief Lettura di cr3.
 *
 * @return contenuto di cr3
 */
extern "C" paddr readCR3();

/*! @brief Invalida tutto il TLB.
 */
extern "C" void invalida_TLB();

/*! @brief Invalida una entrata del TLB.
 *
 * @param v	indirizzo virtuale di cui invalidare la traduzione
 */
extern "C" void invalida_entrata_TLB(vaddr v);

/**
 * @brief Lettura di cr2.
 *
 * @return contenuto di cr2
 */
extern "C" vaddr readCR2();

/// @name Funzioni usate da map() e unmap().
///
/// Queste funzioni devono essere definite per poter usare, in particolare,
/// @ref map() e @ref unmap().  La libce fornisce una versione semplificata che
/// può essere usata dai programmi 'bare'.  La versione semplificata alloca le
/// tabelle sullo heap e non le rilascia mai. Dal momento che non le rilascia
/// mai, non gestisce nemmeno un contatore delle entrate valide delle tabelle.
/// Il nucleo fornisce le proprie versioni di queste funzioni, che gestiscono
/// correttamente i contatori delle entrate valide e permettono di deallocare
/// le tabelle.
/// @{

/// invocata quando serve una nuova tabella
extern paddr alloca_tab();
/// invocata quando una tabella non serve più
extern void rilascia_tab(paddr);
/// invocata quando una tabella acquisisce una nuova entrata valida
extern void inc_ref(paddr);
/// invocata quando una tabella perde una entrata precedentemente valida
extern void dec_ref(paddr);
/// invocata per leggere il contatore delle entrate valide di una tabella
extern natl get_ref(paddr);
/// @}

/*! @brief Crea tutto il sottoalbero necessario a tradurre tutti gli
 *         indirizzi di un intervallo.
 *
 * L'intero intervallo non deve contenere traduzioni pre-esistenti.
 *
 * I bit RW e US che sono a 1 nel parametro _flags_ saranno settati anche in
 * tutti i descrittori rilevanti del sottoalbero. Se _flags_ contiene i bit PCD e/o PWT,
 * questi saranno settati solo sui descrittori foglia.
 *
 * Il parametro _getpaddr_ deve poter essere invocato come _getpaddr(v)_, dove _v_ è
 * un indirizzo virtuale. L'invocazione deve restituire l'indirizzo fisico che
 * si vuole far corrispondere a _v_, o zero in caso di errore.
 *
 * La funzione, per default, crea traduzioni con pagine di livello 1. Se
 * si vogliono usare pagine di livello superiore (da 2 a @ref MAX_PS_LVL) occorre
 * passare anche il parametro _ps_lvl_.
 *
 * @tparam T	tipo di _getpaddr_
 * @param tab	indirizzo fisico della radice del TRIE
 * @param begin	base dell'intervallo
 * @param end	limite dell'intervallo
 * @param flags	flag da settare
 * @param getpaddr funzione che deve restituire l'indirizzo fisico
 * 		   da associare ad ogni indirizzo virtuale.
 * @param ps_lvl livello delle traduzioni da creare
 * @return	il primo indirizzo non mappato (_end_ in caso di successo)
 */
template<typename T>
vaddr map(paddr tab, vaddr begin, vaddr end, natl flags, T& getpaddr, int ps_lvl = 1)
{
/*
 * Lo schema generale della funzione, escludendo i controlli sugli errori e
 * la gestione dei flag, è il seguente:
 *     new_f = 0;
 *     for (tutto il sotto-albero visitato in pre-ordine)
 *         tab_entry& e = riferimento all'entrata corrente
 *         vaddr v = indirizzo virtuale corrente
 *         if (livello non foglia)
 *             if (tabella assente)
 *                 new_f = alloca_tab();
 *         else
 *             new_f = getpaddr(v);
 *         if (new_f)
 *             inizializza 'e' in modo che punti a 'new_f'
 */
	vaddr v;	/* indirizzo virtuale corrente */
	int l;		/* livello (del TRIE) corrente */
	natq dr;	/* dimensione delle regioni di livello ps_lvl */
	const char* err_msg = "";
	static const natq allowed_flags = BIT_RW | BIT_US | BIT_PWT | BIT_PCD;

	// controlliamo che ps_lvl abbia uno dei valori consentiti
	if (ps_lvl < 1 || ps_lvl > MAX_PS_LVL) {
		fpanic("map: ps_lvl %d non ammesso (deve essere compreso tra 1 e %d)",
				ps_lvl, MAX_PS_LVL);
	}
	// il mapping coinvolge sempre almeno pagine, quindi consideriamo come
	// erronei dei parametri beg, end che non sono allineati alle pagine
	// (di livello ps_lvl)
	dr = dim_region(ps_lvl - 1);
	if (begin & (dr - 1)) {
		fpanic("map: begin=%llx non allineato alle pagine di livello %d",
				static_cast<unsigned long long>(begin), ps_lvl);
	}
	if (end & (dr - 1)) {
		fpanic("map: end=%llx non allineato alle pagine di livello %d",
				static_cast<unsigned long long>(end), ps_lvl);
	}
	// controlliamo che flags contenga solo i flag che possiamo gestire
	if (flags & ~allowed_flags) {
		fpanic("map: flags contiene bit non validi: %llx",
				static_cast<unsigned long long>(flags & ~allowed_flags));
	}

	// usiamo un tab_iter per percorrere tutto il sottoalbero.  Si noti che
	// il sottoalbero verrà costruito man mano che lo visitiamo.
	//
	// Si noti che tab_iter fa ulteriori controlli sulla validità
	// dell'intervallo (si veda tab_iter::valid_interval in libce)
	//
	// mentre percorriamo l'albero controlliamo che non esistano già
	// traduzioni per qualche indirizzo in [begin, end). In caso contrario
	// ci fermiamo restituendo errore.
	tab_iter it(tab, begin, end - begin);
	for ( /* niente */ ; it; it.next()) {
		tab_entry& e = it.get_e();
		l = it.get_l();
		v = it.get_v();
		// new_f diventerà diverso da 0 se dobbiamo settare a 1 il bit
		// P di 'e'
		paddr new_f = 0;

		if (l > ps_lvl) {
			// per tutti i livelli non "foglia" allochiamo la
			// tabella di livello inferiore e facciamo in modo che
			// il descrittore corrente la punti (Si noti che la
			// tabella potrebbe esistere già, e in quel caso non
			// facciamo niente)
			if (!(e & BIT_P)) {
				new_f = alloca_tab(); // [1]
				if (!new_f) {
					err_msg = "impossibile allocare tabella";
					goto error;
				}
			} else if (e & BIT_PS) {
				// se PS==1 questo descrittore non contiene un
				// puntatore alla prossima tabella, ma è già
				// una traduzione. La traduzione è di livello
				// più alto di quella richiesta e dunque è
				// sicuramente preesistente.
				err_msg = "gia' mappato";
				goto error;
			}
		} else {
			// arrivati ai descrittori di livello ps_lvl creiamo la
			// traduzione vera e propria, salvo errori.
			if (e & BIT_P) {
				err_msg = "gia' mappato";
				goto error;
			}
			// otteniamo il corrispondente indirizzo fisico
			// chiedendolo all'oggetto getpaddr
			new_f = getpaddr(v);	// [2]
			if (!new_f) {
				err_msg = "getpaddr() ha restituito 0";
				goto error;
			}

			// se la pagina è di livello maggiore di 1 dobbiamo
			// settare il bit PS
			if (l > 1)
				e |= BIT_PS;

			// settiamo anche i bit PWT e PCD, se richiesto
			e |= (flags & (BIT_PWT|BIT_PCD));
		}
		if (new_f) {
			// siamo qui perché c'è da allocare una tabella (punto
			// [1] qui sopra) o installare una traduzione (punto
			// [2]). Le cose da fare sono le stesse in entrambi i
			// casi:

			// 'e' non puntava a niente e ora deve puntare a new_f
			set_IND_FISICO(e, new_f);
			e |= BIT_P;

			// dobbiamo incrementare il contatore delle entrate
			// valide della tabella a cui 'e' appartiene
			inc_ref(it.get_tab());
		}

		// se sono stati richiesti i bit RW e/o US, questi vanno
		// settati su tutti i livelli (anche su eventuali entrate di
		// livello > ps_lvl già esistenti), altrimenti non hanno
		// effetto.
		e |= (flags & (BIT_RW|BIT_US));
	}
	return end;

error:
	flog(LOG_WARN, "map: indirizzo %llx, livello %d: %s", static_cast<unsigned long long>(v), l, err_msg);
	// it punta all'ultima entrata visitata. Se si tratta di una tabella
	// dobbiamo provare a deallocare tutto il percorso che abbiamo creato
	// fino a quel punto. Una chiamata ad unmap() non è sufficiente, in
	// quanto queste tabelle potrebbero non contenere traduzioni complete.
	paddr p = it.get_tab();
	for (;;) {
		if (get_ref(p))
			break;
		rilascia_tab(p);
		if (!it.up())
			break;
		it.get_e() = 0;
		p = it.get_tab();
		dec_ref(p);
	}
	return max(v, begin);
}

/// @brief Overloading di @ref map() per il caso il cui _getpaddr_ non sia un lvalue.
template<typename T>
vaddr map(paddr tab, vaddr begin, vaddr end, natl flags, const T& getpaddr, int ps_lvl = 1)
{
	T tmp = getpaddr;
	return map(tab, begin, end, flags, tmp, ps_lvl);
}

/*! @brief Elimina tutte le traduzioni di un intervallo.
 *
 * Rilascia automaticamente tutte le sottotabelle che diventano vuote dopo aver
 * eliminato le traduzioni. Per liberare le pagine vere e proprie, invece,
 * chiama la funzione _putpaddr()_ passandole l'indirizzo virtuale, l'indirizzo
 * fisico e il livello della pagina da eliminare.
 *
 * @tparam T	tipo di _putpaddr_
 * @param tab	indirizzo fisico della radice del TRIE
 * @param begin	base dell'intervallo
 * @param end	limite dell'intervallo
 * @param putpaddr funzione che verrà invocata per ogni traduzione eliminata
 */
template<typename T>
void unmap(paddr tab, vaddr begin, vaddr end, T& putpaddr)
{
	tab_iter it(tab, begin, end - begin);
	// eseguiamo una visita in ordine posticipato
	for (it.post(); it; it.next_post()) {
		tab_entry& e = it.get_e();

		if (!(e & BIT_P))
			continue;

		paddr p = extr_IND_FISICO(e);
		if (!it.is_leaf()) {
			// l'entrata punta a una tabella.
			if (!get_ref(p)) {
				// Se la tabella non contiene più entrate
				// valide la deallochiamo
				rilascia_tab(p);
			} else {
				// altrimenti non facciamo niente
				// (la tabella serve per traduzioni esterne
				// all'intervallo da eliminare)
				continue;
			}
		} else {
			// l'entrata punta ad una pagina (di livello it.get_l())
			vaddr v = it.get_v();
			int l = it.get_l();
			// Tutta la pagina deve essere inclusa nell'intervallo da eliminare
			if (v < begin || v + dim_region(l - 1) - 1 > end - 1) {
				fpanic("unmap: tentativo di riumuovere traduzione esterna a [%lx, %lx)",
						begin, end);
			}
			// lasciamo al chiamante decidere cosa fare
			putpaddr(v, p, l);
		}

		// per tutte le pagine, e per le tabelle che abbiamo
		// deallocato, azzeriamo l'entrata 'e' e decrementiamo il
		// contatore delle entrate valide nella tabella che la contiene
		e = 0;
		dec_ref(it.get_tab());
	}
}

/// @brief Overloading di @ref unmap() per il caso in cui _putpaddr_ non sia un lvalue.
template<typename T>
void unmap(paddr tab, vaddr begin, vaddr end, const T& putpaddr)
{
	// adattatore nel caso in cui putpaddr non sia un lvalue
	T tmp = putpaddr;
	unmap(tab, begin, end, tmp);
}
/// @}
