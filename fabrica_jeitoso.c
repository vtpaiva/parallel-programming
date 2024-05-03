#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*

Grupo 6
André Jacob Suaide - 13864673
Marcus Vinicius da Silva - 13833150
Oliver Kenzo Kobayashi - 13676930
Rodrigo Rodrigues de Castro - 13695362
Vitor Augusto Paiva de Brito - 13732303

gcc -o exec fabrica.c -lpthread

Após o programa ser compilada pela linha de compilação acima,
execute o programa com ./exec

*/

// Condição que determina se o programa ainda deve executar.
#define resta_estoque (materia_deposito > 0 || materia_fabrica > 0 || canetas_deposito > 0)

#define minimo(x, y) ((x > y) ? y : x)
#define FALSE '0'
#define TRUE '1'

// Variáveis de entrada.

int estoque_materia_prima;
int materia_enviada_iteracao;
int tempo_materia_entrega;
int tempo_fabricacao_caneta;
int capacidade_desposito_canetas;
int canetas_compradadas_por_solicitacao;
int tempo_solicitacao_comprador;

// Variáveis globais de condições e quantidades da fábrica.

int materia_deposito;
int espacos_disponiveis;
int materia_fabrica = 0;
int canetas_deposito = 0;
int canetas_solicitadas = 0;
int canetas_enviadas = 0;
char permissao_controle = TRUE;
char solicitacao_comprador = '1';

// Mutexes do programa.

pthread_mutex_t mutex_materia_deposito = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_materia_fabrica = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_canetas_deposito = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_espacos_disponiveis = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_impressao_mensagem = PTHREAD_MUTEX_INITIALIZER;

// Condições de execução das threads.

pthread_cond_t permissao_controle_producao = PTHREAD_COND_INITIALIZER;
pthread_cond_t deposito_canetas_cheio = PTHREAD_COND_INITIALIZER;
pthread_cond_t deposito_canetas_livre = PTHREAD_COND_INITIALIZER;
pthread_cond_t solicitacao_compra_canetas = PTHREAD_COND_INITIALIZER;

void imprime_solicitacao_compra() {
    if(canetas_enviadas != 0) {
        if(canetas_solicitadas != 0) {
            printf("%d caneta(s) comprada(s), resta(m) %d.\n", canetas_enviadas, canetas_solicitadas);
        } else {
            printf("%d caneta(s) comprada(s).\n", canetas_enviadas);
        }
     } else {
        printf("Nao ha canetas em estoque no momento. Volte mais tarde!\n");
     }
}

