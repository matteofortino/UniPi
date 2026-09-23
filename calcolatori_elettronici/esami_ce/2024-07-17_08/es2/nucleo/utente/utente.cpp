#include <all.h>

testdes testdata;

void test0(natq)
{
	terminate_p();
}

extern "C" void main()
{
	testdata.sync = sem_ini(0);
	testdata.level = 0;
	passtd(&testdata);
	sem_wait(testdata.sync);
	meminfo before = getmeminfo(), after;
	passtd(&testdata);

	// test 0
	sem_wait(testdata.sync);
	activate_p(test0, 0, 50, LIV_UTENTE);
	
	// test 1
	sem_wait(testdata.sync);
	activate_p(test0, 0, 50, LIV_UTENTE);

	// test 2
	sem_wait(testdata.sync);
	activate_p(test0, 0, 50, LIV_UTENTE);
	activate_p(test0, 0, 50, LIV_UTENTE);

	// test 3
	sem_wait(testdata.sync);
	activate_p(test0, 0, 50, LIV_UTENTE);

	// test 4
	sem_wait(testdata.sync);

	// test 5
	sem_wait(testdata.sync);
	activate_p(test0, 0, 50, LIV_UTENTE);

	// test 6
	sem_wait(testdata.sync);
	activate_p(test0, 0, 50, LIV_UTENTE);
	sem_wait(testdata.sync);
	activate_p(test0, 0, 50, LIV_UTENTE);

	// test finale
	sem_wait(testdata.sync);
	after = getmeminfo();
	passtd(&testdata);
	if (before.heap_libero != after.heap_libero) {
		printf("non liberati %ld byte dello heap",
				(long)before.heap_libero - (long)after.heap_libero);
	}
	if (before.num_frame_liberi != after.num_frame_liberi) {
		printf("non liberati %ld frame",
				(long)before.num_frame_liberi - (long)after.num_frame_liberi);
	}
	pause();
	terminate_p();
}
