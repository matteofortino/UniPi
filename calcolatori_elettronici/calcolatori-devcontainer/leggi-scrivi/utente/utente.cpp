#include <all.h>


void lettore(natq a){
    flog(LOG_INFO, "sto per leggere");
    natq msg = leggi();
    flog(LOG_INFO, "ho letto %lu", msg);
    terminate_p();
}

void scrittore(natq a){
    flog(LOG_INFO, "sto per scrivere");
    scrivi(500);
    flog(LOG_INFO, "ho scritto");
    terminate_p();
}

void pulitore(natq a){
    flog(LOG_INFO, "sto per pulire");
    pulisci();
    flog(LOG_INFO, "ho pulito");
    terminate_p();
}
void main(){
    activate_p(lettore, 0, 10, LIV_UTENTE);
    activate_p(lettore, 0, 10, LIV_UTENTE);
    activate_p(lettore, 0, 10, LIV_UTENTE);

    activate_p(pulitore, 0, 8, LIV_UTENTE);

    activate_p(scrittore, 0, 5, LIV_UTENTE);

    terminate_p();
}