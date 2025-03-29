#include "../internal.h"

namespace apic {

	volatile natl* pEOIR = reinterpret_cast<natl*>(0xFEE000B0);

}
