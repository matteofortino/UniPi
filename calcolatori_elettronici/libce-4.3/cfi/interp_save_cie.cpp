#include "../internal.h"
#include "cfi_internal.h"

void cfi_interp::save_cie()
{
	cie_regs = regs;
}
