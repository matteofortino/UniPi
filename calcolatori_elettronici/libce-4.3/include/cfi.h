/// @cond INTERNAL
enum CFI {
	RAX = 0,
	RDX,
	RCX,
	RBX,
	RSI,
	RDI,
	RBP,
	RSP,
	R8,
	R9,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15,
	RA,	// return address

	NREG
};

struct cfi_d {
	vaddr eh_frame;
	natq eh_frame_len;
	void *token;
	natq (*read_stack)(void* token, vaddr);
	natq cs;
	natq regs[CFI::NREG];
	natq flags;
	natq rip;
};

bool cfi_backstep(cfi_d& cfi, vaddr rip, bool& was_fault);
void cfi_backtrace(cfi_d& cfi, log_sev sev, const char *msg);
extern "C" void cfi_dump(cfi_d& cfi, log_sev sev);
/// @endcond
