#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Condição que determina se o programa ainda deve executar.
#define resta_estoque (materia_deposito > 0 || materia_fabrica > 0 || canetas_deposito > 0)

#define minimo(x, y) ((x > y) ? y : x)

const char FALSE = '\0';
const char TRUE = '1';
const int ERRO =  1;
const int SUCESSO = 0;
const char CRIADOR = '0';
const char COMPRADOR = '1';
const char DEPOSITO = '2';

// Variáveis de entrada.

int estoque_materia_prima;
int materia_enviada_iteracao;
int tempo_materia_entrega;
int tempo_fabricacao_caneta;
int capacidade_desposito_canetas;
int canetas_compradadas_por_solicitacao;
int tempo_solicitacao_comprador;

// Variáveis universais para intercâmbio de dados entre threads.

int materia_deposito;
int espacos_disponiveis;
int materia_fabrica = 0;
int canetas_deposito = 0;
int canetas_enviadas = 0;
int canetas_solicitadas = 0;
char permissao_controle = TRUE;
char solicitacao_comprador = COMPRADOR;

// Mutexes do programa.

pthread_mutex_t mutex_materia_deposito = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_materia_fabrica = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_canetas_deposito = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_espacos_disponiveis = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_impressao_mensagem = PTHREAD_MUTEX_INITIALIZER;

// Condições de execução das threads.

pthread_cond_t permissao_controle_execucao = PTHREAD_COND_INITIALIZER;
pthread_cond_t deposito_canetas_cheio = PTHREAD_COND_INITIALIZER;
pthread_cond_t deposito_canetas_livre = PTHREAD_COND_INITIALIZER;
pthread_cond_t solicitacao_compra_canetas = PTHREAD_COND_INITIALIZER;

void* deposito_materia_prima(void *thread_args) {
    time_t tempo_inicial = time(NULL);

    while(TRUE) {
        
        if(time(NULL) - tempo_inicial >= tempo_materia_entrega) {
            pthread_mutex_lock(&mutex_materia_fabrica);

            // Executa apenas caso haja a permissão do 
            // controle por meio de uma variável de condição.
            while(permissao_controle == FALSE) {
                pthread_cond_wait(&permissao_controle_execucao, &mutex_materia_fabrica);
            }

            pthread_mutex_lock(&mutex_materia_deposito);

            materia_fabrica += minimo(materia_enviada_iteracao, materia_deposito);
            materia_deposito -= minimo(materia_enviada_iteracao, materia_deposito);

            pthread_mutex_unlock(&mutex_materia_deposito);
            pthread_mutex_unlock(&mutex_materia_fabrica);

            tempo_inicial = time(NULL);
        }
    }
}

void *fabrica_canetas(void *thread_args) {
    time_t tempo_inicial;

    while(TRUE) {

        if(materia_fabrica > 0) {
            tempo_inicial = time(NULL);

            while(time(NULL) - tempo_inicial < tempo_fabricacao_caneta);

            pthread_mutex_lock(&mutex_materia_fabrica);

            // Executa apenas caso haja a permissão do 
            // controle por meio de uma variável de condição.
            while(permissao_controle == FALSE) {
                pthread_cond_wait(&permissao_controle_execucao, &mutex_materia_fabrica);
            }

            pthread_mutex_lock(&mutex_canetas_deposito);

            canetas_deposito++;
            materia_fabrica--;

            pthread_mutex_unlock(&mutex_canetas_deposito);
            pthread_mutex_unlock(&mutex_materia_fabrica);
        }
    }
}

void *controle(void *thread_args) {

    while(TRUE) {
        pthread_mutex_lock(&mutex_espacos_disponiveis);

        // Caso não hajam espaços disponíveis no depósito de canetas,
        // bloqueia o envio de matéria prima e a produção de canetas.
        if(espacos_disponiveis == 0) {
            permissao_controle = FALSE;
        } else if(permissao_controle != TRUE) {
            permissao_controle = TRUE;
            pthread_cond_broadcast(&permissao_controle_execucao);
        }

        pthread_mutex_unlock(&mutex_espacos_disponiveis);
    }
}

