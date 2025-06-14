#include "../internal.h"

bool tab_iter::down()
{
	if (done() || is_leaf()) {
		return false;
	}

	// leggiamo il descrittore attualmente puntato dall'iteratore
	tab_entry e = get_e();

	// calcoliamo gli estremi [a, b) dell'intervallo per il livello inferiore
	// Nel caso di 'b', le varie sottrazioni e somme di 1 servono a gestire
	// correttamente il caso in cui il limite sia zero (intervallo che si
	// trova in fondo allo spazio di indirizzamento).
	vaddr a = max(pp()->cur, sp()->cur),
	      b = min(pp()->end - 1, sp()->cur + dim_region(l - 1) - 1) + 1;

	// scendiamo di livello
	l--;
	// la tabella da esaminare a questo livello è quella
	// puntata dalla tab_entry corrente
	sp()->tab = extr_IND_FISICO(e);
	// calcoliamo l'intervallo di entrate da esaminare
	// in questa tabella
	sp()->cur = base(a, l - 1);
	sp()->end = limit(b, l - 1);

	// ora l'iteratore punta all'entrata che traduce sp()->cur nella
	// tabella di livello inferiore
	return true;
}
