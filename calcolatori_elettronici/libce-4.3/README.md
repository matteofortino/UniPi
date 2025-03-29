Questa libreria contiene alcune funzioni per l'accesso a basso livello
ad una macchina PC-compatibile (in particolare, una macchina QEMU con
processore Intel/AMD a 64 bit, bus PCI a 32 bit e periferiche ISA).

La libreria è usata per i seguenti scopi durante il corso di Calcolatori
Elettronici:

- per supportare un boot loader (_boot64_, incluso nella libreria stessa) che
  porta il processore nella modalità a 64 bit partendo dal modo protetto a 32
  bit; in questo caso la libreria è compilata a 32 bit;

- per supportare i programmi caricati da _boot64_. Questi programmi girano con
  la CPU a livello sistema e hanno il completo controllo della macchina QEMU.
  Questi programmi sono detti "bare" (nudi) nella documentazione, e durante il
  corso si distinguono in:

    - esercizi di traduzione da C++ ad Assembler (soprattutto nel
      caso in cui la macchina ospite non sia Intel/AMD, ma sia ad esempio
      un Mac M1/M2/...);

    - _esempiIO_, piccoli programmi che sono utilizzati
      per spiegare il funzionamento delle periferiche ISA, del bus PCI e
      delle interruzioni;

    - il modulo _sistema_ del nucleo introdotto nella seconda parte del corso.

- per supportare i moduli aggiuntivi del nucleo  (moduli _io_ e _utente_);

I file nella directory `bare` sono specifici dei programmi bare e non sono
usati da _boot64_ o dai moduli _io_ e _utente_.

Per compilare la libreria è sufficiente eseguire `make`. Si otterranno i seguenti
file:

- `boot.bin` (il bootloader boot64)

- `libce32.a` (libreria compilata a 32 bit, usata per costruire `boot.bin`)

- `libce64.a` (libreria compilata a 64 bit, usata per tutto il resto)

I file oggetto della libreria si trovano in `build`, suddivisi in varie sottodirectory.

La libreria contiene anche gli script `compile`, `boot` e `debug` e un file `libce-debug.py`
che contiene alcune estensioni per `gdb`, usate da `debug`.
Contiene inoltre gli script `colorlog` e `decodelog` usati per il post-processing del
messaggi invati sul log.
