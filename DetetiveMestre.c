#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Definições de tamanho
#define MAX_NOME 50
#define MAX_PISTA 100
#define MAX_SUSPEITO 50
#define TAM_HASH 10 // Tamanho pequeno para demonstração

// -------------------------------------------------------------------
// 1. ESTRUTURAS DE DADOS
// -------------------------------------------------------------------

// Estrutura para o NÓ DA BST (Pistas Coletadas - Chave de busca: texto da pista)
typedef struct PistaBST {
    char texto[MAX_PISTA];
    // Novo: Armazena o nome do suspeito associado a esta pista
    char suspeito[MAX_SUSPEITO];
    struct PistaBST *esquerda;
    struct PistaBST *direita;
} PistaBST;

// Estrutura para o CÔMODO (Nó do Mapa da Mansão)
typedef struct Comodo {
    char nome[MAX_NOME];
    char pista[MAX_PISTA];
    char suspeito_associado[MAX_SUSPEITO]; // Novo: Quem a pista incrimina
    int pistaColetada;
    struct Comodo* esquerda;
    struct Comodo* direita;
} Comodo;

// Estrutura para o NÓ DA LISTA ENCADEADA na Tabela Hash (Item da Hash)
typedef struct NoHash {
    char suspeito[MAX_SUSPEITO];
    int contagem_pistas; // Número de pistas que incriminam este suspeito
    struct NoHash *proximo;
} NoHash;

// Tabela Hash (Array de ponteiros para listas encadeadas)
typedef struct TabelaHash {
    NoHash* buckets[TAM_HASH];
} TabelaHash;


// -------------------------------------------------------------------
// 2. FUNÇÕES DA TABELA HASH (ASSOCIAÇÃO SUSPEITO-PISTA)
// -------------------------------------------------------------------

// Função Hash: Simples, baseada na soma dos caracteres
unsigned int calcularHash(const char* chave) {
    unsigned int hash = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        hash = hash + (unsigned int)chave[i];
    }
    return hash % TAM_HASH;
}

// Inicializa a Tabela Hash
void inicializarHash(TabelaHash* tabela) {
    for (int i = 0; i < TAM_HASH; i++) {
        tabela->buckets[i] = NULL;
    }
}

// Incrementa a contagem de pistas para um suspeito na Tabela Hash
void incrementarContagemSuspeito(TabelaHash* tabela, const char* nomeSuspeito) {
    unsigned int indice = calcularHash(nomeSuspeito);
    NoHash* atual = tabela->buckets[indice];

    // 1. Procura o nó existente
    while (atual != NULL) {
        if (strcmp(atual->suspeito, nomeSuspeito) == 0) {
            atual->contagem_pistas++;
            printf("  [Hash]: Contagem de pistas para **%s** incrementada para %d.\n", nomeSuspeito, atual->contagem_pistas);
            return;
        }
        atual = atual->proximo;
    }

    // 2. Se não encontrou, cria um novo nó na lista encadeada (inserção no início)
    NoHash* novoNo = (NoHash*)malloc(sizeof(NoHash));
    if (novoNo == NULL) {
        perror("Erro na alocação de memória para NoHash");
        exit(EXIT_FAILURE);
    }

    strncpy(novoNo->suspeito, nomeSuspeito, MAX_SUSPEITO - 1);
    novoNo->suspeito[MAX_SUSPEITO - 1] = '\0';
    novoNo->contagem_pistas = 1; // Primeira pista

    // Insere no início da lista encadeada (bucket)
    novoNo->proximo = tabela->buckets[indice];
    tabela->buckets[indice] = novoNo;
    printf("  [Hash]: Novo suspeito **%s** adicionado à Hash com 1 pista.\n", nomeSuspeito);
}

