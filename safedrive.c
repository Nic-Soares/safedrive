#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_AMOSTRAS 100

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


void carregaDadosIniciais () {
    printf("[Ainda não implementado]");
}

void insirirNovaAmostra (){
    printf("[Ainda não implementado]");
}

void relatorioProcessarExebir () {
    printf("[Ainda não implementado]");
}

void delegateChoice (int chosenOption) {
    switch (chosenOption) {
        case 1:
            carregaDadosIniciais();
            break;
        case 2:
            insirirNovaAmostra();
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

    float atrito;
    int sensibilidade;
    /* As 5 tabelas do programa - por enquanto so declaradas, vazias. */
    float velocidades[MAX_AMOSTRAS][2];
    float sensores_frontais[MAX_AMOSTRAS][3];
    float sensores_laterais[MAX_AMOSTRAS][2];
    float processamento[MAX_AMOSTRAS][2];
    int status[MAX_AMOSTRAS][3];

    int total_amostras = 0;
    int chosenOption;

    srand(time(NULL));

    perguntasIniciais(&atrito, &sensibilidade);

    do {
        chosenOption = menuOptions();
        delegateChoice(chosenOption);
    } while (chosenOption != 4);

    return 0;
}
