/*

André Jacob Suaide - 13864673
Rodrigo Rodrigues de Castro - 13695362
Oliver Kenzo Kobayashi - 
Marcus Vinicius da Silva - 
Vitor Paiva de Brito - 

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREADS 6

// argumentos de entrada (CONSTANTES, NÃO MUDAM)

int materia_prima_existente;    // (quantidade de materia prima inicial)
int materia_prima_enviada;      // (quantidade de materia prima enviada à fabrica)
int tempo_envio_materia;        // (tempo entre cada envio de materia prima)
int tempo_fabrica_caneta;       // (tempo para fabricar UMA caneta)
int max_caneta_armazenada;      // (quantidade MÁXIMA de canetas suportada pelo Deposito de canetas)
int canetas_compra;             // (quantidade de canetas compradas a cada iteração)
int tempo_espera_compra;        // (coll down para compra de canetas)

// variaveis universais para comunicação entre threads ( G L O B A I S )

int materia_prima_deposito;     // (quantidade atual de materia prima no deposito)
int materia_prima_fabrica;      // (quantidade atual de materia prima na fabrica)
int caneta_deposito;            // (quantidade atual de canetas no deposito)
int caneta_enviada;             // (quantidade de caneta comprada com sucesso)

// mutexes

pthread_mutex_t mutex_materia_deposito;
pthread_mutex_t mutex_materia_fabrica;
pthread_mutex_t mutex_caneta_deposito;
pthread_mutex_t mutex_caneta_enviada;

// variaveis de condicao

pthread_cond_t materia_deposito_vazio, materia_deposito_cheio;
pthread_cond_t materia_fabrica_vazio, materia_fabrica_cheio; // se nao tiver limite nao precisa
pthread_cond_t caneta_deposito_vazio, caneta_deposito_cheio;

// funções responsaveis pelo gestao das threads
void criador();
void* deposito_materia();
void* fabrica_caneta();
void* controle();
void* deposito_caneta();
void* comprador();


void criador(){

    // criação das threads
    pthread_t t_deposito_materia;
    pthread_t t_fabrica_caneta;
    pthread_t t_controle;
    pthread_t t_deposito_caneta;
    pthread_t t_comprador;

    pthread_mutex_init(&mutex_caneta_deposito, NULL);
    pthread_mutex_init(&mutex_materia_fabrica, NULL);
    pthread_mutex_init(&mutex_caneta_deposito, NULL);
    pthread_mutex_init(&mutex_caneta_enviada, NULL);
    pthread_cond_init(&, NULL);

    if (pthread_create(&t_deposito_materia, NULL, (void*) deposito_materia, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

    if (pthread_create(&t_fabrica_caneta, NULL, (void*) fabrica_caneta, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

    if (pthread_create(&t_controle, NULL, (void*) controle, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

    if (pthread_create(&t_deposito_caneta, NULL, (void*) deposito_caneta, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

    if (pthread_create(&t_comprador, NULL, (void*) comprador, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

}

void* deposito_materia(){





}


int main(void){

    criador();

    exit(0);
}