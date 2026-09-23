#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_AMOSTRAS 100

/*
 * SAFEDRIVE — Processamento de telemetria ADAS (Projeto 1, AP2 2026.2)
 *
 * Integrantes do grupo:
 *   Nicolas Soares Santos — RA: 10370082
 *   Pedro Evangelisti     — RA: 10728508
 *
 * Organização (restrições do enunciado):
 * - Toda leitura (scanf) acontece na main.
 * - Funções de cálculo (Regras A-D) só recebem dados por parâmetro, sem
 *   scanf nem printf.
 * - Resultados só são impressos pela função de relatório (Regra E).
 * - Sem struct, alocação dinâmica, ponteiros explícitos ou variáveis
 *   globais: o total de amostras volta para a main pelo return.
 *
 * Resumo do enunciado, matrizes e Regras A-E: ver README.md
 */

/* ---------------------------------------------------------------------------
 * SORTEIO — gerador de números aleatórios
 * ------------------------------------------------------------------------- */

double drawDoubleNumber (double min, double max) {
  if (min < max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
  }
  return 0.0;
}

/* ---------------------------------------------------------------------------
 * DADOS DE ENTRADA — opção 1 (sorteio) e opção 2 (digitação)
 * ------------------------------------------------------------------------- */

// Preenche as 50 primeiras linhas com valores sorteados e devolve o novo total
int carregaDadosIniciais (double matrixA[][2], double matrixB[][3], double matrixC[][2]) {
  int amostras = 50;

  for (int i = 0; i < amostras; i++) {
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

  return amostras;
}

// Grava as 7 leituras (já lidas na main) na próxima linha vazia e devolve o novo total
int inserirNovaAmostra (double matrixA[][2], double matrixB[][3], double matrixC[][2], int amostras, double leitura[]) {
  matrixA[amostras][0] = leitura[0];  // velocidade atual
  matrixA[amostras][1] = leitura[1];  // velocidade do veículo da frente

  matrixB[amostras][0] = leitura[2];  // radar
  matrixB[amostras][1] = leitura[3];  // lidar
  matrixB[amostras][2] = leitura[4];  // câmera

  matrixC[amostras][0] = leitura[5];  // faixa esquerda
  matrixC[amostras][1] = leitura[6];  // faixa direita

  return amostras + 1;
}

/* ---------------------------------------------------------------------------
 * REGRAS DE PROCESSAMENTO — opção 3, rodam em ordem A → B → C → D → E
 * ------------------------------------------------------------------------- */

void medianaSensores (double matrixB[][3], double processamento[][2], int amostras) { // Regra A
  for (int i = 0; i < amostras; i++) {
    // Se o elemento 0 for o maior de todos - radar
    if (matrixB[i][0] > matrixB[i][1] && matrixB[i][0] > matrixB[i][2]) {
      // A mediana será o maior entre os dois restantes (1 e 2)
      if (matrixB[i][1] > matrixB[i][2]) {
        processamento[i][0] = matrixB[i][1];
      } else {
        processamento[i][0] = matrixB[i][2];
      }
    }
    // Se o elemento 1 for o maior de todos - lidar
    else if (matrixB[i][1] > matrixB[i][0] && matrixB[i][1] > matrixB[i][2]) {
      // A mediana será o maior entre os dois restantes (0 e 2)
      if (matrixB[i][0] > matrixB[i][2]) {
        processamento[i][0] = matrixB[i][0];
      } else {
        processamento[i][0] = matrixB[i][2];
      }
    }
    // Chegou aqui: ou a camera é a maior, ou radar e lidar empataram no topo
    else {
      // Nos dois casos, a mediana é o maior entre radar e lidar (0 e 1)
      if (matrixB[i][0] > matrixB[i][1]) {
        processamento[i][0] = matrixB[i][0];
      } else {
        processamento[i][0] = matrixB[i][1];
      }
    }
  }
}

void distanciaSeguraFrenagem (double matrixA[][2], double processamento[][2], double atrito, int sensibilidade, int amostras) { // Regra B
  double tempoReacao = 0.0, v = 0.0, distancia = 0.0;

  if (sensibilidade == 1) {
    tempoReacao = 1.0;
  } else if (sensibilidade == 2) {
    tempoReacao = 1.5;
  } else {
    tempoReacao = 2.0;
  }

  for (int i = 0; i < amostras; i++) {
    v = matrixA[i][0] / 3.6;
    distancia = v * tempoReacao + (v * v) / (2 * atrito * 9.81);
    processamento[i][1] = distancia;
  }
}

void riscoFrontal (double matrixA[][2], double processamento[][2], int status[][3], int amostras) { // Regra C
  for (int i = 0; i < amostras; i++) {
    double distValidada = processamento[i][0];            // metros que eu TENHO
    double distSegura   = processamento[i][1];            // metros que eu PRECISO
    double metadeSegura = 0.5 * distSegura;               // fronteira entre Atenção e Risco
    double velRelativa  = matrixA[i][0] - matrixA[i][1];  // km/h: estou me aproximando?

    if (velRelativa <= 0) {
      status[i][0] = 0;                 // Seguro: o da frente está igual ou mais rápido
    } else {
      if (distValidada >= distSegura) {
        status[i][0] = 0;             // Seguro: tenho o espaço que preciso
      } else if (distValidada >= metadeSegura) {
        status[i][0] = 1;             // Atenção: falta espaço, mas tenho ao menos metade
      } else {
        status[i][0] = 2;             // Risco de Colisão: nem metade do que preciso
      }
    }
  }
}

// Regra D — auxiliares
double calculaMargemDinamica (double velocidade) {
  if (velocidade > 80) {
    return 0.50 + 0.01 * (velocidade - 80);
  } else {
    return 0.50;
  }
}

double calculaZonaAtencao (double margemDinamica) {
  return margemDinamica + 0.20;
}

void assistenteFaixa (double matrixA[][2], double matrixC[][2], int status[][3], int amostras) { // Regra D
  for (int i = 0; i < amostras; i++) {
    double velocidade = matrixA[i][0];
    double margemDinamica = calculaMargemDinamica( velocidade );
    double zonaAtencao = calculaZonaAtencao(  margemDinamica );
    double leituraEsquerda = matrixC[i][0];
    double leituraDireita = matrixC[i][1];

    if (leituraEsquerda < margemDinamica) {
      status[i][1] = 2;
    } else if (leituraEsquerda < zonaAtencao) {
      status[i][1] = 1;
    } else {
      status[i][1] = 0;
    }

    if (leituraDireita < margemDinamica) {
      status[i][2] = 2;
    } else if (leituraDireita < zonaAtencao) {
      status[i][2] = 1;
    } else {
      status[i][2] = 0;
    }
  }

}

void relatorioProcessarExibir (double matrixA[][2], double matrixB[][3], double matrixC[][2], double processamento[][2], int status[][3], int amostras) { // Regra E
  if (amostras == 0) {
    printf("Nenhuma amostra carregada. Use a opção 1 ou 2 antes de processar.\n");
    return;
  }

  for (int i = 0; i < amostras; i++) {
    printf("\n--- Amostra %d ---\n", i + 1);

    // Dados de entrada
    printf("Velocidade atual: %.1f km/h | Carro da frente: %.1f km/h\n", matrixA[i][0], matrixA[i][1]);
    printf("Radar: %.1f m | Lidar: %.1f m | Câmera: %.1f m\n", matrixB[i][0], matrixB[i][1], matrixB[i][2]);
    printf("Faixa esquerda: %.2f m | Faixa direita: %.2f m\n", matrixC[i][0], matrixC[i][1]);

    // Dados processados (Regras A e B)
    printf("Distância validada: %.1f m | Distância segura exigida: %.1f m\n", processamento[i][0], processamento[i][1]);

    // Status frontal (Regra C)
    printf("Status frontal: ");
    if (status[i][0] == 2) {
      printf("RISCO DE COLISÃO (AEB ACIONADO)\n");
    } else if (status[i][0] == 1) {
      printf("ATENÇÃO\n");
    } else {
      printf("SEGURO\n");
    }

    // Status das faixas (Regra D) — j = 1 esquerda, j = 2 direita
    for (int j = 1; j <= 2; j++) {
      if (j == 1) {
        printf("Faixa esquerda: ");
      } else {
        printf("Faixa direita: ");
      }

      if (status[i][j] == 2) {
        printf("PERIGO DE INVASÃO\n");
      } else if (status[i][j] == 1) {
        printf("ATENÇÃO\n");
      } else {
        printf("NORMAL\n");
      }
    }

    // Status geral: o pior dos três status da linha
    int pior = status[i][0];
    if (status[i][1] > pior) {
      pior = status[i][1];
    }
    if (status[i][2] > pior) {
      pior = status[i][2];
    }

    if (pior == 2) {
      // Alerta máximo: o enunciado pede exibição em destaque
      printf("**************************************************\n");
      printf("STATUS GERAL: INTERVENÇÃO CRÍTICA EXIGIDA\n");
      printf("**************************************************\n");
    } else if (pior == 1) {
      printf("STATUS GERAL: ATENÇÃO\n");
    } else {
      printf("STATUS GERAL: NORMAL\n");
    }
  }
}

/* ---------------------------------------------------------------------------
 * MAIN — toda a leitura de dados e o laço do menu
 * ------------------------------------------------------------------------- */

int main(void) {
  double atrito;
  int sensibilidade;

  double velocidades[MAX_AMOSTRAS][2];
  double sensores_frontais[MAX_AMOSTRAS][3];
  double sensores_laterais[MAX_AMOSTRAS][2];
  double processamento[MAX_AMOSTRAS][2];
  int status[MAX_AMOSTRAS][3];

  int totalAmostras = 0;
  int opcao = 0;
  int lidos;     // retorno do scanf: quantos valores foram lidos
  int c;         // usado para descartar o que sobrou na linha após uma entrada inválida

  char perguntas[7][50] = {
    "Velocidade atual (km/h): ",
    "Velocidade do veículo da frente (km/h): ",
    "Leitura do radar (m): ",
    "Leitura do lidar (m): ",
    "Leitura da câmera (m): ",
    "Distância da faixa esquerda (m): ",
    "Distância da faixa direita (m): "
  };
  double leitura[7];

  srand(time(NULL));

  // Parâmetros iniciais. Se o scanf não conseguir ler um número, a linha é
  // descartada e a pergunta se repete (evita laço infinito com letras).
  do {
    printf("Qual é o atrito da via? (asfalto seco ≈ 0.7–0.8, chão molhado ≈ 0.4–0.5, gelo ≈ 0.1–0.2)\n");
    lidos = scanf("%lf", &atrito);
    if (lidos == EOF) {
      return 0;
    }
    if (lidos != 1) {
      while ((c = getchar()) != '\n' && c != EOF);
      atrito = -1;
    }
  } while (atrito <= 0 || atrito > 1);  // 0 dividiria por zero na Regra B

  do {
    printf("Qual é a sensibilidade do ADAS? (1-Esportivo, 2-Normal, 3-Seguro)\n");
    lidos = scanf("%d", &sensibilidade);
    if (lidos == EOF) {
      return 0;
    }
    if (lidos != 1) {
      while ((c = getchar()) != '\n' && c != EOF);
      sensibilidade = 0;
    }
  } while (sensibilidade < 1 || sensibilidade > 3);

  do {
    printf("\n1. Carregar dados iniciais\n");
    printf("2. Inserir nova amostra\n");
    printf("3. Processar e exibir relatório de riscos\n");
    printf("4. Sair\n");
    lidos = scanf("%d", &opcao);
    if (lidos == EOF) {
      return 0;
    }
    if (lidos != 1) {
      while ((c = getchar()) != '\n' && c != EOF);
      opcao = 0;
    }

    switch (opcao) {
      case 1:
        totalAmostras = carregaDadosIniciais(velocidades, sensores_frontais, sensores_laterais);
        printf("%d amostras carregadas.\n", totalAmostras);
        break;
      case 2:
        if (totalAmostras >= MAX_AMOSTRAS) {
          printf("Limite de %d amostras atingido. Nenhuma amostra foi inserida.\n", MAX_AMOSTRAS);
          break;
        }
        for (int k = 0; k < 7; k++) {
          do {
            printf("%s", perguntas[k]);
            lidos = scanf("%lf", &leitura[k]);
            if (lidos == EOF) {
              return 0;
            }
            if (lidos != 1) {
              while ((c = getchar()) != '\n' && c != EOF);
            }
          } while (lidos != 1);
        }
        totalAmostras = inserirNovaAmostra(velocidades, sensores_frontais, sensores_laterais, totalAmostras, leitura);
        break;
      case 3:
        medianaSensores(sensores_frontais, processamento, totalAmostras);
        distanciaSeguraFrenagem(velocidades, processamento, atrito, sensibilidade, totalAmostras);
        riscoFrontal(velocidades, processamento, status, totalAmostras);
        assistenteFaixa(velocidades, sensores_laterais, status, totalAmostras);
        relatorioProcessarExibir(velocidades, sensores_frontais, sensores_laterais, processamento, status, totalAmostras);
        break;
      case 4:
        printf("Saindo...\n");
        break;
      default:
        printf("Opção inválida. Escolha de 1 a 4.\n");
    }
  } while (opcao != 4);

  return 0;
}
