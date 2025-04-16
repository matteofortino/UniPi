.global _main


.data
msg: .asciz "ciao\r"

.text
_main: 
    nop 
    lea msg, %ebx
test:
    call outline
    call newline
    ret 

.include "./files/utility.s"