#include "../internal.h"

namespace vid {

	volatile natw& pos(natw x, natw y)
	{
		return video[y * COLS + x];
	}

}
