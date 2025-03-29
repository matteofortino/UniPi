#include "../internal.h"

namespace vid {

	void char_put(char c, natw x, natw y) {
		if (x < COLS && y < ROWS)
			video[y * COLS + x] = attr | c;
	}

}
