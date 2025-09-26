#include <all.h>

void corpo(natq a)
{
  natw id;
  id = getid();
  printf("Il mio id: %hu e il mio paramentro e' %ld\n", id, a);
  pause();
  terminate_p();
}

int main()
{
  activate_p(corpo, 5, 20, LIV_UTENTE);
  activate_p(corpo, 10, 50, LIV_UTENTE);
  terminate_p();
}
