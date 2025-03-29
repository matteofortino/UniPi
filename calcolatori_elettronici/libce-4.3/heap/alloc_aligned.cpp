#include "../internal.h"

using namespace heap;

void *alloc_aligned(size_t dim, std::align_val_t a)
{
	size_t alignment = static_cast<size_t>(a);

	// alignment deve essere una potenza di due
	if (!alignment || (alignment & (alignment - 1)))
		return nullptr;

	// allineamo sia alignment che dim alla dimensione di des_mem.
	// In questo modo tutte le operazioni saranno in multipli
	// di sizeof(des_mem).
	alignment = allinea(alignment, sizeof(des_mem));
	dim = allinea(dim, sizeof(des_mem));

	// L'idea è di allocare alignment - sizeof(des_mem) byte in più di
	// quelli richiesti, in modo da essere sicuri di poter spostare il
	// puntatore ricevuto in avanti fino a trovare un multiplo di alignment
	size_t newdim = dim + alignment - sizeof(des_mem);
	if (newdim < dim) // overflow
		return nullptr;

	void *p = alloc(newdim);
	if (!p)
		return nullptr;

	// calcoliamo il primo indirizzo successivo a p e allineato ad
	// alignment
	uintptr_t m1 = int_cast<uintptr_t>(p);
	uintptr_t m2 = allinea(m1, alignment);
	// recuperiamo il descrittore del chunk allocato
	des_mem *d = static_cast<des_mem*>(p) - 1;
	if (m2 != m1) {
		// il p ricevuto da alloc() non è allineato ad alignemnt.
		// Spostiamo p fino ad m2, liberando la memoria tra m2 e m1.
		size_t allocdim = d->dimensione,
		       skipdim = m2 - m1 - sizeof(des_mem);
		d->dimensione = skipdim;
		dealloc(p);
		p = voidptr_cast(m2);
		// creiamo un nuovo descrittore
		d = static_cast<des_mem*>(p) - 1;
		d->dimensione = allocdim - skipdim - sizeof(des_mem);
		d->next = 0xdeadbeefL;
	}
	if (d->dimensione > dim && free_interna(voidptr_cast(m2 + dim), d->dimensione - dim)) {
			d->dimensione = dim;
	}
	return p;
}
