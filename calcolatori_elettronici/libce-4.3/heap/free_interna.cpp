#include "../internal.h"

namespace heap {
// rende libera la zona di memoria puntata da "indirizzo" e grande "quanti"
// byte, preoccupandosi di creare il descrittore della zona e, se possibile, di
// unificare la zona con eventuali zone libere contigue in memoria.  La
// riunificazione è importante per evitare il problema della "falsa
// frammentazione", in cui si vengono a creare tante zone libere, contigue, ma
// non utilizzabili per allocazioni più grandi della dimensione di ciascuna di
// esse.
// "free_interna" può essere usata anche in fase di inizializzazione, per
// definire le zone di memoria fisica che verranno utilizzate dall'allocatore
bool free_interna(void* indirizzo, size_t quanti)
{

	// non è un errore invocare free_interna con "quanti" uguale a 0
	if (quanti == 0)
		return true;

	// il caso "indirizzo == 0", invece, va escluso, in quanto l'indirizzo
	// 0 viene usato per codificare il puntatore nullo
	if (!indirizzo)
		panic("free_interna(0)");

	uintptr_t x = int_cast<uintptr_t>(indirizzo);
	if (allinea(x, __STDCPP_DEFAULT_NEW_ALIGNMENT__) != x)
		fpanic("free_interna(%p): indirizzo non allineato", indirizzo);

	// la zona va inserita nella lista delle zone libere, ordinata per
	// indirizzo di partenza (tale ordinamento serve a semplificare la
	// successiva operazione di riunificazione)
	des_mem *prec = nullptr, *scorri = memlibera;
	while (scorri && scorri < indirizzo) {
		prec = scorri;
		scorri = natq2desmem(scorri->next);
	}
	// assert(scorri == 0 || scorri >= indirizzo)

	// in alcuni casi siamo in grado di riconoscere l'errore di doppia
	// free: "indirizzo" non deve essere l'indirizzo di partenza di una
	// zona già libera
	if (scorri == indirizzo) {
		fpanic("double free all'indirizzo %p", indirizzo);
	}
	// assert(scorri == 0 || scorri > indirizzo)

	// verifichiamo che la zona possa essere unificata alla zona che la
	// precede.  Ciò è possibile se tale zona esiste e il suo ultimo byte
	// è contiguo al primo byte della nuova zona
	if (prec && reinterpret_cast<natb*>(prec + 1) + prec->dimensione == indirizzo) {

		// controlliamo se la zona è unificabile anche alla eventuale
		// zona che la segue
		if (scorri && static_cast<natb*>(indirizzo) + quanti == reinterpret_cast<natb*>(scorri)) {

			// in questo caso le tre zone diventano una sola, di
			// dimensione pari alla somma delle tre, più il
			// descrittore della zona puntata da scorri (che ormai
			// non serve più)
			prec->dimensione += quanti + sizeof(des_mem) + scorri->dimensione;
			prec->next = scorri->next;

		} else {

			// unificazione con la zona precedente: non creiamo una
			// nuova zona, ma ci limitiamo ad aumentare la
			// dimensione di quella precedente
			prec->dimensione += quanti;
		}

		return true;
	}
	if (scorri && static_cast<natb*>(indirizzo) + quanti == reinterpret_cast<natb*>(scorri)) {

		// la zona non è unificabile con quella che la precede, ma è
		// unificabile con quella che la segue. L'unificazione deve
		// essere eseguita con cautela, per evitare di sovrascrivere il
		// descrittore della zona che segue prima di averne letto il
		// contenuto

		// salviamo il descrittore della zona seguente in un posto
		// sicuro
		des_mem salva = *scorri;

		// allochiamo il nuovo descrittore all'inizio della nuova zona
		// (se quanti < sizeof(des_mem), tale descrittore va a
		// sovrapporsi parzialmente al descrittore puntato da scorri)
		des_mem* nuovo = static_cast<des_mem*>(indirizzo);

		// quindi copiamo il descrittore della zona seguente nel nuovo
		// descrittore. Il campo next del nuovo descrittore è
		// automaticamente corretto, mentre il campo dimensione va
		// aumentato di "quanti"
		*nuovo = salva;
		nuovo->dimensione += quanti;

		// infine, inseriamo "nuovo" in lista al posto di "scorri"
		if (prec)
			prec->next = desmem2natq(nuovo);
		else
			memlibera = nuovo;

		return true;
	}
	if (quanti >= sizeof(des_mem)) {

		// la zona non può essere unificata con nessun'altra.  Non
		// possiamo, però, inserirla nella lista se la sua dimensione
		// non è tale da contenere il descrittore (nel qual caso, la
		// zona viene ignorata)

		des_mem* nuovo = static_cast<des_mem*>(indirizzo);
		nuovo->dimensione = quanti - sizeof(des_mem);

		// inseriamo "nuovo" in lista, tra "prec" e "scorri"
		nuovo->next = desmem2natq(scorri);
		if (prec)
			prec->next = desmem2natq(nuovo);
		else
			memlibera = nuovo;

		return true;
	}
	return false;
}
}
