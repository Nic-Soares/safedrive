#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_AMOSTRAS 100

/*
 * ============================================================================
 * SAFEDRIVE — Processamento de telemetria ADAS (Projeto 1, AP2)
 * Resumo do enunciado — só o essencial, pra não precisar abrir o PDF toda hora.
 * Fonte completa: Projeto 1 — Enunciado (AlgProII-2026.2 Projeto1).pdf (Vault)
 * ============================================================================
 *
 * RESTRIÇÕES TÉCNICAS
 * - Proibido: struct, malloc/realloc, variável global.
 * - O enunciado também cita "ponteiros explícitos" como proibidos.
 * - Isolamento de E/S: funções de cálculo (Regras A-D) não podem ter scanf
 *   nem printf — só recebem dados por parâmetro. Leitura só em main(). Só a
 *   função de relatório (Regra E) pode ter printf.
 *
 * DADOS (MAX_AMOSTRAS = 100 linhas por matriz, só 50 usadas no início)
 * - velocidades[][2]       col 0 = velocidade atual (km/h)
 *                          col 1 = velocidade do carro da frente (km/h)
 * - sensores_frontais[][3] col 0 = radar, col 1 = lidar, col 2 = câmera
 * - sensores_laterais[][2] col 0 = distância faixa esquerda (m)
 *                          col 1 = distância faixa direita (m)
 * - processamento[][2]     col 0 = distância validada (Regra A)
 *                          col 1 = distância segura calculada (Regra B)
 * - status[][3] (int)      col 0 = status frontal (Regra C)
 *                          col 1 = status faixa esquerda (Regra D)
 *                          col 2 = status faixa direita (Regra D)
 *
 * MENU (main, laço até sair)
 * 1. Carregar dados iniciais — 50 registros aleatórios
 * 2. Inserir nova amostra — lê velocidades + 5 leituras de sensores, grava
 *    na próxima linha vazia
 * 3. Processar e exibir relatório — roda as Regras A-E em sequência
 * 4. Sair
 *
 * REGRA A — Fusão de sensores
 * Mediana das 3 leituras de sensores_frontais → processamento[][0].
 *
 * REGRA B — Distância segura de frenagem
 * TempoReacao por sensibilidade: 1 = 1.0s, 2 = 1.5s, 3 = 2.0s.
 * Converter velocidade de km/h pra m/s (÷ 3.6) antes de calcular.
 * Distancia = (v × TempoReacao) + v² / (2 × atrito × 9.81)  → processamento[][1]
 *
 * REGRA C — Risco frontal (AEB) → status[][0]
 * VelRelativa = VelAtual − VelFrente.
 * Se VelRelativa ≤ 0 (carro da frente igual ou mais rápido): status = 0 (Seguro).
 * Senão, compare validada × segura:
 *   validada ≥ segura              → 0 (Seguro)
 *   validada < segura e ≥ 50% dela → 1 (Atenção)
 *   validada < 50% da segura       → 2 (Risco de Colisão — AEB acionado)
 *
 * REGRA D — Assistente de faixa dinâmico → status[][1] (esquerda), status[][2] (direita)
 * Margem base = 0.50m. +0.01m para cada 1 km/h que a velocidade atual passar de 80.0 km/h.
 * Zona de atenção = margem dinâmica + 0.20m.
 *   leitura < margem dinâmica          → 2 (Perigo de Invasão)
 *   leitura < margem dinâmica + 0.20m  → 1 (Atenção)
 *   caso contrário                     → 0 (Normal)
 *
 * REGRA E — Relatório (única função com printf pós-cálculo)
 * Por amostra: dados de entrada (velocidades, sensores) + dados processados
 * (processamento) + tradução de status (0/1/2 → texto) + status geral da
 * linha (algum status = 2 → "INTERVENÇÃO CRÍTICA EXIGIDA"; senão algum = 1
 * → "ATENÇÃO"; senão → "NORMAL").
 *
 * PRAZOS (confirmados 22/09/2026, substituem o Plano de Aula — ver PROGRESS.md)
 * Apresentação: 24/09 (qui) · P1: 25/09 (sex) · Entrega final: 27/09 (dom)
 * ============================================================================
 */

int drawIntNumber (int min, int max) {
    if (min < max) {
        return min + rand() % (max - min + 1);
    }
    return 0;
}

double drawDoubleNumber (double min, double max) {
    if (min < max) {
        return min + ((double)rand() / RAND_MAX) * (max - min);
    }
    return 0.0;
}

