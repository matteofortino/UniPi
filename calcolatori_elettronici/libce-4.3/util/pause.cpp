#include "../internal.h"

// attende che venga premuto il tasto ESC
void pause()
{
	vid::str_write("Premere ESC per proseguire\n");
	while (kbd::char_read() != 0x1B)
		;
}
