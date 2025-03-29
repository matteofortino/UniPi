/// @cond INTERNAL
#include "../include/cfi.h"
enum cfi_mode {
	UNDEFINED,
	SAME_VALUE,
	OFFSET,
	VAL_OFFSET,
	REGISTER,
	EXPRESSION,
	VAL_EXPRESSION
};

struct cfi_reg_info {
	cfi_mode mode;
	long offset;
	int regn;
};

struct cfi_regs {
	long cfa_offset;
	int cfa_register;
	cfi_reg_info r[CFI::NREG];
};

struct cfi_stack {
#define CFI_STACK_SIZE 2
	cfi_regs s[CFI_STACK_SIZE];
	int top;
};

struct cfi_interp {
	cfi_d *cfi;
	cfi_regs regs;
	cfi_regs cie_regs;
	cfi_stack stack;
	bool is_fault;
	vaddr cfa;
	natq caf;
	long daf;
	int rar;

	vaddr next_op;
	vaddr cur_loc;

	void init(cfi_d*, vaddr);
	void save_cie();
	void jump(vaddr);
	void exec();
	void snapshot();
};

natq read_uleb128(vaddr& scan);
long read_sleb128(vaddr& scan);
/// @endcond
