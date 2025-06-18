#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#define HAVE_STRUCT_TIMESPEC
#pragma comment(lib,"pthreadVC2.lib")


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#define LINHAS 50000
#define COLUNAS 50000
#define SEED 10
#define NUM_THREADS 12
#define LINHAS_MACROBLOCO 1000
#define COLUNAS_MACROBLOCO 1000


// Variáveis globais
short int** matriz;
int totalPrimos = 0;
long long macroblocoAtual = 0;
long long totalMacroblocos = 0;
long long totalMacroblocosColunas = 0;


// Mutex para regiões críticas
pthread_mutex_t mutexMacrobloco;
pthread_mutex_t mutexTotalPrimos;


// Função para alocar memória à matriz
void alocaMatriz() {

    matriz = (short int**)malloc(LINHAS * sizeof(short int*));
    for (int i = 0; i < LINHAS; i++) {
        matriz[i] = (short int*)malloc(COLUNAS * sizeof(short int));
    }
}


// Função para preencher a matriz
void preencheMatriz() {

    srand(SEED);
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            matriz[i][j] = rand() % 32000;
        }
    }
}


// Se ele for primo, a função retorna 1 (true), caso contrário, 0 (false)
int ehPrimo(int n) {

    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    int limite = sqrt(n);
    for (int i = 3; i <= limite; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}


// Função para liberar a memória
void liberaMatriz() {

    for (int i = 0; i < LINHAS; i++) {
        free(matriz[i]);
    }
    free(matriz);
}


// Função executada por uma única thread
void BuscaSerial() {

    clock_t inicio = clock();

    int contador = 0;
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (ehPrimo(matriz[i][j])) contador++;
        }
    }

    clock_t fim = clock();
    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("\nPrimos encontrados: %d\n", contador);
    printf("Tempo de execucao: %.2f segundos\n", tempo);
}


// Função executada por cada thread
void* BuscaParalela(void* arg) {

    int primosLocais = 0;

    while (1) {

        long long macroAtual;

        // Região crítica para pegar o próximo macrobloco
        pthread_mutex_lock(&mutexMacrobloco);
        macroAtual = macroblocoAtual;
        macroblocoAtual++;
        pthread_mutex_unlock(&mutexMacrobloco);

        if (macroAtual >= totalMacroblocos) break;

        // Cálculo das coordenadas do macrobloco
        long long linhaMacro = (macroAtual / totalMacroblocosColunas) * LINHAS_MACROBLOCO;
        long long colunaMacro = (macroAtual % totalMacroblocosColunas) * COLUNAS_MACROBLOCO;

        // Define os limites do macrobloco
        long long linhaMax = linhaMacro + LINHAS_MACROBLOCO;
        if (linhaMax > LINHAS) linhaMax = LINHAS;

        long long colunaMax = colunaMacro + COLUNAS_MACROBLOCO;
        if (colunaMax > COLUNAS) colunaMax = COLUNAS;

        // Processa o macrobloco
        for (long long i = linhaMacro; i < linhaMax; i++) {
            for (long long j = colunaMacro; j < colunaMax; j++) {
                if (ehPrimo(matriz[i][j])) {
                    primosLocais++;
                }
            }
        }
    }

    // Atualiza o total global de forma segura
    pthread_mutex_lock(&mutexTotalPrimos);
    totalPrimos += primosLocais;
    pthread_mutex_unlock(&mutexTotalPrimos);

    pthread_exit(NULL);
}


int main() {

    if ((LINHAS % LINHAS_MACROBLOCO != 0) || (COLUNAS % COLUNAS_MACROBLOCO != 0)) {
        printf("Macroblocos definidos nao serao identicos, nao eh possível continuar o processamento");
        exit(1);
    }

    clock_t startMatriz = clock();

    printf("Alocando e preenchendo matriz de tamanho %d x %d\n", LINHAS, COLUNAS);
    printf("Dimensao dos macroblocos %d x %d\n", LINHAS_MACROBLOCO, COLUNAS_MACROBLOCO);
    printf("Numero SEED: %d\n", SEED);
    printf("Numero de threads: %d\n", NUM_THREADS);

    alocaMatriz();
    preencheMatriz();

    clock_t endMatriz = clock();
    double tempoMatriz = (double)(endMatriz - startMatriz) / CLOCKS_PER_SEC;

    printf("\nTempo para alocar e preencher a matriz: %.2f segundos\n", tempoMatriz);
    printf("\nBusca serial\n");

    clock_t startSerial = clock();

    BuscaSerial();

    clock_t endSerial = clock();
    double tempoSerial = (double)(endSerial - startSerial) / CLOCKS_PER_SEC;

    // Inicializa os mutexes
    pthread_mutex_init(&mutexMacrobloco, NULL);
    pthread_mutex_init(&mutexTotalPrimos, NULL);

    printf("\nBusca paralela\n");

    totalMacroblocosColunas = (COLUNAS + COLUNAS_MACROBLOCO - 1) / COLUNAS_MACROBLOCO;
    long long totalMacroblocosLinhas = (LINHAS + LINHAS_MACROBLOCO - 1) / LINHAS_MACROBLOCO;
    totalMacroblocos = totalMacroblocosLinhas * totalMacroblocosColunas;

    totalPrimos = 0;
    macroblocoAtual = 0;

    clock_t startParalelo = clock();

    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, BuscaParalela, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t endParalelo = clock();
    double tempoParalelo = (double)(endParalelo - startParalelo) / CLOCKS_PER_SEC;

    printf("\nPrimos encontrados: %d\n", totalPrimos);
    printf("Tempo de execucao: %.2f segundos\n", tempoParalelo);

    // Calcula o speedup
    double speedup = tempoSerial / tempoParalelo;
    printf("\nSpeedup: %.2f\n", speedup);

    // Destroi os mutexes
    pthread_mutex_destroy(&mutexMacrobloco);
    pthread_mutex_destroy(&mutexTotalPrimos);

    liberaMatriz();
    return 0;
}