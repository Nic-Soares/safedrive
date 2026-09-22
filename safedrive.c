#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_AMOSTRAS 100

/* Funcao 1 - Carregar dados iniciais (opcao 1 do menu).
 * Preenche velocidades, sensores_frontais e sensores_laterais com 50
 * registros aleatorios. processamento e status ficam zerados aqui - sao
 * calculados depois, na opcao 3 do menu. */
void inicializar_matrizes(float velocidades[][2], float sensores_frontais[][3],
                           float sensores_laterais[][2], float processamento[][2],
                           int status[][3], int *total_amostras);

/* Funcao 2 - Inserir nova amostra (opcao 2 do menu).
 * So grava os valores ja lidos (pela main) na proxima linha vazia -
 * nao contem scanf, por causa da regra de isolamento de E/S. */
void inserir_amostra(float velocidades[][2], float sensores_frontais[][3],
                      float sensores_laterais[][2], int total_amostras,
                      float vel_atual, float vel_frente,
                      float radar, float lidar, float camera,
                      float lat_esquerda, float lat_direita);

/* Regra A - Fusao de sensores: mediana das 3 leituras frontais ->
 * processamento[][0] (distancia validada). */
void fusao_sensores(float sensores_frontais[][3], float processamento[][2],
                     int total_amostras);

/* Regra B - Distancia segura de frenagem -> processamento[][1]. */
void distancia_segura(float velocidades[][2], float processamento[][2],
                       int total_amostras, float atrito, int sensibilidade);

/* Regra C - Risco frontal / AEB -> status[][0]. */
void risco_frontal(float velocidades[][2], float processamento[][2],
                    int status[][3], int total_amostras);

/* Regra D - Assistente de faixa dinamico -> status[][1] e status[][2]. */
void assistente_faixa(float velocidades[][2], float sensores_laterais[][2],
                       int status[][3], int total_amostras);

/* Regra E - Unica funcao autorizada a usar printf apos os calculos. */
void exibir_relatorio(float velocidades[][2], float sensores_frontais[][3],
                       float sensores_laterais[][2], float processamento[][2],
                       int status[][3], int total_amostras);

int main(void) {
    /* TODO (sua vez):
     * - declarar as 5 matrizes com MAX_AMOSTRAS linhas
     * - declarar total_amostras = 0
     * - ler atrito (float) e sensibilidade (int, 1-3)
     * - laco do menu (4 opcoes) chamando as funcoes acima
     */
    return 0;
}