void perguntasIniciais (double *atrito, int *sensibilidade) {
    do {
        printf("Qual é o atrito atual? (asfalto seco ≈ 0.7–0.8, chão molhado ≈ 0.4–0.5, gelo ≈ 0.1–0.2)\n");
        scanf("%lf", atrito);
    } while (*atrito < 0 || *atrito > 1);

    do {
        printf("Qual é sua velocidade de reação? (1-Esportivo, 2-Normal, 3-Seguro) \n");
        scanf("%d", sensibilidade);
    } while (*sensibilidade < 1 || *sensibilidade > 3);

}

int menuOptions () {
    int chosenOption = 0;
    do {
        printf("1. Carregar dados iniciais \n");
        printf("2. Inserir nova amostra \n");
        printf("3. Processar e exibir relatório \n");
        printf("4. Sair\n");
        scanf("%d", &chosenOption);
    } while (chosenOption < 1 || chosenOption > 4);

    return chosenOption;
}

void calculaMargemDinamica () {
    
}

void calculaZonaAtencao () {
    
}


void carregaDadosIniciais (double matrixA[][2], double matrixB[][3], double matrixC[][2], int *amostras) {
    *amostras = 50;
    
    for (int i = 0; i < *amostras; i++) {
        for (int j = 0; j < 3; j++) {
            if (j < 2) {
                matrixA[i][j] = drawDoubleNumber(5, 100);
                matrixB[i][j] = drawDoubleNumber(0, 100);
                matrixC[i][j] = drawDoubleNumber(0.1, 1);
            } else {
                matrixB[i][j] = drawDoubleNumber(0, 100);
            }
        }
    }
}

void insirirNovaAmostra (double matrixA[][2], double matrixB[][3], double matrixC[][2], int *amostras){
    double velocidadeAtual, velocidadeCarroDaFrente, radar, lidar, camera,
           distanciaFaixaEsquerda, distanciaFaixaDireita;

    printf("Velocidade atual (km/h): ");
    scanf("%lf", &velocidadeAtual);

    printf("Velocidade do veículo da frente (km/h): ");
    scanf("%lf", &velocidadeCarroDaFrente);

    printf("Leitura do radar (m): ");
    scanf("%lf", &radar);

    printf("Leitura do lidar (m): ");
    scanf("%lf", &lidar);

    printf("Leitura da câmera (m): ");
    scanf("%lf", &camera);

    printf("Distância da faixa esquerda (m): ");
    scanf("%lf", &distanciaFaixaEsquerda);

    printf("Distância da faixa direita (m): ");
    scanf("%lf", &distanciaFaixaDireita);

    matrixA[*amostras][0] = velocidadeAtual;
    matrixA[*amostras][1] = velocidadeCarroDaFrente;

    matrixB[*amostras][0] = radar;
    matrixB[*amostras][1] = lidar;
    matrixB[*amostras][2] = camera;

    matrixC[*amostras][0] = distanciaFaixaEsquerda;
    matrixC[*amostras][1] = distanciaFaixaDireita;

    *amostras = *amostras + 1;
}

void relatorioProcessarExebir () {
    printf("[Ainda não implementado]");
}

void delegateChoice (int chosenOption, double matrixA[][2], double matrixB[][3], double matrixC[][2], int *amostras) {
    switch (chosenOption) {
        case 1:
            carregaDadosIniciais(matrixA, matrixB, matrixC, amostras);
            break;
        case 2:
            insirirNovaAmostra(matrixA, matrixB, matrixC, amostras);
            break;
        case 3:
            relatorioProcessarExebir();
            break;
        case 4:
            printf("Saindo...\n");
            break;
    }
}


int main(void) {

    double atrito;
    int sensibilidade;

    double velocidades[MAX_AMOSTRAS][2];
    double sensoresFrontais[MAX_AMOSTRAS][3];
    double sensoresLaterais[MAX_AMOSTRAS][2];
    double processamento[MAX_AMOSTRAS][2];
    int status[MAX_AMOSTRAS][3];

    int totalAmostras = 0;
    int chosenOption;

    srand(time(NULL));

    perguntasIniciais(&atrito, &sensibilidade);

    do {
        chosenOption = menuOptions();
        delegateChoice(chosenOption, velocidades, sensoresFrontais, sensoresLaterais, &totalAmostras);
    } while (chosenOption != 4);

    return 0;
}
