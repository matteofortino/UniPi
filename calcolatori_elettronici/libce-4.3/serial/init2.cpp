#include "../internal.h"

namespace serial {

	void init2()
	{
		natw CBITR = 0x000C;			// 9600 bit/sec.
		outputb(0x80, iCOM2_LCR);		// DLAB 1
		outputb(CBITR, iCOM2_DLR_LSB);
		outputb(CBITR >> 8, iCOM2_DLR_MSB);
		outputb(0x03, iCOM2_LCR);		// 1 bit STOP, 8 bit/car, parità dis, DLAB 0
		outputb(0x00, iCOM2_IER);		// richieste di interruzione disabilitate
		inputb(iCOM2_RBR);			// svuotamento buffer RBR
	}

}
