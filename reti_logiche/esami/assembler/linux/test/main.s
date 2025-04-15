.global _main
.include "./files/utility.s"

.data
msg: .asciz "flavio gay\r"

.text
_main: 
	lea msg, %ebx
	call outline
	call newline
	ret

