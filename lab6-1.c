/*
Disciplina Programacao Concorrente
Professora: Silvana Rossetto
Estudante: Mariana Fernandes Cabral
DRE: 121060838
Laboratorio 6: Atividade 1
*/

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<semaphore.h>

#define N 10 //tamanho do buffer
#define P 2 //qtde de threads produtoras
#define C 4 //qtde de threads consumidoras

//variaveis do problema
int Buffer[N]; //espaco de dados compartilhados
int count=0;

//variaveis para sincronizacao
sem_t slotCheio, slotVazio;
sem_t mutex;

//inicializa o buffer
void IniciaBuffer(int n) {
  int i;
  for(i=0; i<n; i++)
    Buffer[i] = 0;
}

//imprime o buffer
void ImprimeBuffer(int n) {
  int i;
  for(i=0; i<n; i++)
    printf("%d ", Buffer[i]);
  printf("\n");
}

//insere um elemento no Buffer ou bloqueia a thread caso o Buffer esteja cheio
void Insere (int item, int id) {
   sem_wait(&slotVazio);
   sem_wait(&mutex);   
   printf("Inserindo na posicao %d\n", count);
   Buffer[count] = item;
   count = (count + 1)%N;
   ImprimeBuffer(N);
//printf("count = %d\n", count);
   if (count==0){
        for(int i = 0; i < N; i++){
            sem_post(&slotCheio);
        }
   }

   sem_post(&mutex);
}

//retira um elemento no Buffer ou bloqueia a thread caso o Buffer esteja vazio
int Retira (int id) {
   int item;
   sem_wait(&slotCheio);
   sem_wait(&mutex);
   printf("Consumindo a posicao %d\n", count);
   item = Buffer[count];
   Buffer[count] = 0;
   count = (count + 1)%N;
   ImprimeBuffer(N);
//printf("count = %d\n", count);
   if (count==0){
        for(int j = 0; j < N; j++){
            sem_post(&slotVazio);
        }
   }
   sem_post(&mutex);
   return item;
}

//thread produtora
void * produtor(void * arg) {
  int *id = (int *) arg;
  printf("Sou a thread produtora %d\n", *id);
  while(1) {
    //produzindo o item
    Insere(*id, *id);
    sleep(1);
  } 
  free(arg);
  pthread_exit(NULL);
}

//thread consumidora
void * consumidor(void * arg) {
  int *id = (int *) arg;
  int item;
  printf("Sou a thread consumidora %d\n", *id);
  while(1) {
    item = Retira(*id);
    sleep(1); //faz o processamento do item 
  } 
  free(arg);
  pthread_exit(NULL);
}

//funcao principal
int main(void) {
  //variaveis auxiliares
  int i;
 
  //identificadores das threads
  pthread_t tid[P+C];
  int *id[P+C];

  //aloca espaco para os IDs das threads
  for(i=0; i<P+C;i++) {
    id[i] = malloc(sizeof(int));
    if(id[i] == NULL) exit(-1);
    *id[i] = i+1;
  }

  //inicializa o Buffer
  IniciaBuffer(N);  

  //inicializa as variaveis de sincronizacao
  sem_init(&mutex, 0, 1);
  sem_init(&slotCheio, 0, 0);
  sem_init(&slotVazio, 0, N);

  //cria as threads produtoras
  for(i=0; i<P; i++) {
    if(pthread_create(&tid[i], NULL, produtor, (void *) id[i])) exit(-1);
  } 
  
  //cria as threads consumidoras
  for(i=0; i<C; i++) {
    if(pthread_create(&tid[i+P], NULL, consumidor, (void *) id[i+P])) exit(-1);
  } 
  pthread_exit(NULL);
  return 1;
}
