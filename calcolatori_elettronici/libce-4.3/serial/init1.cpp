#include "../internal.h"

namespace serial {

	void init1()
	{
		natw CBITR = 0x000C;			// 9600 bit/sec.
		outputb(0x80, iCOM1_LCR);		// DLAB 1
		outputb(CBITR, iCOM1_DLR_LSB);
		outputb(CBITR >> 8, iCOM1_DLR_MSB);
		outputb(0x03, iCOM1_LCR);		// 1 bit STOP, 8 bit/car, parità dis, DLAB 0
		outputb(0x00, iCOM1_IER);		// richieste di interruzione disabilitate
		inputb(iCOM1_RBR);			// svuotamento buffer RBR
	}

}
