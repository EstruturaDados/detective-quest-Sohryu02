#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definindo o tamanho máximo para o nome do cômodo
#define MAX_NOME 50

// Estrutura para representar um cômodo (nó da árvore)
typedef struct Comodo {
    char nome[MAX_NOME];
    // Ponteiro para o "caminho da esquerda" (filho esquerdo)
    struct Comodo* esquerda;
    // Ponteiro para o "caminho da direita" (filho direito)
    struct Comodo* direita;
} Comodo;

// Função para criar um novo nó (cômodo)
Comodo* criarComodo(const char* nome) {
    Comodo* novoComodo = (Comodo*)malloc(sizeof(Comodo));
    if (novoComodo == NULL) {
        perror("Erro na alocação de memória");
        exit(EXIT_FAILURE);
    }

    // Copia o nome para o novo cômodo
    strncpy(novoComodo->nome, nome, MAX_NOME - 1);
    novoComodo->nome[MAX_NOME - 1] = '\0'; // Garantir terminação da string

    novoComodo->esquerda = NULL; // Inicialmente sem caminhos
    novoComodo->direita = NULL;  // Inicialmente sem caminhos

    return novoComodo;
}

// Função para montar o mapa da mansão (montagem automática)
Comodo* montarMapa() {
    // Nível 0 - Raiz
    Comodo* hallEntrada = criarComodo("Hall de Entrada");

    // Nível 1
    Comodo* salaEstar = criarComodo("Sala de Estar");
    Comodo* cozinha = criarComodo("Cozinha");
    hallEntrada->esquerda = salaEstar;
    hallEntrada->direita = cozinha;

    // Nível 2
    Comodo* quartoPrincipal = criarComodo("Quarto Principal");
    Comodo* banheiro = criarComodo("Banheiro");
    salaEstar->esquerda = quartoPrincipal;
    salaEstar->direita = banheiro;

    Comodo* despensa = criarComodo("Despensa"); // Fim de caminho
    cozinha->esquerda = despensa;
    // cozinha->direita = NULL; // Já é NULL por padrão (fim de caminho)

    // Nível 3
    Comodo* varanda = criarComodo("Varanda"); // Fim de caminho
    quartoPrincipal->esquerda = varanda;
    // quartoPrincipal->direita = NULL;

    // Fim do caminho: Banheiro não tem filhos. Despensa não tem filhos. Varanda não tem filhos.

    return hallEntrada; // Retorna a raiz da árvore
}

// Função principal para a exploração do jogador
void explorar(Comodo* atual) {
    if (atual == NULL) {
        printf("Você não está em nenhum lugar! Fim da exploração.\n");
        return;
    }

    Comodo* proximo = NULL;
    char escolha;

    while (atual != NULL) {
        printf("\n--- Local Atual: **%s** ---\n", atual->nome);

        // Verifica se há caminhos disponíveis (folha da árvore)
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("FIM DA LINHA! Este cômodo não tem mais caminhos (esquerda ou direita).\n");
            printf("Sua exploração termina aqui. Parabéns!\n");
            break; // Sai do loop principal
        }

        printf("Para onde você quer ir?\n");

        // Exibe as opções disponíveis
        if (atual->esquerda != NULL) {
            printf("   **[E]squerda** -> Vai para %s\n", atual->esquerda->nome);
        }
        if (atual->direita != NULL) {
            printf("   **[D]ireita** -> Vai para %s\n", atual->direita->nome);
        }
        printf("   **[S]air** -> Para terminar a simulação agora.\n");

        printf("Escolha: ");
        scanf(" %c", &escolha);
        escolha = toupper(escolha); // Para aceitar 'e' ou 'E'

        proximo = NULL;

        switch (escolha) {
            case 'E':
                if (atual->esquerda != NULL) {
                    proximo = atual->esquerda;
                } else {
                    printf("Não há caminho à esquerda. Tente outra opção.\n");
                }
                break;
            case 'D':
                if (atual->direita != NULL) {
                    proximo = atual->direita;
                } else {
                    printf("Não há caminho à direita. Tente outra opção.\n");
                }
                break;
            case 'S':
                printf("Exploração encerrada pelo jogador.\n");
                return;
            default:
                printf("Opção inválida. Por favor, escolha E, D ou S.\n");
                continue; // Volta ao início do loop
        }

        if (proximo != NULL) {
            atual = proximo; // Move para o próximo cômodo
        }
    }
}

// Função recursiva para liberar a memória da árvore
void liberarMapa(Comodo* comodo) {
    if (comodo == NULL) {
        return;
    }

    // Libera primeiro os filhos (pós-ordem)
    liberarMapa(comodo->esquerda);
    liberarMapa(comodo->direita);

    // Depois, libera o próprio nó
    printf("Liberando: %s\n", comodo->nome);
    free(comodo);
}

int main() {
    printf("--- Simulador de Mapa da Mansão (Árvore Binária) ---\n");

    // 1. Monta o mapa (cria a árvore)
    Comodo* mansao = montarMapa();

    // 2. Inicia a exploração
    explorar(mansao);

    // 3. Libera a memória alocada
    printf("\n--- Fim da Simulação. Liberando memória ---\n");
    liberarMapa(mansao);

    return 0;
}
