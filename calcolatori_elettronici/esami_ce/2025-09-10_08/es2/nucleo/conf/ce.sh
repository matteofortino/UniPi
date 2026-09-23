QEMU_EXTRA_OPTIONS="-chardev pipe,id=ce1a,path=ce1a -device ce,id=ce1,async=ce1a -chardev pipe,id=ce2a,path=ce2a -device ce,id=ce2,async=ce2a"
QEMU_PRE_CMD="perl ce1.pl & perl ce2.pl &"
QEMU_FIFOS="ce1 ce1a ce2 ce2a"
