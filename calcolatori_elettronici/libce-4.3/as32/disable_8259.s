.set ICW1M, 0x20
.set ICW2M, 0x21
.set ICW3M, 0x21
.set ICW4M, 0x21
.set OCW1M, 0x21
.set OCW3M, 0x20
.set ICW1S, 0xa0
.set ICW2S, 0xa1
.set ICW3S, 0xa1
.set ICW4S, 0xa1
.set OCW1S, 0xa1
.set OCW3S, 0xa0
.set OCW2M, 0x20
.set OCW3M, 0x20
.set OCW2S, 0xa0
.set OCW3S, 0xa0

	.global disable_8259
disable_8259:
	push %ebp
	mov %esp, %ebp
	push %eax

	// master
	movb $0x11, %al		// cascata
	outb %al, $ICW1M
	movb $0x20, %al		// nuova base
	outb %al, $ICW2M
	movb $0x04, %al		// slave connesso a IR2
	outb %al, $ICW3M
	movb $0x01, %al		// modo annidato
	outb %al, $ICW4M
	movb $0b11111011, %al	// maschera tutte le interruzioni, tranne quelle
	outb %al, $OCW1M	//  provenienti dallo slave
	movb $0x48, %al
	outb %al, $OCW3M	// fully nested

	// slave
	movb $0x11, %al		// cascata
	outb %al, $ICW1S
	movb $0x28, %al		// nuova base
	outb %al, $ICW2S
	movb $0x02, %al		// id. dello slave
	outb %al, $ICW3S
	movb $0x01, %al		// modo annidato
	outb %al, $ICW4S
	movb $0b11111111, %al	// maschera tutte le interruzioni
	outb %al, $OCW1S
	movb $0x48, %al		// fully nested
	outb %al, $OCW3S

	pop %eax
	leave
	ret
