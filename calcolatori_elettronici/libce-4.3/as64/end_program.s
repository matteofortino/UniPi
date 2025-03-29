.global end_program
end_program:
	.cfi_startproc
	cli
	call reboot
	hlt
	.cfi_endproc
