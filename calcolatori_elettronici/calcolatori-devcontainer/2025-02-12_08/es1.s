.global _ZN2clC1E3st1

.set _this, -8
.set ss, -24
.set ar2, -32
.set i, -40

.set freccia_v2, 0
.set freccia_v1, 32
.set freccia_v3, 8

_ZN2clC1E3st1:
    pushq %rbp
    movq %rsp, %rbp
    subq $40, %rsp

    movq %rdi, _this(%rbp)
    movq %rsi, ss(%rbp)

    movl $0, i(%rbp)

condizione_for:
    movslq i(%rbp), %rax
    cmp $4, %rax
    jl corpo_for
    jmp fine_for
corpo_for:
    movl ss(%rbp, %rax, 4), %ecx
    movb %cl, _this+freccia_v2+freccia_v1(%rbp, %rax, 1)

    mov $0, %rcx
    movl ss(%rbp, %rax, 4), %ecx
    shll $1, %ecx
    movq %rcx, _this+freccia_v2(%rbx, %rax, 8)

    movb %cl, _this+freccia_v1+freccia_v2+freccia_v3(%rbx, %rax, 1)

    incq i(%rbp)
    jmp condizione_for
fine_for:

    leave
    ret

.global _ZN2clC1ER3st1Pi
_ZN2clC1ER3st1Pi:
    // pushq %rbp
    // movq %rsp, %rbp
    // sub $40, %rsp

    movq %rdi, _this(%rbp)
    // movq %rsi, %rsi
    movq %rdx, ar2(%rbp)


    movl $0, i(%rbp)
condizione_for1:
    movslq i(%rbp), %rax
    cmp $4, %rax
    jl corpo_for1
    jmp fine_for1
corpo_for1:
    movl (%rsi, %rax, 4), %ecx 
    movb %ch, _this+freccia_v2+freccia_v1(%rbx, %rax, 1)

    mov $0, %rcx
    movl (%rsi, %rax, 4), %ecx
    shll $2, %ecx
    movq %rcx, _this+freccia_v2(%rbx, %rax, 8)

    movq ar2(%rbp), %rdi
    movl (%rdi, %rax, 4), %ecx
    movb %ch, _this+freccia_v1+freccia_v2+freccia_v3(%rbp, %rax, 1)


    incq i(%rbp)
    jmp condizione_for1
fine_for1:

    leave
    ret

