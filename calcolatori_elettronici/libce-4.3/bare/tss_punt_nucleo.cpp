#include "../internal.h"
#include "../include/boot.h"

// Variabile globale inizializzata in start64.s con le informazioni passate dal
// boot loader
natq tss_punt_nucleo;
