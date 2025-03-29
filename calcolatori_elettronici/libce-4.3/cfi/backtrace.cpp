#include "../internal.h"
#include "../include/boot.h"
#include "cfi_internal.h"

void cfi_backtrace(cfi_d& cfi, log_sev sev, const char *msg)
{
	vaddr rip = cfi.rip;
	bool was_fault;
	do {
		for (natq i = 0; i < boot_info->nummod; i++) {
			boot64_modinfo *m = &boot_info->mod[i];
			for (natq j = 0; j < m->numseg; j++) {
				boot64_segment *s = &m->segments[j];
				if (rip >= s->vaddr && rip < s->vaddr + s->memsize) {
					cfi.eh_frame = m->eh_frame_base;
					cfi.eh_frame_len = m->eh_frame_size;
					goto found;
				}
			}
		}
		break;

	found:
		if (!cfi_backstep(cfi, rip, was_fault))
			break;

		rip = cfi.regs[CFI::RA];

		if (!rip)
			break;

		if (!was_fault)
			rip--;

		flog(sev, "%s0x%llx", msg, static_cast<long long unsigned>(rip));
	} while (true);
}
