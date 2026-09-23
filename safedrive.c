#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_AMOSTRAS 100

/*
 * SAFEDRIVE — Processamento de telemetria ADAS (Projeto 1, AP2)
 * Resumo do enunciado, matrizes e Regras A-E: ver ENUNCIADO.md
 */

/* ---------------------------------------------------------------------------
 * SORTEIO — geradores de números aleatórios
 * ------------------------------------------------------------------------- */

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

/* ---------------------------------------------------------------------------
 * ENTRADA INTERATIVA — parâmetros iniciais e menu
 * ------------------------------------------------------------------------- */

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

/* ---------------------------------------------------------------------------
 * DADOS DE ENTRADA — opção 1 (sorteio) e opção 2 (digitação)
 * ------------------------------------------------------------------------- */

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

void relatorioProcessarExibir (double matrixB[][3], double processamento[][2], int amostras) { // Regra E
  printf("[Relatório ainda não implementado]\n");
}

/* ---------------------------------------------------------------------------
 * CONTROLE — despacho do menu e main
 * ------------------------------------------------------------------------- */

void delegateChoice (int chosenOption, double matrixA[][2], double matrixB[][3], double matrixC[][2], double processamento[][2], int status[][3], int *amostras, double atrito, int sensibilidade) {
  switch (chosenOption) {
    case 1:
      carregaDadosIniciais(matrixA, matrixB, matrixC, amostras);
      break;
    case 2:
      insirirNovaAmostra(matrixA, matrixB, matrixC, amostras);
      break;
    case 3:
      medianaSensores(matrixB, processamento, *amostras);
      distanciaSeguraFrenagem(matrixA, processamento, atrito, sensibilidade, *amostras);
      riscoFrontal(matrixA, processamento, status, *amostras);
      assistenteFaixa(matrixA, matrixC, status, *amostras);
      relatorioProcessarExibir(matrixB, processamento, *amostras);
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
    delegateChoice(chosenOption, velocidades, sensoresFrontais, sensoresLaterais, processamento, status, &totalAmostras, atrito, sensibilidade);
  } while (chosenOption != 4);

  return 0;
}