void *deposito_canetas(void *thread_args) {

    while(TRUE) {

        pthread_mutex_lock(&mutex_espacos_disponiveis);

        // Atualiza os espaços disponíveis a cada execução.
        espacos_disponiveis = capacidade_desposito_canetas - canetas_deposito;

        pthread_mutex_unlock(&mutex_espacos_disponiveis);

        // Caso o comprador tenha solicitado uma compra,
        // transfira as canetas.
        if(solicitacao_comprador == DEPOSITO) {
            pthread_mutex_lock(&mutex_canetas_deposito);

            if(canetas_deposito >= canetas_solicitadas) {
                canetas_enviadas = canetas_solicitadas;
                canetas_deposito -= canetas_solicitadas;
                canetas_solicitadas = 0;
            }  else {
                canetas_enviadas = canetas_deposito;
                canetas_solicitadas -= canetas_deposito;
                canetas_deposito = 0;
            }

            pthread_mutex_unlock(&mutex_canetas_deposito);
            pthread_cond_broadcast(&deposito_canetas_cheio);

            pthread_mutex_unlock(&mutex_impressao_mensagem);

            // Alerta o criador para imprimir os dados da compra.
            solicitacao_comprador = CRIADOR;
            pthread_cond_broadcast(&solicitacao_compra_canetas);
        }
    }
}

void *comprador(void *thread_args) {
    time_t tempo_inicial = time(NULL);
    
    while(TRUE) {
        
        pthread_mutex_lock(&mutex_impressao_mensagem);

        // Enquanto não receber o alerta para iniciar o ciclo da compra,
        // permanece bloqueado.
        while(solicitacao_comprador != COMPRADOR) {
            pthread_cond_wait(&solicitacao_compra_canetas, &mutex_impressao_mensagem);
        }

        while(time(NULL) - tempo_inicial < tempo_solicitacao_comprador);

        // Solicita a compra.
        canetas_solicitadas = canetas_compradadas_por_solicitacao;

        // Solicita a transferência de canetas ao depósito e envia
        // os dados solicitados.
        solicitacao_comprador = DEPOSITO;

        tempo_inicial = time(NULL);
    }
}

void imprime_solicitacao_compra() {
    if(canetas_enviadas != 0) {
        printf("%d caneta(s) comprada(s).\n", canetas_enviadas);
     } else {
        printf("Nao ha canetas em estoque no momento. Volte mais tarde!\n");
     }
}

int criador() {
    int total_canetas_compradas = 0;

    pthread_t t_deposito_materia_prima;
    pthread_t t_fabrica_canetas;
    pthread_t t_controle;
    pthread_t t_deposito_canetas;
    pthread_t t_comprador_canetas;

    // Inicializa as threads com suas respectivas rotinas.

    if (pthread_create(&t_deposito_materia_prima, NULL, (void*) deposito_materia_prima, NULL) != 0){
        printf("Erro ao criar thread Deposito de materia prima! \n");
        fflush(0);
	return ERRO;
    }

    if (pthread_create(&t_fabrica_canetas, NULL, (void*) fabrica_canetas, NULL) != 0){
        printf("Erro ao criar thread fabrica de canetas! \n");
        fflush(0);
	return ERRO;
    }

    if (pthread_create(&t_controle, NULL, (void*) controle, NULL) != 0){
        printf("Erro ao criar thread controle! \n");
        fflush(0);
	return ERRO;
    }

    if (pthread_create(&t_deposito_canetas, NULL, (void*) deposito_canetas, NULL) != 0){
        printf("Erro ao criar thread deposito de canetas! \n");
        fflush(0);
	return ERRO;
    }

    if (pthread_create(&t_comprador_canetas, NULL, (void*) comprador, NULL) != 0){
        printf("Erro ao criar thread comprador de canetas! \n");
        fflush(0);
	return ERRO;
    }

    while(resta_estoque) {
        pthread_mutex_lock(&mutex_impressao_mensagem);

        // Enquanto não receber os dados da compra, permanece
        // em espera.
        while(solicitacao_comprador != CRIADOR) {
            pthread_cond_wait(&solicitacao_compra_canetas, &mutex_impressao_mensagem);
        }

        imprime_solicitacao_compra();

        total_canetas_compradas += canetas_enviadas;

        pthread_mutex_unlock(&mutex_impressao_mensagem);

        // Alerta o comprador para iniciar o ciclo de compra.
        solicitacao_comprador = COMPRADOR;
        pthread_cond_broadcast(&solicitacao_compra_canetas);
    }

    printf("Total de caneta(s) comprada(s): %d\n", total_canetas_compradas);

    return SUCESSO;
}

int main(int argc, char **argv) {

    if(argc == 8) {
        estoque_materia_prima = atoi(argv[1]);
        materia_enviada_iteracao = atoi(argv[2]);
        tempo_materia_entrega = atoi(argv[3]);
        tempo_fabricacao_caneta = atoi(argv[4]);
        capacidade_desposito_canetas = atoi(argv[5]);
        canetas_compradadas_por_solicitacao = atoi(argv[6]);
        tempo_solicitacao_comprador =  atoi(argv[7]);
    } else {
        printf("Erro no fornecimento de entradas \n");
        exit(ERRO);
    }
    
    materia_deposito = estoque_materia_prima;
    espacos_disponiveis = capacidade_desposito_canetas;

    exit(criador());
}