// Retorna a contagem de pistas para um suspeito (ou 0 se não encontrado)
int obterContagemSuspeito(TabelaHash* tabela, const char* nomeSuspeito) {
    unsigned int indice = calcularHash(nomeSuspeito);
    NoHash* atual = tabela->buckets[indice];

    while (atual != NULL) {
        if (strcmp(atual->suspeito, nomeSuspeito) == 0) {
            return atual->contagem_pistas;
        }
        atual = atual->proximo;
    }
    return 0; // Suspeito não encontrado
}

// Libera a memória alocada para a Tabela Hash
void liberarHash(TabelaHash* tabela) {
    for (int i = 0; i < TAM_HASH; i++) {
        NoHash* atual = tabela->buckets[i];
        while (atual != NULL) {
            NoHash* proximo = atual->proximo;
            free(atual);
            atual = proximo;
        }
    }
}

// -------------------------------------------------------------------
// 3. FUNÇÕES DA BST (PISTAS)
// -------------------------------------------------------------------

// Cria um novo nó de pista para a BST
PistaBST* criarPistaBST(const char* texto, const char* suspeito) {
    PistaBST* novaPista = (PistaBST*)malloc(sizeof(PistaBST));
    if (novaPista == NULL) {
        perror("Erro na alocação de memória para PistaBST");
        exit(EXIT_FAILURE);
    }
    strncpy(novaPista->texto, texto, MAX_PISTA - 1);
    novaPista->texto[MAX_PISTA - 1] = '\0';
    strncpy(novaPista->suspeito, suspeito, MAX_SUSPEITO - 1);
    novaPista->suspeito[MAX_SUSPEITO - 1] = '\0';
    novaPista->esquerda = novaPista->direita = NULL;
    return novaPista;
}

// Insere uma pista na BST (organização alfabética pelo texto da pista)
PistaBST* inserirPistaBST(PistaBST* raiz, const char* texto, const char* suspeito) {
    if (raiz == NULL) {
        return criarPistaBST(texto, suspeito);
    }

    int comparacao = strcmp(texto, raiz->texto);

    if (comparacao < 0) {
        raiz->esquerda = inserirPistaBST(raiz->esquerda, texto, suspeito);
    } else if (comparacao > 0) {
        raiz->direita = inserirPistaBST(raiz->direita, texto, suspeito);
    }

    return raiz;
}

