#include "../internal.h"
#include "cfi_internal.h"

void cfi_dump(cfi_d& cfi, log_sev sev)
{
	flog(sev, "  RIP=0x%llx CPL=%s",
			static_cast<unsigned long long>(cfi.rip),
			cfi.cs == SEL_CODICE_UTENTE ? "LIV_UTENTE" : "LIV_SISTEMA");
	natq rflags = cfi.flags;
	flog(sev, "  RFLAGS=%llx [%s %s %s %s %s %s %s %s %s %s, IOPL=%s]",
		static_cast<unsigned long long>(rflags),
		(rflags & 1U << 14) ? "NT" : "--",
		(rflags & 1U << 11) ? "OF" : "--",
		(rflags & 1U << 10) ? "DF" : "--",
		(rflags & 1U << 9)  ? "IF" : "--",
		(rflags & 1U << 8)  ? "TF" : "--",
		(rflags & 1U << 7)  ? "SF" : "--",
		(rflags & 1U << 6)  ? "ZF" : "--",
		(rflags & 1U << 4)  ? "AF" : "--",
		(rflags & 1U << 2)  ? "PF" : "--",
		(rflags & 1U << 0)  ? "CF" : "--",
		(rflags & 0x3000) == 0x3000 ? "UTENTE" : "SISTEMA");
	flog(sev, "  RAX=%16llx RBX=%16llx RCX=%16llx RDX=%16llx",
			static_cast<unsigned long long>(cfi.regs[CFI::RAX]),
			static_cast<unsigned long long>(cfi.regs[CFI::RBX]),
			static_cast<unsigned long long>(cfi.regs[CFI::RCX]),
			static_cast<unsigned long long>(cfi.regs[CFI::RDX]));
	flog(sev, "  RDI=%16llx RSI=%16llx RBP=%16llx RSP=%16llx",
			static_cast<unsigned long long>(cfi.regs[CFI::RDI]),
			static_cast<unsigned long long>(cfi.regs[CFI::RSI]),
			static_cast<unsigned long long>(cfi.regs[CFI::RBP]),
			static_cast<unsigned long long>(cfi.regs[CFI::RSP]));
	flog(sev, "  R8 =%16llx R9 =%16llx R10=%16llx R11=%16llx",
			static_cast<unsigned long long>(cfi.regs[CFI::R8]),
			static_cast<unsigned long long>(cfi.regs[CFI::R9]),
			static_cast<unsigned long long>(cfi.regs[CFI::R10]),
			static_cast<unsigned long long>(cfi.regs[CFI::R11]));
	flog(sev, "  R12=%16llx R13=%16llx R14=%16llx R15=%16llx",
			static_cast<unsigned long long>(cfi.regs[CFI::R12]),
			static_cast<unsigned long long>(cfi.regs[CFI::R13]),
			static_cast<unsigned long long>(cfi.regs[CFI::R14]),
			static_cast<unsigned long long>(cfi.regs[CFI::R15]));

	if (cfi.read_stack) {
		flog(sev, "  backtrace:");
		cfi_backtrace(cfi, sev, "  > ");
	}
}
