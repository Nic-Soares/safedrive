#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_AMOSTRAS 100

void perguntasIniciais (float *atrito, int *sensibilidade) {
    do {
        printf("Qual é o atrito atual? (asfalto seco ≈ 0.7–0.8, chão molhado ≈ 0.4–0.5, gelo ≈ 0.1–0.2)\n");
        scanf("%f", atrito);
    } while (*atrito < 0 || *atrito > 1);

    do {
        printf("Qual é sua velocidade de reação? (1-Esportivo, 2-Normal, 3-Seguro) \n");
        scanf("%d", sensibilidade);
    } while (*sensibilidade < 1 || *sensibilidade > 3);

}

int main(void) {

    float atrito;
    int sensibilidade;
    /* As 5 tabelas do programa - por enquanto so declaradas, vazias. */
    float velocidades[MAX_AMOSTRAS][2];
    float sensores_frontais[MAX_AMOSTRAS][3];
    float sensores_laterais[MAX_AMOSTRAS][2];
    float processamento[MAX_AMOSTRAS][2];
    int status[MAX_AMOSTRAS][3];

    int total_amostras = 0;

    perguntasIniciais(&atrito, &sensibilidade);

    return 0;
}
