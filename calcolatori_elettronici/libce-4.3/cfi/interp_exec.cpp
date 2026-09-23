#include "../internal.h"
#include "cfi_internal.h"

#define DW_CFA_himask		(0x3 << 6)

#define DW_CFA_advance_loc	(0x1 << 6)
#define DW_CFA_offset		(0x2 << 6)
#define DW_CFA_restore		(0x3 << 6)

#define DW_CFA_nop			0
#define DW_CFA_set_loc			0x01
#define DW_CFA_advance_loc1		0x02
#define DW_CFA_advance_loc2		0x03
#define DW_CFA_advance_loc4		0x04
#define DW_CFA_offset_extended		0x05
#define DW_CFA_restore_extended		0x06
#define DW_CFA_undefined		0x07
#define DW_CFA_same_value		0x08
#define DW_CFA_register			0x09
#define DW_CFA_remember_state		0x0a
#define DW_CFA_restore_state		0x0b
#define DW_CFA_def_cfa			0x0c
#define DW_CFA_def_cfa_register		0x0d
#define DW_CFA_def_cfa_offset		0x0e
#define DW_CFA_def_cfa_expression	0x0f
#define DW_CFA_expression		0x10
#define DW_CFA_offset_extended_sf	0x11
#define DW_CFA_def_cfa_sf		0x12
#define DW_CFA_def_cfa_offset_sf	0x13
#define DW_CFA_val_offset		0x14
#define DW_CFA_val_offset_sf		0x15
#define DW_CFA_val_expression		0x16
#define DW_CFA_lo_user			0x1c
#define DW_CFA_hi_user			0x3f

void cfi_interp::exec()
{
	natq reg, reg2;
	natb boff;
	natw woff;
	natl loff;
	natq qoff;
	int  ioff;
	long soff;
	natb op;
	vaddr scan = next_op;

	memcpy(&op, reinterpret_cast<void*>(scan), 1);
	scan++;

	if (op & DW_CFA_himask) {
		natl v = op & ~DW_CFA_himask;
		switch (op & DW_CFA_himask) {
		case DW_CFA_advance_loc:
			cur_loc += v;
			break;
		case DW_CFA_offset:
			regs.r[v].mode = cfi_mode::OFFSET;
			qoff = read_uleb128(scan);
			regs.r[v].offset = daf * (long)qoff;
			break;
		case DW_CFA_restore:
			regs.r[v] = cie_regs.r[v];
			break;
		}
	} else {
		switch (op) {
		case DW_CFA_nop:
			is_fault = (regs.cfa_offset == 40 || regs.cfa_offset == 48);
			break;
		case DW_CFA_set_loc:
			memcpy(&ioff, reinterpret_cast<void*>(scan), sizeof(int));
			cur_loc = scan + ioff;
			scan += 4;
			break;
		case DW_CFA_advance_loc1:
			memcpy(&boff, reinterpret_cast<void*>(scan), 1);
			cur_loc += boff;
			scan += 1;
			break;
		case DW_CFA_advance_loc2:
			memcpy(&woff, reinterpret_cast<void*>(scan), 2);
			cur_loc += woff;
			scan += 2;
			break;
		case DW_CFA_advance_loc4:
			memcpy(&loff, reinterpret_cast<void*>(scan), 4);
			cur_loc += loff;
			scan += 4;
			break;
		case DW_CFA_offset_extended:
			reg = read_uleb128(scan);
			qoff = read_uleb128(scan);
			regs.r[reg].mode = cfi_mode::OFFSET;
			regs.r[reg].offset = daf * qoff;
			break;
		case DW_CFA_restore_extended:
			reg = read_uleb128(scan);
			regs.r[reg] = cie_regs.r[reg];
			break;
		case DW_CFA_undefined:
			reg = read_uleb128(scan);
			regs.r[reg].mode = cfi_mode::UNDEFINED;
			break;
		case DW_CFA_same_value:
			reg = read_uleb128(scan);
			regs.r[reg].mode = cfi_mode::SAME_VALUE;
			break;
		case DW_CFA_register:
			reg = read_uleb128(scan);
			reg2 = read_uleb128(scan);
			regs.r[reg].mode = cfi_mode::REGISTER;
			regs.r[reg].regn = reg2;
			break;
		case DW_CFA_remember_state:
			if (stack.top >= CFI_STACK_SIZE) {
				flog(LOG_DEBUG, "CFI: stack overflow");
				break;
			}
			stack.s[stack.top++] = regs;
			break;
		case DW_CFA_restore_state:
			regs = stack.s[--stack.top];
			break;
		case DW_CFA_def_cfa:
			regs.cfa_register = read_uleb128(scan);
			regs.cfa_offset   = read_uleb128(scan);
			break;
		case DW_CFA_def_cfa_register:
			regs.cfa_register = read_uleb128(scan);
			break;
		case DW_CFA_def_cfa_offset:
			regs.cfa_offset = read_uleb128(scan);
			break;
		case DW_CFA_def_cfa_expression:
			break;
		case DW_CFA_expression:
			break;
		case DW_CFA_offset_extended_sf:
			reg = read_uleb128(scan);
			soff = read_sleb128(scan);
			regs.r[reg].mode = cfi_mode::OFFSET;
			regs.r[reg].offset = daf * soff;
			break;
		case DW_CFA_def_cfa_sf:
			regs.cfa_register = read_uleb128(scan);
			regs.cfa_offset = daf * read_sleb128(scan);
			break;
		case DW_CFA_def_cfa_offset_sf:
			regs.cfa_offset = daf * read_sleb128(scan);
			break;
		case DW_CFA_val_offset:
			reg = read_uleb128(scan);
			qoff = read_uleb128(scan);
			regs.r[reg].mode = cfi_mode::VAL_OFFSET;
			regs.r[reg].offset = daf * (long)qoff;
			break;
		case DW_CFA_val_offset_sf:
			reg = read_uleb128(scan);
			soff = read_sleb128(scan);
			regs.r[reg].mode = cfi_mode::VAL_OFFSET;
			regs.r[reg].offset = daf * soff;
			break;
		case DW_CFA_val_expression:
			// TODO
			break;
		default:
			break;
		}
	}
	next_op = scan;
}
