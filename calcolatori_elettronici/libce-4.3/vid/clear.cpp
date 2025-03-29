#include "../internal.h"

namespace vid {

	void clear()
	{
		for (unsigned int i = 0; i < VIDEO_SIZE; i++)
			video[i] = ' ' | attr;
		cursor();
	}

}
