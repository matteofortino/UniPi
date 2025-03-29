# directory di installazione
PREFIX ?= $(HOME)/CE
# dimensione dell'hard disk emulato
CE_HDSIZE=20M

# directory che conterrà libce.conf
CE_ETC=$(PREFIX)/etc
# directory che conterrà compile/boot/debug
CE_BIN=$(PREFIX)/bin
# directory per la libreria a 32 bit (boot loader)
CE_LIB32=$(PREFIX)/lib/ce
# directory che conterrà la libreria a 64 bit
CE_LIB64=$(PREFIX)/lib64/ce
# directory che conterrà i file .h
CE_INCLUDE=$(PREFIX)/include/ce
# percorso dell'hard disk emulato
CE_HDPATH=$(PREFIX)/share/hd.img
# percorso del boot loader
CE_QEMU_BOOT=$(CE_LIB32)/boot.bin
CE_QEMU_BOOT_DEBUG=$(CE_LIB32)/boot.debug
# directory che contiene i sorgenti (directory corrente)
CE_SOURCES=$(PWD)

# Useremo una macchina ospite per compilare e una macchina virtuale
# per eseguire i programmi compilati.
#
# Definiamo le seguenti varibili, in base al sistema operativo/cpu
# della macchina ospite:
#
# - compilatore C++, assembler, linker, archivio per i programmi
#   che devono girare sulla macchina virtuale (a 32 e 64 bit):
#
#       CE_CXX32 CE_CXX64 CE_AS32 CE_AS64 CE_LD32 CE_LD64 CE_AR
#
# - debugger e convertitore indirizzo->linea di codice, da usare sulla macchina
#   ospite, ma per i programmi della macchina virtuale:
#
#   	CE_GDB CE_ADDR2LINE
#
# Per i programmi che devono girare direttamente sulla macchina
# ospite useremo direttamente g++, as, ld, etc.

# sistema operativo
os:=$(shell uname -s)
# modello della CPU
cpu:=$(shell uname -m)
ifeq ($(os),Linux)
    CE_QEMU_AUDIO   ?= pa
    CE_QEMU_DISPLAY ?= gtk,gl=off
    ifeq ($(cpu),x86_64)
	CE_CXX32    := g++ -m32
	CE_CXX64    := g++
	CE_AS32     := gcc -c -x assembler-with-cpp -m32
	CE_AS64     := gcc -c -x assembler-with-cpp
	CE_LD32     := ld -melf_i386
	CE_LD64     := ld
	CE_AR       := ar
	CE_GDB      := gdb -nx
	CE_ADDR2LINE:= addr2line
	CE_NM	    := nm
	CE_CPPFILT  := c++filt
	CE_STRIP    := strip
    else ifeq ($(cpu),aarch64)
	CE_CXX32    := i686-linux-gnu-g++
	CE_CXX64    := i686-linux-gnu-g++ -m64
	CE_AS32     := i686-linux-gnu-gcc -c -x assembler-with-cpp
	CE_AS64     := i686-linux-gnu-gcc -c -x assembler-with-cpp -m64
	CE_LD32     := i686-linux-gnu-ld
	CE_LD64     := i686-linux-gnu-ld -melf_x86_64
	CE_AR       := i686-linux-gnu-ar
	CE_GDB      := gdb-multiarch -nx
	CE_ADDR2LINE:= addr2line
	CE_NM	    := nm
	CE_CPPFILT  := c++filt
	CE_STRIP    := i686-linux-gnu-strip
    else
$(error architettura sconosciuta: $(cpu))
    endif
