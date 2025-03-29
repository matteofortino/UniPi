#include "../include/libce.h"
.bss
.align 16
.global stack, stack_end
stack:
	.space 16*KiB, 0
stack_end:
