#include "../internal.h"

namespace vid {

	void clear(natb a)
	{
		attr = static_cast<natw>(a) << 8;
		clear();
	}

}