// Travessia In-Order para exibir as pistas em ordem alfabética
void exibirPistasEmOrdem(PistaBST* raiz) {
    if (raiz != NULL) {
        exibirPistasEmOrdem(raiz->esquerda);
        printf(" -> Pista: \"%s\" | Suspeito Associado: %s\n", raiz->texto, raiz->suspeito);
        exibirPistasEmOrdem(raiz->direita);
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
// 4. FUNÇÕES DO MAPA (ÁRVORE BINÁRIA)
// -------------------------------------------------------------------

// Cria um novo nó (cômodo) com pista e suspeito
Comodo* criarComodo(const char* nome, const char* pista, const char* suspeito) {
    Comodo* novoComodo = (Comodo*)malloc(sizeof(Comodo));
    if (novoComodo == NULL) {
        perror("Erro na alocação de memória para Comodo");
        exit(EXIT_FAILURE);
    }

    strncpy(novoComodo->nome, nome, MAX_NOME - 1);
    novoComodo->nome[MAX_NOME - 1] = '\0';

    strncpy(novoComodo->pista, pista, MAX_PISTA - 1);
    novoComodo->pista[MAX_PISTA - 1] = '\0';

    strncpy(novoComodo->suspeito_associado, suspeito, MAX_SUSPEITO - 1);
    novoComodo->suspeito_associado[MAX_SUSPEITO - 1] = '\0';

    novoComodo->pistaColetada = 0;
    novoComodo->esquerda = NULL;
    novoComodo->direita = NULL;

    return novoComodo;
}

// Monta o mapa da mansão com pistas e associações (montagem automática)
Comodo* montarMapa() {
    // Nível 0 - Raiz
    Comodo* hallEntrada = criarComodo("Hall de Entrada", "A porta principal estava trancada por dentro.", "Elias");

    // Nível 1
    Comodo* salaEstar = criarComodo("Sala de Estar", "Um bilhete rasgado menciona 'encontro na despensa'.", "Diana");
    Comodo* cozinha = criarComodo("Cozinha", "", ""); // Cômodo sem pista
    hallEntrada->esquerda = salaEstar;
    hallEntrada->direita = cozinha;

    // Nível 2
    Comodo* quartoPrincipal = criarComodo("Quarto Principal", "O diário menciona um relógio de ouro.", "Elias");
    Comodo* banheiro = criarComodo("Banheiro", "Uma luva de seda vermelha foi encontrada próxima ao lavabo.", "Bruno");
    salaEstar->esquerda = quartoPrincipal;
    salaEstar->direita = banheiro;

    Comodo* despensa = criarComodo("Despensa", "Uma lanterna quebrada e marcas de pés enlameados.", "Diana"); // Fim de caminho
    cozinha->esquerda = despensa;

    // Nível 3
    Comodo* varanda = criarComodo("Varanda", "O relógio de ouro estava caído no parapeito.", "Elias"); // Fim de caminho
    quartoPrincipal->esquerda = varanda;

    // A solução (culpado) é Elias, com 3 pistas (Hall, Quarto, Varanda).

    return hallEntrada;
}

// Libera a memória da árvore de cômodos (Mapa)
void liberarMapa(Comodo* comodo) {
    if (comodo == NULL) return;
    liberarMapa(comodo->esquerda);
    liberarMapa(comodo->direita);
    free(comodo);
}

// -------------------------------------------------------------------
// 5. SIMULAÇÃO DA EXPLORAÇÃO
// -------------------------------------------------------------------

void explorar(Comodo* atual, PistaBST** raiz_pistas, TabelaHash* hash_suspeitos) {
    if (atual == NULL) return;

    Comodo* proximo = NULL;
    char escolha;

    printf("\n🚨 Você é o detetive e precisa encontrar o culpado! 🚨\n");

    while (atual != NULL) {
        printf("\n========================================================\n");
        printf("--- LOCAL ATUAL: **%s** ---\n", atual->nome);

        // LÓGICA DE COLETA DE PISTAS E HASH
        if (strlen(atual->pista) > 0 && atual->pistaColetada == 0) {
            printf("\n🔎 **PISTA ENCONTRADA!**\n");

            // 1. Insere a pista na BST
            *raiz_pistas = inserirPistaBST(*raiz_pistas, atual->pista, atual->suspeito_associado);

            // 2. Associa a pista ao suspeito na Tabela Hash
            incrementarContagemSuspeito(hash_suspeitos, atual->suspeito_associado);

            // 3. Marca como coletada
            atual->pistaColetada = 1;
            printf("  [Sistema]: Pista incrimina **%s** e foi registrada.\n", atual->suspeito_associado);
        } else if (strlen(atual->pista) > 0 && atual->pistaColetada == 1) {
             printf("ℹ️ Pista já coletada neste cômodo.\n");
        } else {
             printf("O cômodo parece limpo. Nenhuma pista visível aqui.\n");
        }

        // Verifica se há caminhos disponíveis (folha da árvore)
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("\n🛑 **FIM DA LINHA!** A exploração da mansão terminou.\n");
            break;
        }

        // MOSTRA OPÇÕES
        printf("\nPara onde você quer ir? (E/D/F-Finalizar)\n");
        if (atual->esquerda != NULL) printf("   **[E]squerda** -> %s\n", atual->esquerda->nome);
        if (atual->direita != NULL) printf("   **[D]ireita** -> %s\n", atual->direita->nome);
        printf("   **[F]inalizar** -> Encerrar a exploração e fazer a acusação.\n");

        printf("Escolha: ");
        scanf(" %c", &escolha);
        escolha = toupper(escolha);

        proximo = NULL;

        switch (escolha) {
            case 'E':
                if (atual->esquerda != NULL) proximo = atual->esquerda;
                else printf("Caminho não existe.\n");
                break;
            case 'D':
                if (atual->direita != NULL) proximo = atual->direita;
                else printf("Caminho não existe.\n");
                break;
            case 'F':
                printf("\nExploração encerrada. Preparando a acusação...\n");
                return;
            default:
                printf("Opção inválida.\n");
                continue;
        }

        if (proximo != NULL) {
            atual = proximo; // Move para o próximo cômodo
        }
    }
}

// -------------------------------------------------------------------
// 6. AVALIAÇÃO FINAL
// -------------------------------------------------------------------

void avaliarAcusacao(TabelaHash* hash_suspeitos) {
    char acusado[MAX_SUSPEITO];
    int pistas_acusacao;

    printf("\n========================================================\n");
    printf("              🕵️ MOMENTO DA ACUSAÇÃO 🕵️             \n");
    printf("========================================================\n");

    printf("Quem você acusa? (Digite o nome do suspeito): ");
    // Lê a linha inteira para pegar nomes compostos, se houver
    scanf(" %49[^\n]", acusado);

    // Define o mínimo de pistas necessárias para uma acusação 'forte'
    const int PISTAS_MINIMAS = 3;

    // Consulta a Tabela Hash para obter a contagem de pistas
    pistas_acusacao = obterContagemSuspeito(hash_suspeitos, acusado);

    printf("\n--- ANÁLISE DO SISTEMA ---\n");
    printf("Acusado: **%s**\n", acusado);
    printf("Pistas Coletadas que o incriminam: **%d**\n", pistas_acusacao);

    if (pistas_acusacao >= PISTAS_MINIMAS) {
        printf("\n✅ **VEREDITO: ACUSAÇÃO SUSTENTÁVEL!**\n");
        printf("O número de %d pistas é suficiente para sustentar a acusação contra %s.\n", pistas_acusacao, acusado);
    } else if (pistas_acusacao > 0) {
        printf("\n⚠️ **VEREDITO: PROVAS INSUFICIENTES!**\n");
        printf("Você precisa de pelo menos %d pistas. Apenas %d foram encontradas contra %s.\n", PISTAS_MINIMAS, pistas_acusacao, acusado);
    } else {
        printf("\n❌ **VEREDITO: ACUSAÇÃO SEM BASE!**\n");
        printf("Nenhuma pista foi coletada que incrimine diretamente %s.\n", acusado);
    }
}

// -------------------------------------------------------------------
// 7. FUNÇÃO PRINCIPAL
// -------------------------------------------------------------------

int main() {
    PistaBST* pistas_coletadas = NULL;
    TabelaHash hash_suspeitos;

    // Inicializa a Tabela Hash antes do uso
    inicializarHash(&hash_suspeitos);

    printf("--- Simulador de Mansão e Resolução de Caso (Árvore + BST + Hash) ---\n");

    // 1. Monta o mapa (cria a árvore de cômodos)
    Comodo* mansao = montarMapa();

    // 2. Inicia a exploração, coleta de pistas e associação via Hash
    explorar(mansao, &pistas_coletadas, &hash_suspeitos);

    // 3. Avaliação final e acusação
    avaliarAcusacao(&hash_suspeitos);

    // 4. Exibe o relatório de pistas coletadas
    printf("\n========================================================\n");
    printf("           📋 RELATÓRIO COMPLETO DE INDÍCIOS 📋          \n");
    printf("========================================================\n");
    if (pistas_coletadas != NULL) {
        exibirPistasEmOrdem(pistas_coletadas);
    } else {
        printf("Nenhuma pista foi coletada.\n");
    }

    // 5. Libera a memória alocada
    printf("\n--- Fim da Simulação. Liberando memória ---\n");
    liberarPistas(pistas_coletadas);
    liberarMapa(mansao);
    liberarHash(&hash_suspeitos);

    return 0;
}
