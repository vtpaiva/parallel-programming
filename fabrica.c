#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define resta_estoque (materia_deposito || materia_fabrica || canetas_deposito)
#define minimo(x, y) ((x > y) ? y : x)

int estoque_materia_prima = 10;
int tempo_materia_entrega = 1;
int tempo_fabricacao_caneta = 1;
int tempo_solicitacao_comprador = 1;
int canetas_compradadas_por_solicitacao = 5;
int capacidade_desposito_canetas = 1;
int materia_enviada_iteracao = 4;

int materia_deposito;
int materia_fabrica = 0;
int canetas_deposito = 0;
int canetas_solicitadas = 0;

pthread_mutex_t mutex_materia_deposito = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_materia_fabrica = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_canetas_deposito = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t deposito_canetas_cheio = PTHREAD_COND_INITIALIZER;

void* deposito_materia(void *thread_args) {
    time_t tempo_inicial = time(NULL);

    while(materia_deposito) {
        printf("%d %d %d\n", materia_deposito, materia_fabrica, canetas_deposito);
        
        if((int) ((time(NULL) - tempo_inicial)) >= tempo_materia_entrega) {
            pthread_mutex_lock(&mutex_materia_fabrica);

            while(canetas_deposito == capacidade_desposito_canetas) {
                pthread_cond_wait(&deposito_canetas_cheio, &mutex_materia_fabrica);
            }

            pthread_mutex_lock(&mutex_materia_deposito);

            materia_fabrica += minimo(materia_enviada_iteracao, materia_deposito);
            materia_deposito -= minimo(materia_enviada_iteracao, materia_deposito);

            pthread_mutex_unlock(&mutex_materia_deposito);
            pthread_mutex_unlock(&mutex_materia_fabrica);

            tempo_inicial = time(NULL);
        }
    }

    pthread_exit(NULL);
}

void *fabrica_canetas(void *thread_args) {
    time_t tempo_inicial;

    while(materia_deposito || materia_fabrica) {
        if(materia_fabrica) {
            tempo_inicial = time(NULL);

            while(time(NULL) - tempo_inicial < tempo_fabricacao_caneta);

            pthread_mutex_lock(&mutex_materia_fabrica);

            while(canetas_deposito >= capacidade_desposito_canetas) {
                pthread_cond_wait(&deposito_canetas_cheio, &mutex_materia_fabrica);
            }
            pthread_mutex_lock(&mutex_canetas_deposito);

            materia_fabrica--;
            canetas_deposito++;

            pthread_mutex_unlock(&mutex_canetas_deposito);
            pthread_mutex_unlock(&mutex_materia_fabrica);
        }
    }

    pthread_exit(NULL);
}

void *deposito_canetas(void *thread_args) {
    time_t tempo_inicial = time(NULL);
    
    while(resta_estoque) {
        if(time(NULL) - tempo_inicial >= tempo_solicitacao_comprador) {
            canetas_solicitadas = canetas_compradadas_por_solicitacao;

            while(resta_estoque && canetas_solicitadas) {
                if(time(NULL) - tempo_inicial >= tempo_solicitacao_comprador) {

                    pthread_mutex_lock(&mutex_canetas_deposito);
                    if(canetas_deposito >= canetas_solicitadas) {

                        printf("%d caneta(s) comprada(s).\n", canetas_solicitadas);
                        
                        canetas_deposito -= canetas_solicitadas;
                        canetas_solicitadas = 0;
                    }  else {
                        printf("%d caneta(s) comprada(s).\n", canetas_deposito);

                        canetas_solicitadas -= canetas_deposito;
                        canetas_deposito = 0;

                    }
                    pthread_mutex_unlock(&mutex_canetas_deposito);

                    pthread_cond_broadcast(&deposito_canetas_cheio);

                    tempo_inicial = time(NULL);
                }
            }

            tempo_inicial = time(NULL);
        }
    }

    pthread_exit(NULL);
}

void *controle(void *thread_args) {
    // controla
}

void *comprador(void *thread_args) {
    // compra
}

int main(void) {
    pthread_t t_deposito_materia;
    pthread_t t_fabrica_canetas;
    pthread_t t_deposito_canetas;

    materia_deposito = estoque_materia_prima;

    if (pthread_create(&t_deposito_materia, NULL, (void*) deposito_materia, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

    if (pthread_create(&t_fabrica_canetas, NULL, (void*) fabrica_canetas, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

    if (pthread_create(&t_deposito_canetas, NULL, (void*) deposito_canetas, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

    pthread_join(t_deposito_canetas, NULL);

    return 0;
}