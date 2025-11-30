#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definições de tamanho
#define MAX_NOME 50
#define MAX_PISTA 100

// -------------------------------------------------------------------
// 1. ESTRUTURAS DE DADOS
// -------------------------------------------------------------------

// Estrutura para o NÓ DA BST (Armazena as Pistas Coletadas)
typedef struct PistaBST {
    char texto[MAX_PISTA];
    struct PistaBST *esquerda;
    struct PistaBST *direita;
} PistaBST;

// Estrutura para o CÔMODO (Nó do Mapa da Mansão)
typedef struct Comodo {
    char nome[MAX_NOME];
    char pista[MAX_PISTA];      // Nova: Pista associada a este cômodo
    int pistaColetada;          // Nova: Flag para saber se a pista já foi coletada (0=Não, 1=Sim)
    struct Comodo* esquerda;
    struct Comodo* direita;
} Comodo;

// -------------------------------------------------------------------
// 2. FUNÇÕES DA ÁRVORE BST (PISTAS)
// -------------------------------------------------------------------

// Cria um novo nó de pista para a BST
PistaBST* criarPistaBST(const char* texto) {
    PistaBST* novaPista = (PistaBST*)malloc(sizeof(PistaBST));
    if (novaPista == NULL) {
        perror("Erro na alocação de memória para PistaBST");
        exit(EXIT_FAILURE);
    }
    strncpy(novaPista->texto, texto, MAX_PISTA - 1);
    novaPista->texto[MAX_PISTA - 1] = '\0';
    novaPista->esquerda = novaPista->direita = NULL;
    return novaPista;
}

// Insere uma pista na BST de forma recursiva (organização alfabética)
PistaBST* inserirPistaBST(PistaBST* raiz, const char* texto) {
    // Caso base: Árvore vazia ou chegou a um ponto de inserção
    if (raiz == NULL) {
        printf("  [Sistema de Pistas]: Pista coletada e adicionada: '%s'\n", texto);
        return criarPistaBST(texto);
    }

    // Compara as strings para decidir a direção (strcmp retorna <0 se texto < raiz, >0 se texto > raiz)
    int comparacao = strcmp(texto, raiz->texto);

    if (comparacao < 0) {
        // Vai para a subárvore esquerda (texto alfabeticamente menor)
        raiz->esquerda = inserirPistaBST(raiz->esquerda, texto);
    } else if (comparacao > 0) {
        // Vai para a subárvore direita (texto alfabeticamente maior)
        raiz->direita = inserirPistaBST(raiz->direita, texto);
    } else {
        // Pista duplicada (ignora)
        printf("  [Sistema de Pistas]: Pista '%s' já havia sido coletada.\n", texto);
    }

    return raiz;
}

// Travessia In-Order para exibir as pistas em ordem alfabética
void exibirPistasEmOrdem(PistaBST* raiz) {
    if (raiz != NULL) {
        exibirPistasEmOrdem(raiz->esquerda); // Visita a esquerda (menores)
        printf(" -> %s\n", raiz->texto);     // Processa o nó (raiz)
        exibirPistasEmOrdem(raiz->direita);  // Visita a direita (maiores)
    }
}

// Libera a memória da BST de Pistas
void liberarPistas(PistaBST* raiz) {
    if (raiz != NULL) {
        liberarPistas(raiz->esquerda);
        liberarPistas(raiz->direita);
        free(raiz);
    }
}

// -------------------------------------------------------------------
// 3. FUNÇÕES DO MAPA DA MANSÃO (ÁRVORE BINÁRIA)
// -------------------------------------------------------------------

// Cria um novo nó (cômodo) com pista e flag de coleta
Comodo* criarComodo(const char* nome, const char* pista) {
    Comodo* novoComodo = (Comodo*)malloc(sizeof(Comodo));
    if (novoComodo == NULL) {
        perror("Erro na alocação de memória para Comodo");
        exit(EXIT_FAILURE);
    }

    // Inicialização do nome
    strncpy(novoComodo->nome, nome, MAX_NOME - 1);
    novoComodo->nome[MAX_NOME - 1] = '\0';

    // Inicialização da pista
    strncpy(novoComodo->pista, pista, MAX_PISTA - 1);
    novoComodo->pista[MAX_PISTA - 1] = '\0';

    novoComodo->pistaColetada = 0; // Pista não coletada inicialmente
    novoComodo->esquerda = NULL;
    novoComodo->direita = NULL;

    return novoComodo;
}

// Monta o mapa da mansão com pistas (montagem automática)
Comodo* montarMapa() {
    // Nível 0 - Raiz
    Comodo* hallEntrada = criarComodo("Hall de Entrada", "A porta principal estava trancada por dentro.");

    // Nível 1
    Comodo* salaEstar = criarComodo("Sala de Estar", "Um bilhete rasgado menciona 'encontro na despensa'.");
    Comodo* cozinha = criarComodo("Cozinha", ""); // Cômodo sem pista
    hallEntrada->esquerda = salaEstar;
    hallEntrada->direita = cozinha;

    // Nível 2
    Comodo* quartoPrincipal = criarComodo("Quarto Principal", "O diário menciona um relógio de ouro.");
    Comodo* banheiro = criarComodo("Banheiro", "Uma luva de seda vermelha foi encontrada próxima ao lavabo.");
    salaEstar->esquerda = quartoPrincipal;
    salaEstar->direita = banheiro;

    Comodo* despensa = criarComodo("Despensa", "Uma lanterna quebrada e marcas de pés enlameados."); // Fim de caminho
    cozinha->esquerda = despensa;

    // Nível 3
    Comodo* varanda = criarComodo("Varanda", "O relógio de ouro estava caído no parapeito."); // Fim de caminho
    quartoPrincipal->esquerda = varanda;

    // Fim dos caminhos: Banheiro, Despensa e Varanda não têm filhos.

    return hallEntrada; // Retorna a raiz da árvore
}

