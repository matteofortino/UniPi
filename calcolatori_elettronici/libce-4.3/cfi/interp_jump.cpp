#include "../internal.h"
#include "cfi_internal.h"

void cfi_interp::jump(vaddr op)
{
	next_op = op;
}