else ifeq ($(os),Darwin)
	# su Darwin modifichiamo anche i backend audio/video da usare in QEMU
	# (le due variabili CE_QEMU_AUDIO e CE_QEMU_DISPLAY sono usate nello
	# script boot)
	CE_QEMU_AUDIO   ?= coreaudio
	CE_QEMU_DISPLAY ?= cocoa
	CE_CXX32    := x86_64-elf-g++ -m32
	CE_CXX64    := x86_64-elf-g++
	CE_AS32     := x86_64-elf-gcc -c -x assembler-with-cpp -m32
	CE_AS64     := x86_64-elf-gcc -c -x assembler-with-cpp
	CE_LD32     := x86_64-elf-ld -melf_i386
	CE_LD64     := x86_64-elf-ld
	CE_AR       := x86_64-elf-ar
	CE_GDB      := x86_64-elf-gdb --nx
	CE_ADDR2LINE:= x86_64-elf-addr2line
	CE_NM	    := x86_64-elf-nm
	CE_CPPFILT  := x86_64-elf-c++filt
	CE_STRIP    := x86_64-elf-strip
else
$(error sistema operativo sconosciuto: $(os))
endif

CXXFLAGS=-Wall -nostdlib -mno-sse -fno-exceptions -ggdb -fno-rtti -fno-stack-protector -mno-red-zone -fpic -fuse-ld=gold -fno-omit-frame-pointer -fcf-protection=none -Werror -Og
ASFLAGS=-g3
BCFLAGS:=$(CXXFLAGS) -fno-pic -fno-PIC -I. -ffreestanding
BLDFLAGS:=$(LDFLAGS) -no-pie -L. -L.. -z noexecstack
BLDLIBS:=-lce32
CXX_SUBDIRS=kbd vid hd bm pci apic util cfi tab_iter bare heap timer piix3 serial vm exc svga
CXX_SUB_SOURCES=$(wildcard $(addsuffix /*.cpp,$(CXX_SUBDIRS)))
CXX_SOURCES:=$(wildcard *.cpp $(CXX_SUB_SOURCES))
AS32_SOURCES:=$(wildcard as32/*.s)
AS64_SOURCES:=$(wildcard as64/*.s bare/*.s)
HEADERS:=$(wildcard include/*.h internal.h)
BUILD32:=build/32
BUILD64:=build/64
OBJECTS32:=$(addprefix $(BUILD32)/,$(CXX_SOURCES:.cpp=.o) $(AS32_SOURCES:.s=.o))
OBJECTS64:=$(addprefix $(BUILD64)/,$(CXX_SOURCES:.cpp=.o) $(AS64_SOURCES:.s=.o))
SCRIPTS_IN:=$(wildcard scripts/*.in)
SCRIPTS:=$(SCRIPTS_IN:.in=)

all: libce32.a libce64.a boot.debug boot.bin libce.conf $(SCRIPTS)

$(OBJECTS32) $(OBJECTS64): $(HEADERS)

libce32.a: $(OBJECTS32)
	@echo "==> Genero libce32.a"
	@$(CE_AR) cr libce32.a $?

libce64.a: $(OBJECTS64)
	@echo "==> Genero libce64.a"
	@$(CE_AR) cr libce64.a $?

$(BUILD32)/as32/%.o: as32/%.s
	@echo "==> [32b] Assemblo $(filter %.s,$^)"
	@mkdir -p $(dir $@)
	@$(CE_AS32) $(ASFLAGS) as32/$*.s -o $@

$(BUILD64)/as64/%.o: as64/%.s
	@echo "==> [64b] Assemblo $(filter %.s,$^)"
	@mkdir -p $(dir $@)
	@$(CE_AS64) $(ASFLAGS) as64/$*.s -o $@

$(BUILD64)/bare/%.o: bare/%.s
	@echo "==> [64b] Assemblo $(filter %.s,$^)"
	@mkdir -p $(dir $@)
	@$(CE_AS64) $(ASFLAGS) bare/$*.s -o $@

$(BUILD32)/%.o: %.cpp
	@echo "==> [32b] Compilo $(filter %.cpp,$^)"
	@mkdir -p $(dir $@)
	@$(CE_CXX32) -c $(CXXFLAGS) $*.cpp -o $@

$(BUILD64)/%.o: %.cpp
	@echo "==> [64b] Compilo $(filter %.cpp,$^)"
	@mkdir -p $(dir $@)
	@$(CE_CXX64) -c $(CXXFLAGS) $*.cpp -o $@

boot.debug: boot64/boot_s.o boot64/boot_cpp.o libce32.a
	@echo "==> [32b] Collego il boot loader (boot.debug)"
	@$(CE_LD32) 			\
		$(BLDFLAGS) 		\
		-o boot.debug		\
		--defsym=LOAD_ADDRESS=0x100000 \
		-Tboot64/boot.ld	\
		boot64/boot_s.o		\
		boot64/boot_cpp.o	\
		$(BLDLIBS)

boot.bin: boot.debug
	@echo "==> [32b] Genero boot.bin"
	@$(CE_STRIP) boot.debug -o boot.bin

# compilazione di boot.s e boot.cpp
boot64/boot_s.o: boot64/boot.s boot64/mboot.h
	@echo "==> [32b] Assemblo boot64/boot.s"
	@$(CE_AS32)			\
		$(BCFLAGS)		\
		-c			\
		boot64/boot.s		\
		-o boot64/boot_s.o

boot64/boot_cpp.o: boot64/boot.cpp boot64/mboot.h boot64/elf64.h include/libce.h include/vm.h include/boot.h
	@echo "==> [32b] Assemblo boot64/boot.cpp"
	@$(CE_CXX32)			\
		$(BCFLAGS)		\
		-c			\
		boot64/boot.cpp		\
		-o boot64/boot_cpp.o


.PHONY: zerohd clean install doc

$(CE_HDPATH):
	@echo "==> Creo '$(CE_HDPATH)' (dimensione: $(CE_HDSIZE))"
	@mkdir -p $(dir $(CE_HDPATH))
	@truncate -s $(CE_HDSIZE) $(CE_HDPATH)

zerohd:
	@echo "==> Azzero '$(CE_HDPATH)'"
	@dd if=/dev/zero of=$(CE_HDPATH) bs=$(CE_HDSIZE) count=1

libce.conf: Makefile
	@echo "==> Creo libce.conf"
	@( $(foreach v,BIN LIB32 LIB64 INCLUDE HDSIZE HDPATH QEMU_BOOT QEMU_BOOT_DEBUG SOURCES AS32 AS64 CXX32 CXX64 LD32 LD64 AR GDB ADDR2LINE NM CPPFILT STRIP QEMU_AUDIO QEMU_DISPLAY,echo export CE_$v="'$(CE_$v)'";) ) > $@

scripts/%: scripts/%.in
	@echo "==> Finalizzo $* "
	@sed 's|@CE_ETC@|$(CE_ETC)|g' $^ > $@

install: $(CE_HDPATH)
	@install -d 			$(CE_ETC)
	@echo "==> Installo libce.conf in '$(CE_ETC)'"
	@install -m 0444 libce.conf	$(CE_ETC)
	@install -d 			$(CE_BIN)
	@install -d 			$(CE_LIB32)
	@install -d 			$(CE_LIB64)
	@echo "==> Installo libce64.a e libce-debug.py in '$(CE_LIB64)'"
	@install -m 0444 libce64.a	$(CE_LIB64)
	@install -m 0444 libce-debug.py	$(CE_LIB64)
	@install -d 			$(CE_INCLUDE)
	@echo "==> Installo i file .h in '$(CE_INCLUDE)'"
	@install -m 0444 include/*	$(CE_INCLUDE)
	@echo "==> Installo boot.bin e boot.ld in '$(CE_LIB32)'"
	@install -m 0444 boot.bin	$(CE_LIB32)
	@install -m 0444 boot.debug	$(CE_LIB32)
	@install -m 0444 boot64/boot.ld	$(CE_LIB32)
	@echo "==> Installo gli script in '$(CE_BIN)'"
	@install -m 0755 $(SCRIPTS)	$(CE_BIN)

clean:
	@echo "==> Elimino tutti i file generati"
	@rm -rf *.o $(BUILD32) $(BUILD64) libce32.a libce64.a boot.bin boot64/*.o $(SCRIPTS)

doc:
	@echo "==> Genero la documentazione"
	@PROJECT_NUMBER=$$(git describe --tags HEAD) doxygen

+%:
	@echo $*=$($*)
