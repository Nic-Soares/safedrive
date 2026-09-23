# Safedrive: processamento de telemetria ADAS

Projeto 1 de Algoritmos e Programação II (2026.2).

Este arquivo resume o enunciado, com só o essencial, para não precisar abrir o PDF toda hora.
Fonte completa: `Projeto 1 — Enunciado (AlgProII-2026.2 Projeto1).pdf` (no Vault).

## Restrições técnicas

- Proibido: `struct`, `malloc`/`realloc` e variável global.
- O enunciado também cita "ponteiros explícitos" como proibidos.
- Isolamento de E/S (texto literal do PDF): as funções de cálculo não podem ter `scanf` nem `printf`, e todos os dados chegam a elas por parâmetro. Só a função de relatório pode ter `printf`, e **a leitura de dados acontece exclusivamente na `main`**. A `main` também pede o atrito e a sensibilidade logo no início.
  - No código: todo `scanf` e todo prompt ficam na `main`, e os resultados só são impressos em `relatorioProcessarExibir`. O total de amostras volta para a `main` pelo `return` de `carregaDadosIniciais`/`inserirNovaAmostra`, então nenhuma função precisa de ponteiro.

## Dados

`MAX_AMOSTRAS = 100` linhas por matriz. A opção 1 preenche as 50 primeiras.

| matriz | tipo | coluna 0 | coluna 1 | coluna 2 |
|---|---|---|---|---|
| `velocidades` | `double [][2]` | velocidade atual (km/h) | velocidade do carro da frente (km/h) | |
| `sensoresFrontais` | `double [][3]` | radar (m) | lidar (m) | câmera (m) |
| `sensoresLaterais` | `double [][2]` | distância da faixa esquerda (m) | distância da faixa direita (m) | |
| `processamento` | `double [][2]` | distância validada (Regra A) | distância segura (Regra B) | |
| `status` | `int [][3]` | status frontal (Regra C) | faixa esquerda (Regra D) | faixa direita (Regra D) |

### Visão das matrizes

A linha `i` é a mesma amostra em todas as matrizes. Os valores abaixo são de exemplo, e as linhas de `totalAmostras` até 99 ficam vazias.

```
           velocidades         sensoresFrontais            sensoresLaterais
         +-------+-------+   +-------+-------+-------+   +-------+-------+
         | atual | frente|   | radar | lidar | camera|   |  esq  |  dir  |
         |  [0]  |  [1]  |   |  [0]  |  [1]  |  [2]  |   |  [0]  |  [1]  |
         +-------+-------+   +-------+-------+-------+   +-------+-------+
 i = 0   |  90.0 |  70.0 |   |  42.0 |   7.5 |  19.3 |   |  0.45 |  0.80 |
 i = 1   |  60.0 |  65.0 |   |  30.1 |  31.0 |  29.8 |   |  0.95 |  0.30 |
  ...    |  ...  |  ...  |   |  ...  |  ...  |  ...  |   |  ...  |  ...  |
         +-------+-------+   +-------+-------+-------+   +-------+-------+
              ENTRADA (dados crus: sorteio na opção 1, digitação na opção 2)

                            processamento                   status (int)
                      +-----------+-----------+   +-------+-------+-------+
                      | validada  |  segura   |   |frontal|  esq  |  dir  |
                      |    [0]    |    [1]    |   |  [0]  |  [1]  |  [2]  |
                      +-----------+-----------+   +-------+-------+-------+
 i = 0                |  Regra A  |  Regra B  |   |   C   |   D   |   D   |
 i = 1                |    ...    |    ...    |   |  ...  |  ...  |  ...  |
                      +-----------+-----------+   +-------+-------+-------+
                           SAÍDA (preenchida pelas Regras, na opção 3)
```

### Fluxo de uma linha `i`

```
sensoresFrontais[i][0..2]                      --A: mediana--> processamento[i][0]
velocidades[i][0] + atrito + sensibilidade     --B-->          processamento[i][1]
velocidades[i][0..1] + processamento[i][0..1]  --C-->          status[i][0]
velocidades[i][0] + sensoresLaterais[i][0..1]  --D-->          status[i][1..2]
tudo acima                                     --E: printf-->  relatório
```

## Menu

A `main` repete o menu até o usuário sair.

1. **Carregar dados iniciais:** gera 50 registros aleatórios.
2. **Inserir nova amostra:** lê as 2 velocidades e as 5 leituras de sensores e grava tudo na próxima linha vazia.
3. **Processar e exibir relatório:** roda as Regras A a E, nesta ordem.
4. **Sair.**

## Regras

### Regra A: fusão de sensores → `processamento[][0]`

A distância validada é a mediana das 3 leituras de `sensoresFrontais`.

### Regra B: distância segura de frenagem → `processamento[][1]`

O tempo de reação depende da sensibilidade: 1 = 1.0 s, 2 = 1.5 s, 3 = 2.0 s.

Antes do cálculo, a velocidade é convertida de km/h para m/s (÷ 3.6):

```
distância = v × tempoReacao + v² / (2 × atrito × 9.81)
```

### Regra C: risco frontal (AEB) → `status[][0]`

```
velRelativa = velAtual − velFrente
```

- Se `velRelativa ≤ 0`, o carro da frente está na mesma velocidade ou mais rápido: **0 (Seguro)**.
- Caso contrário, compare a distância validada com a distância segura:

| condição | status |
|---|---|
| validada ≥ segura | 0 (Seguro) |
| validada < segura e ≥ 50% dela | 1 (Atenção) |
| validada < 50% da segura | 2 (Risco de Colisão, AEB acionado) |

### Regra D: assistente de faixa dinâmico → `status[][1]` (esquerda) e `status[][2]` (direita)

- A margem base é 0.50 m. Ela aumenta 0.01 m para cada 1 km/h que a velocidade atual passar de 80.0 km/h. O resultado é a margem dinâmica.
- A zona de atenção é a margem dinâmica + 0.20 m.

| condição | status |
|---|---|
| leitura < margem dinâmica | 2 (Perigo de Invasão) |
| leitura < zona de atenção | 1 (Atenção) |
| caso contrário | 0 (Normal) |

### Regra E: relatório

Esta é a única função que imprime resultados. Para cada amostra, ela mostra:

- os dados de entrada (velocidades e sensores);
- os dados processados (`processamento`);
- a tradução de cada status (0/1/2 → texto);
- o status geral da linha:
  - algum status = 2 → `INTERVENÇÃO CRÍTICA EXIGIDA`;
  - senão, algum = 1 → `ATENÇÃO`;
  - senão → `NORMAL`.

## Avaliação (v2 do enunciado, 23/09/2026)

| item | pontos |
|---|---|
| Estrutura, matrizes, restrições e separação E/P/S | 1,0 |
| Entrada, inicialização e menu | 1,5 |
| Processamento (Regras A a D, 1,0 cada) | 4,0 |
| Relatório | 1,5 |
| Apresentação (domínio, demonstração, respostas) | 2,0 |

Penalidades: não compilar ou **travar** → zero; sem identificação do grupo no `.c` → −1; binários na entrega → −1; formato incorreto → −1. A entrega é um `.zip` só com os `.c`, enviado pelo Moodle por uma pessoa do grupo, e o grupo tem de 2 a 3 pessoas.

## Prazos

Confirmados em 22/09/2026. Estas datas substituem as do Plano de Aula.

- Apresentação: 24/09 (qui), turma 02P11
- P1: 25/09 (sex)
- Entrega final: 27/09 (dom)
