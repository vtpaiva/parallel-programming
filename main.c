#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREADS 6

// argumentos de entrada
int materia_prima_existente;
int materia_prima_enviada;
int tempo_envio_materia;
int tempo_fabrica_caneta;
int max_caneta_armazenada;
int canetas_compra;
int tempo_espera_compra;

// variaveis universais para comunicação entre threads
int materia_prima_deposito;
int materia_prima_fabrica;
int caneta_deposito;
int caneta_enviada;

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
void deposito_materia();
void fabrica_caneta();
void controle();
void deposito_caneta();
void comprador();

int main(void){

    // criação das threads
    pthread_t t_criador;
    pthread_t t_deposito_materia;
    pthread_t t_fabrica_caneta;
    pthread_t t_controle;
    pthread_t t_deposito_caneta;
    pthread_t t_comprador;

    exit(0);
}