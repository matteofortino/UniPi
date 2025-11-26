#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NTHREADS 12

pthread_t father;
int cont = 0;
pthread_mutex_t mutex;

void *func(void *arg) {
  int *id = (int *)arg;
  for (int i = 0; i < 100000; i++) {
    pthread_mutex_lock(&mutex);
    cont++;
    printf("Sono il thread di id %d e il valode di cont e' %d\n", *id, cont);
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(NULL);
}
int main() {
  pthread_mutex_init(&mutex, NULL);
  father = pthread_self();
  pthread_t tid[NTHREADS];
  int *args[NTHREADS];
  for (int i = 0; i < NTHREADS; i++) {
    args[i] = (int *)malloc(sizeof(int));
    *args[i] = i + 1;
    pthread_create(&tid[i], NULL, func, (void *)args[i]);
  }

  pthread_exit(NULL);
}
