#include "../internal.h"

namespace vid {

	void str_write(const char buf[])
	{
		natl i = 0;
		while (buf[i]) {
			char_write(buf[i]);
			i++;
		}
	}

}
