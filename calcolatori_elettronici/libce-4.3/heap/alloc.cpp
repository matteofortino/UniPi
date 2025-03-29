#include "../internal.h"

using namespace heap;
// funzione di allocazione: cerca la prima zona di memoria libera di dimensione
// almeno pari a "quanti" byte e ne restituisce l'indirizzo di partenza.
// Se la zona trovata è sufficientemente più grande di "quanti" byte divide la zona
// in due: una grande "quanti" byte, che viene occupata, e una contenente i rimanenti byte
// a formare una nuova zona libera, con un nuovo descrittore.
void* alloc(size_t dim)
{
	// rispettiamo l'allineamento minimo richiesto dallo standard
	size_t quanti = allinea(dim, max(sizeof(des_mem),
				static_cast<size_t>(__STDCPP_DEFAULT_NEW_ALIGNMENT__)));
	// allochiamo "quanti" byte invece dei "dim" richiesti

	// per prima cosa cerchiamo una zona di dimensione sufficiente
	des_mem *prec = nullptr, *scorri = memlibera;
	while (scorri && scorri->dimensione < quanti) {
		prec = scorri;
		scorri = natq2desmem(scorri->next);
	}
	// assert(scorri == 0 || scorri->dimensione >= quanti);

	void* p = nullptr;
	if (scorri) {
		p = scorri + 1; // puntatore al primo byte dopo il descrittore
				// (coincide con l'indirizzo iniziale delle zona
				// di memoria)

		// per poter dividere in due la zona trovata, la parte
		// rimanente, dopo aver occupato "quanti" byte, deve poter contenere
		// almeno il descrittore più sizeof(des_mem) byte (minima dimensione
		// allocabile)
		if (scorri->dimensione - quanti >= 2 * sizeof(des_mem)) {

			// il nuovo descrittore verrà scritto nei primi byte
			// della zona da creare (quindi, "quanti" byte dopo "p")
			void* pnuovo = static_cast<natb*>(p) + quanti;
			des_mem* nuovo = static_cast<des_mem*>(pnuovo);

			// aggiustiamo le dimensioni della vecchia e della nuova zona
			nuovo->dimensione = scorri->dimensione - quanti - sizeof(des_mem);
			scorri->dimensione = quanti;

			// infine, inseriamo "nuovo" nella lista delle zone libere,
			// al posto precedentemente occupato da "scorri"
			nuovo->next = scorri->next;
			if (prec != nullptr)
				prec->next = desmem2natq(nuovo);
			else
				memlibera = nuovo;

		} else {

			// se la zona non è sufficientemente grande per essere
			// divisa in due la occupiamo tutta, rimuovendola
			// dalla lista delle zone libere
			if (prec != nullptr)
				prec->next = scorri->next;
			else
				memlibera = natq2desmem(scorri->next);
		}

		// a scopo di debug, inseriamo un valore particolare nel campo
		// next (altrimenti inutilizzato) del descrittore. Se tutto
		// funziona correttamente ci aspettiamo di ritrovare lo stesso
		// valore quando quando la zona verrà successivamente
		// deallocata. (Notare che il valore non è allineato a
		// sizeof(long) byte, quindi un valido indirizzo di descrittore
		// non può assumere per caso questo valore).
		scorri->next = 0xdeadbeefL;

	}

	// restituiamo l'indirizzo della zona allocata (nullptr se non è stato
	// possibile allocare).  NOTA: il descrittore della zona si trova nei
	// byte immediatamente precedenti l'indirizzo "p".  Questo è
	// importante, perché ci permette di recuperare il descrittore dato
	// "p" e, tramite il descrittore, la dimensione della zona occupata
	// (tale dimensione può essere diversa da quella richiesta, quindi
	// è ignota al chiamante della funzione).
	if (p != nullptr)
		memset(p, 0, dim);
	return p;
}