// Libera a memória da árvore de cômodos (Mapa)
void liberarMapa(Comodo* comodo) {
    if (comodo == NULL) {
        return;
    }
    liberarMapa(comodo->esquerda);
    liberarMapa(comodo->direita);
    // printf("Liberando cômodo: %s\n", comodo->nome); // Opcional
    free(comodo);
}

// -------------------------------------------------------------------
// 4. SIMULAÇÃO DA EXPLORAÇÃO
// -------------------------------------------------------------------

// Função principal para a exploração do jogador
void explorar(Comodo* atual, PistaBST** raiz_pistas) {
    if (atual == NULL) {
        printf("Início da exploração inválido.\n");
        return;
    }

    Comodo* proximo = NULL;
    char escolha;

    printf("\n Você é o detetive e precisa encontrar todos os indícios! \n");

    while (atual != NULL) {
        printf("\n========================================================\n");
        printf("--- LOCAL ATUAL: **%s** ---\n", atual->nome);

        // LÓGICA DE COLETA DE PISTAS
        if (strlen(atual->pista) > 0 && atual->pistaColetada == 0) {
            printf("\n **PISTA ENCONTRADA!**\n");
            // Insere a pista na BST para organização
            *raiz_pistas = inserirPistaBST(*raiz_pistas, atual->pista);
            // Marca como coletada
            atual->pistaColetada = 1;
        } else if (strlen(atual->pista) > 0 && atual->pistaColetada == 1) {
             printf(" Você já coletou a pista deste cômodo.\n");
        } else {
             printf("O cômodo parece limpo. Nenhuma pista visível aqui.\n");
        }

        // Verifica se há caminhos disponíveis (folha da árvore)
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("\n **FIM DA LINHA!** Este cômodo não tem mais caminhos (esquerda ou direita).\n");
            printf("A exploração da mansão termina aqui.\n");
            break;
        }

        // MOSTRA OPÇÕES
        printf("\nPara onde você quer ir?\n");

        if (atual->esquerda != NULL) {
            printf("   **[E]squerda** -> Vai para %s\n", atual->esquerda->nome);
        }
        if (atual->direita != NULL) {
            printf("   **[D]ireita** -> Vai para %s\n", atual->direita->nome);
        }
        printf("   **[S]air** -> Para encerrar a exploração.\n");

        printf("Escolha: ");
        scanf(" %c", &escolha);
        escolha = toupper(escolha);

        proximo = NULL;

        switch (escolha) {
            case 'E':
                if (atual->esquerda != NULL) {
                    proximo = atual->esquerda;
                } else {
                    printf("Caminho não existe. Escolha outra direção.\n");
                }
                break;
            case 'D':
                if (atual->direita != NULL) {
                    proximo = atual->direita;
                } else {
                    printf("Caminho não existe. Escolha outra direção.\n");
                }
                break;
            case 'S':
                printf("\nExploração encerrada pelo detetive. Voltando para analisar os indícios.\n");
                return;
            default:
                printf("Opção inválida. Por favor, escolha E, D ou S.\n");
                continue;
        }

        if (proximo != NULL) {
            atual = proximo; // Move para o próximo cômodo
        }
    }
}

// -------------------------------------------------------------------
// 5. FUNÇÃO PRINCIPAL
// -------------------------------------------------------------------

int main() {
    // Ponteiro para a raiz da BST de Pistas.
    // Começa como NULL (vazia) e será atualizada pela função inserirPistaBST.
    PistaBST* pistas_coletadas = NULL;

    printf("--- Simulador de Mapa da Mansão (Árvore Binária) e Coleta de Pistas (BST) ---\n");

    // 1. Monta o mapa (cria a árvore de cômodos)
    Comodo* mansao = montarMapa();

    // 2. Inicia a exploração e coleta de pistas
    explorar(mansao, &pistas_coletadas);

    // 3. Exibe o resultado final das pistas coletadas e organizadas
    printf("\n========================================================\n");
    printf("           📋 RELATÓRIO DE PISTAS COLETADAS 📋          \n");
    printf("========================================================\n");

    if (pistas_coletadas != NULL) {
        printf("As pistas estão organizadas em ordem alfabética (via BST In-Order):\n");
        exibirPistasEmOrdem(pistas_coletadas);
    } else {
        printf("Nenhuma pista foi coletada durante a exploração.\n");
    }

    printf("\n--- Fim da Simulação. Liberando memória ---\n");
    // 4. Libera a memória alocada para ambas as estruturas
    liberarPistas(pistas_coletadas);
    liberarMapa(mansao);

    return 0;
}