void* deposito_materia_prima(void *thread_args) {
    time_t tempo_inicial = time(NULL);

    while(materia_deposito > 0) {
        
        if(time(NULL) - tempo_inicial >= tempo_materia_entrega) {
            pthread_mutex_lock(&mutex_materia_fabrica);

            // Executa apenas caso haja a permissão do 
            // controle por meio de uma variável de condição.
            while(permissao_controle == FALSE) {
                pthread_cond_wait(&permissao_controle_producao, &mutex_materia_fabrica);
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

    while(materia_deposito > 0 || materia_fabrica > 0) {

        if(materia_fabrica > 0) {
            tempo_inicial = time(NULL);

            while(time(NULL) - tempo_inicial < tempo_fabricacao_caneta);

            pthread_mutex_lock(&mutex_materia_fabrica);

            // Executa apenas caso haja a permissão do 
            // controle por meio de uma variável de condição.
            while(permissao_controle == FALSE) {
                pthread_cond_wait(&permissao_controle_producao, &mutex_materia_fabrica);
            }

            pthread_mutex_lock(&mutex_canetas_deposito);

            canetas_deposito++;
            materia_fabrica--;

            pthread_mutex_unlock(&mutex_canetas_deposito);
            pthread_mutex_unlock(&mutex_materia_fabrica);
        }
    }

    pthread_exit(NULL);
}

void *controle(void *thread_args) {
    while(resta_estoque) {
        pthread_mutex_lock(&mutex_espacos_disponiveis);

        // Permanece bloqueada enquanto houverem espaços 
        // livres no depósito de canetas.
        while(espacos_disponiveis != 0) {
            pthread_cond_wait(&deposito_canetas_livre, &mutex_espacos_disponiveis);
        }

        // No momento em que todos os espaços do despósito são ocupados,
        // bloqueia o depósito de matéria prima e a fábrica de canetas.
        permissao_controle = FALSE;

        // Permanece bloqueada enquanto não houverem espaços 
        // livres no depósito de canetas.
        while(espacos_disponiveis == 0) {
            pthread_cond_wait(&deposito_canetas_cheio, &mutex_espacos_disponiveis);
        }

        // No momento em que é liberado algum espaço no depósito,
        // libera o depósito de matéria prima e a fábrica de canetas.
        permissao_controle = TRUE;

        pthread_cond_broadcast(&permissao_controle_producao);
        pthread_mutex_unlock(&mutex_espacos_disponiveis);
    }
}

void *deposito_canetas(void *thread_args) {

    while(resta_estoque) {

        pthread_mutex_lock(&mutex_espacos_disponiveis);

        // Atualiza os espaços disponíveis a cada execução.
        espacos_disponiveis = capacidade_desposito_canetas - canetas_deposito;

        // Caso não hajam espaços disponíveis, avise o 
        // controle para o bloqueio.
        if(espacos_disponiveis == 0) {
            pthread_cond_broadcast(&deposito_canetas_livre);
        }

        pthread_mutex_unlock(&mutex_espacos_disponiveis);

        // Caso o comprador tenha solicitado uma compra,
        // transfira as canetas.
        if(solicitacao_comprador == '2') {
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
            solicitacao_comprador = '0';
            pthread_cond_broadcast(&solicitacao_compra_canetas);
        }
    }
}

void *comprador(void *thread_args) {
    time_t tempo_inicial = time(NULL);
    
    while(resta_estoque) {
        pthread_mutex_lock(&mutex_impressao_mensagem);

        // Enquanto não receber o alerta para iniciar o ciclo da compra,
        // permanece bloqueado.
        while(solicitacao_comprador != '1') {
            pthread_cond_wait(&solicitacao_compra_canetas, &mutex_impressao_mensagem);
        }

        while(time(NULL) - tempo_inicial < tempo_solicitacao_comprador);

        // Solicita a compra.
        if(canetas_solicitadas == 0) {
            canetas_solicitadas = canetas_compradadas_por_solicitacao;
        }

        // Solicita a transferência de canetas ao depósito e envia
        // os dados solicitados.
        solicitacao_comprador = '2';

        tempo_inicial = time(NULL);
    }

    pthread_exit(NULL);

}

int criador() {
    pthread_t t_deposito_materia_prima;
    pthread_t t_fabrica_canetas;
    pthread_t t_controle;
    pthread_t t_deposito_canetas;
    pthread_t t_comprador_canetas;

    // Inicializa as threads com suas respectivas rotinas.

    if (pthread_create(&t_deposito_materia_prima, NULL, (void*) deposito_materia_prima, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

    if (pthread_create(&t_fabrica_canetas, NULL, (void*) fabrica_canetas, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

    if (pthread_create(&t_controle, NULL, (void*) controle, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

    if (pthread_create(&t_deposito_canetas, NULL, (void*) deposito_canetas, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

    if (pthread_create(&t_comprador_canetas, NULL, (void*) comprador, NULL) != 0){
        printf("Erro ao criar Thread! \n");
        return 0;
    }

    while(resta_estoque) {
        pthread_mutex_lock(&mutex_impressao_mensagem);

        // Enquanto não receber os dados da compra, permanece
        // bloqueado.
        while(solicitacao_comprador != '0') {
            pthread_cond_wait(&solicitacao_compra_canetas, &mutex_impressao_mensagem);
        }

        imprime_solicitacao_compra();

        pthread_mutex_unlock(&mutex_impressao_mensagem);

        // Acorda o comprador para iniciar o ciclo de compra.
        solicitacao_comprador = '1';
        pthread_cond_broadcast(&solicitacao_compra_canetas);
    }

    return 1;
}

int main(void) {
    scanf("%d %d %d %d %d %d %d", &estoque_materia_prima, &materia_enviada_iteracao,
                                  &tempo_materia_entrega, &tempo_fabricacao_caneta,
                                  &capacidade_desposito_canetas, &canetas_compradadas_por_solicitacao,
                                  &tempo_solicitacao_comprador);

    materia_deposito = estoque_materia_prima;
    espacos_disponiveis = capacidade_desposito_canetas;

    if(criador() == 0) {
        return printf("Erro ao executar o criador.\n"), 0;
    }

    return 0;
}
