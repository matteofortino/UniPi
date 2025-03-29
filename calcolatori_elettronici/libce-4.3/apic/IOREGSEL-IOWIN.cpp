#include "../internal.h"

namespace apic {

	volatile natl* pIOREGSEL = reinterpret_cast<natl*>(0xFEC00000);
	volatile natl* pIOWIN = reinterpret_cast<natl*>(0xFEC00010);

}
