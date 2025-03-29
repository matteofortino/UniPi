PHDRS
{
	headers PT_PHDR PHDRS ;
	text PT_LOAD FILEHDR PHDRS ;
	rodata PT_LOAD ;
	data PT_LOAD ;
	eh_frame 0x6474e550 ; /* GNU_EH_FRAME */
}

SECTIONS
{
	. = LOAD_ADDRESS + SIZEOF_HEADERS;
	.text : { *(.text) } :text
	_etext = .;
	. = ALIGN(0x1000);
	.rodata : { *(.rodata) } :rodata
	.eh_frame : { *(.eh_frame) } :rodata :eh_frame
	. = ALIGN(0x1000);
	__init_array_start = .;
	.init_array : { *(.ctors) *(.init_array) } :data
	__init_array_end = .;
	__fini_array_start = .;
	.fini_array : { *(.dtors) *(.fini_array) } :data
	__fini_array_end = .;
	.data : { *(.data) } :data
	_edata = .;
	.bss : { *(.bss) } :data
	_end = .;
}

ENTRY(_start)
