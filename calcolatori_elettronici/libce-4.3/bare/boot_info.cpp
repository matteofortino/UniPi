#include "../internal.h"
#include "../include/boot.h"

// Variabile globale inizializzata in start64.s con l'indirizzo della struttura
// boot64_info creata dal boot loader
boot64_info* boot_info = nullptr;
