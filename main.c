#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LINHA_MAX 4096
#define TAM_HASH 2000003

typedef struct
{
    int NU_ANO;
    int CO_CURSO;
    int CO_IES;
    int CO_CATEGAD;
    int CO_ORGACAD;
    int CO_GRUPO;
    int CO_MODALIDADE;
    int CO_MUNIC_CURSO;
    int CO_UF_CURSO;
    int CO_REGIAO_CURSO;
} RegistroArq1;

typedef struct
{
    int CO_CURSO;
    int QUA_AL;    // Quantidade de Alunos
    float *NT_GER; // Nota geral
    float *NT_CE;  // Nota específica
    float somaGer; // Soma Geral
    float somaCe;  // Soma específica
    int CAP;       // Capacidade dos vetores

} RegistroArq3;

typedef struct
{
    int curso;         // chave
    RegistroArq3 info; // dados
    int usado;         // 0 = vazio; 1 = ocupado

} HashTab;

// Função para chave Hash
int hash(int x)
{
    return (x % TAM_HASH + TAM_HASH) % TAM_HASH;
}

// Funções de comparação para qsort
int compararPorCurso(const void *a, const void *b)
{
    return ((RegistroArq1 *)a)->CO_CURSO - ((RegistroArq1 *)b)->CO_CURSO;
}

int compararPorIES(const void *a, const void *b)
{
    return ((RegistroArq1 *)a)->CO_IES - ((RegistroArq1 *)b)->CO_IES;
}

int compararPorUF(const void *a, const void *b)
{
    return ((RegistroArq1 *)a)->CO_UF_CURSO - ((RegistroArq1 *)b)->CO_UF_CURSO;
}

int compararPorGrupo(const void *a, const void *b)
{
    return ((RegistroArq1 *)a)->CO_GRUPO - ((RegistroArq1 *)b)->CO_GRUPO;
}

float maiorGer(const RegistroArq3 *r)
{
    float m = -1.0f;
    for (int i = 0; i < r->QUA_AL; i++)
        if (r->NT_GER[i] > m)
            m = r->NT_GER[i];
    return m;
}

float maiorCe(const RegistroArq3 *r)
{
    float m = -1.0f;
    for (int i = 0; i < r->QUA_AL; i++)
        if (r->NT_CE[i] > m)
            m = r->NT_CE[i];
    return m;
}

float maiorNota(const RegistroArq3 *r)
{
    float mg = maiorGer(r);
    float mc = maiorCe(r);
    return (mg > mc ? mg : mc);
}

int cmpNtGerPtr(const void *a, const void *b)
{
    HashTab *x = *(HashTab**)a;
    HashTab *y = *(HashTab**)b;
    float ax = maiorGer(&x->info);
    float ay = maiorGer(&y->info);
    return (ay > ax) - (ay < ax);
}

int cmpNtCePtr(const void *a, const void *b)
{
    HashTab *x = *(HashTab**)a;
    HashTab *y = *(HashTab**)b;
    float ax = maiorCe(&x->info);
    float ay = maiorCe(&y->info);
    return (ay > ax) - (ay < ax);
}

int cmpMaiorNotaPtr(const void *a, const void *b)
{
    HashTab *x = *(HashTab**)a;
    HashTab *y = *(HashTab**)b;
    float ax = maiorNota(&x->info);
    float ay = maiorNota(&y->info);
    return (ay > ax) - (ay < ax);
}

RegistroArq1 *lerArq1(const char *nomeArquivo, int *tam)
{
    FILE *f = fopen(nomeArquivo, "r");
    if (!f)
    {
        printf("Erro ao abrir %s\n", nomeArquivo);
        exit(1);
    }

    RegistroArq1 *vet = NULL;
    char linha[LINHA_MAX];
    *tam = 0;

    fgets(linha, LINHA_MAX, f); // descarta cabeçalho

    while (fgets(linha, LINHA_MAX, f))
    {
        vet = realloc(vet, (*tam + 1) * sizeof(RegistroArq1));
        if (!vet)
        {
            printf("Erro no realloc\n");
            exit(1);
        }

        sscanf(linha, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
               &vet[*tam].NU_ANO,
               &vet[*tam].CO_CURSO,
               &vet[*tam].CO_IES,
               &vet[*tam].CO_CATEGAD,
               &vet[*tam].CO_ORGACAD,
               &vet[*tam].CO_GRUPO,
               &vet[*tam].CO_MODALIDADE,
               &vet[*tam].CO_MUNIC_CURSO,
               &vet[*tam].CO_UF_CURSO,
               &vet[*tam].CO_REGIAO_CURSO);

        (*tam)++;
    }

    fclose(f);
    printf("Arquivo %s lido com sucesso!\n", nomeArquivo);
    return vet;
}

void initCursoInfo(RegistroArq3 *c, int co_curso)
{
    c->CO_CURSO = co_curso;
    c->QUA_AL = 0;
    c->CAP = 10;
    c->somaGer = 0;
    c->somaCe = 0;
    c->NT_GER = malloc(c->CAP * sizeof(float));
    c->NT_CE = malloc(c->CAP * sizeof(float));
}

void inserirNotaTabela(int curso, float ntGer, float ntCe, HashTab *tabela)
{
    int h = hash(curso);

    while (tabela[h].usado && tabela[h].curso != curso)
    {
        h = (h + 1) % TAM_HASH;
    }

    // Primeiro registro do curso
    if (!tabela[h].usado)
    {
        tabela[h].usado = 1;
        tabela[h].curso = curso;
        initCursoInfo(&tabela[h].info, curso);
    }

    RegistroArq3 *ci = &tabela[h].info;

    // Aumentar capacidade se necessário
    if (ci->QUA_AL == ci->CAP)
    {
        ci->CAP *= 2;
        ci->NT_GER = realloc(ci->NT_GER, ci->CAP * sizeof(float));
        ci->NT_CE = realloc(ci->NT_CE, ci->CAP * sizeof(float));
    }

    // Adicionar notas
    ci->NT_GER[ci->QUA_AL] = ntGer;
    ci->NT_CE[ci->QUA_AL] = ntCe;

    // Atualizar estatísticas
    ci->somaGer += ntGer;
    ci->somaCe += ntCe;
    ci->QUA_AL++;
}

RegistroArq3 *lerArq3(const char *nomeArquivo, int *tam, HashTab *tabela)
{

    FILE *f = fopen(nomeArquivo, "r");

    if (!f)
    {
        printf("Erro ao abrir %s\n", nomeArquivo);
        exit(1);
    }

    int capacidade = 1000;
    RegistroArq3 *vet = malloc(capacidade * sizeof(RegistroArq3));
    char linha[LINHA_MAX];
    *tam = 0;

    fgets(linha, LINHA_MAX, f); // descarta cabeçalho

    while (fgets(linha, LINHA_MAX, f))
    {
        if (*tam >= capacidade)
        {
            capacidade *= 2;
            vet = realloc(vet, capacidade * sizeof(RegistroArq3));
        }
        if (!vet)
        {
            printf("Erro no realloc\n");
            exit(1);
        }

        char *token = strtok(linha, ";");
        int col = 1;

        int curso = 0;
        float ntGer = -1;
        float ntCe = -1;

        while (token != NULL)
        {

            if (col == 2)
                curso = atoi(token);
            if (col == 25)
                ntGer = atof(token);
            if (col == 32)
                ntCe = atof(token);

            col++;
            token = strtok(NULL, ";");
        }

        inserirNotaTabela(curso, ntGer, ntCe, tabela);

        (*tam)++;
    }

    fclose(f);

    printf("Arquivo %s lido com sucesso! %d registros carregados.\n",
           nomeArquivo, *tam);

    return vet;
}

// Busca binária por código de curso
int buscarCurso(RegistroArq1 *vet, int tam, int codigoCurso)
{

    int inicio = 0, fim = tam - 1;
    int encontrado = 0;

    while (inicio <= fim)
    {
        int meio = (inicio + fim) / 2;

        if (vet[meio].CO_CURSO == codigoCurso)
        {
            return meio;
            encontrado = 1;
            break;
        }
        else if (vet[meio].CO_CURSO < codigoCurso)
        {
            inicio = meio + 1;
        }
        else
        {
            fim = meio - 1;
        }
    }

    if (!encontrado)
    {
        return -1;
    }
}

int compararPorIESeCurso(const void *a, const void *b)
{
    int diffIES = ((RegistroArq1 *)a)->CO_IES - ((RegistroArq1 *)b)->CO_IES;
    if (diffIES != 0)
        return diffIES;
    return ((RegistroArq1 *)a)->CO_CURSO - ((RegistroArq1 *)b)->CO_CURSO;
}

// Função auxiliar para comparar por UF e depois por Curso
int compararPorUFeCurso(const void *a, const void *b)
{
    int diffUF = ((RegistroArq1 *)a)->CO_UF_CURSO - ((RegistroArq1 *)b)->CO_UF_CURSO;
    if (diffUF != 0)
        return diffUF;
    return ((RegistroArq1 *)a)->CO_CURSO - ((RegistroArq1 *)b)->CO_CURSO;
}

// Função auxiliar para comparar por Grupo e depois por Curso
int compararPorGrupoeCurso(const void *a, const void *b)
{
    int diffGrupo = ((RegistroArq1 *)a)->CO_GRUPO - ((RegistroArq1 *)b)->CO_GRUPO;
    if (diffGrupo != 0)
        return diffGrupo;
    return ((RegistroArq1 *)a)->CO_CURSO - ((RegistroArq1 *)b)->CO_CURSO;
}

void listarCursosPorIES(RegistroArq1 *vet, int tam, int codigoIES)
{
    // Ordenar por IES e depois por Curso
    qsort(vet, tam, sizeof(RegistroArq1), compararPorIESeCurso);

    printf("\n--- Cursos da IES %d (Busca Binaria) ---\n", codigoIES);
    int count = 0;

    // Busca binária para encontrar primeiro registro
    int inicio = 0, fim = tam - 1;
    int indiceInicial = -1;

    while (inicio <= fim)
    {
        int meio = (inicio + fim) / 2;
        if (vet[meio].CO_IES == codigoIES)
        {
            indiceInicial = meio;
            fim = meio - 1;
        }
        else if (vet[meio].CO_IES < codigoIES)
        {
            inicio = meio + 1;
        }
        else
        {
            fim = meio - 1;
        }
    }

    if (indiceInicial != -1)
    {
        int ultimoCurso = -1;
        for (int i = indiceInicial; i < tam && vet[i].CO_IES == codigoIES; i++)
        {
            if (vet[i].CO_CURSO != ultimoCurso)
            {
                printf("Curso: %d | Grupo: %d | UF: %d\n",
                       vet[i].CO_CURSO, vet[i].CO_GRUPO, vet[i].CO_UF_CURSO);
                ultimoCurso = vet[i].CO_CURSO;
                count++;
            }
        }
    }

    printf("Total de cursos unicos: %d\n", count);
    if (count == 0)
        printf("Nenhum curso encontrado para esta IES.\n");
}

void listarCursosPorUF(RegistroArq1 *vet, int tam, int codigoUF)
{
    // Ordenar por UF e depois por Curso
    qsort(vet, tam, sizeof(RegistroArq1), compararPorUFeCurso);

    printf("\n--- Cursos da UF %d (Busca Binaria) ---\n", codigoUF);
    int count = 0;

    // Busca binária para encontrar primeiro registro
    int inicio = 0, fim = tam - 1;
    int indiceInicial = -1;

    while (inicio <= fim)
    {
        int meio = (inicio + fim) / 2;
        if (vet[meio].CO_UF_CURSO == codigoUF)
        {
            indiceInicial = meio;
            fim = meio - 1;
        }
        else if (vet[meio].CO_UF_CURSO < codigoUF)
        {
            inicio = meio + 1;
        }
        else
        {
            fim = meio - 1;
        }
    }

    if (indiceInicial != -1)
    {
        int ultimoCurso = -1;
        for (int i = indiceInicial; i < tam && vet[i].CO_UF_CURSO == codigoUF; i++)
        {
            if (vet[i].CO_CURSO != ultimoCurso)
            {
                printf("Curso: %d | IES: %d | Grupo: %d\n",
                       vet[i].CO_CURSO, vet[i].CO_IES, vet[i].CO_GRUPO);
                ultimoCurso = vet[i].CO_CURSO;
                count++;
            }
        }
    }

    printf("Total de cursos unicos: %d\n", count);
    if (count == 0)
        printf("Nenhum curso encontrado para esta UF.\n");
}

void listarCursosPorGrupo(RegistroArq1 *vet, int tam, int codigoGrupo)
{
    // Ordenar por Grupo e depois por Curso
    qsort(vet, tam, sizeof(RegistroArq1), compararPorGrupoeCurso);

    printf("\n--- Cursos do Grupo (Area) %d (Busca Binaria) ---\n", codigoGrupo);
    int count = 0;

    // Busca binária para encontrar primeiro registro
    int inicio = 0, fim = tam - 1;
    int indiceInicial = -1;

    while (inicio <= fim)
    {
        int meio = (inicio + fim) / 2;
        if (vet[meio].CO_GRUPO == codigoGrupo)
        {
            indiceInicial = meio;
            fim = meio - 1;
        }
        else if (vet[meio].CO_GRUPO < codigoGrupo)
        {
            inicio = meio + 1;
        }
        else
        {
            fim = meio - 1;
        }
    }

    if (indiceInicial != -1)
    {
        int ultimoCurso = -1;
        for (int i = indiceInicial; i < tam && vet[i].CO_GRUPO == codigoGrupo; i++)
        {
            if (vet[i].CO_CURSO != ultimoCurso)
            {
                printf("Curso: %d | IES: %d | UF: %d\n",
                       vet[i].CO_CURSO, vet[i].CO_IES, vet[i].CO_UF_CURSO);
                ultimoCurso = vet[i].CO_CURSO;
                count++;
            }
        }
    }

    printf("Total de cursos unicos: %d\n", count);
    if (count == 0)
        printf("Nenhum curso encontrado para este Grupo.\n");
}

void imprimeResultadoArq1(int meio, RegistroArq1 *vet)
{
    printf("\n--- Curso Encontrado (Busca Binaria) ---\n");
    printf("Ano: %d\n", vet[meio].NU_ANO);
    printf("Codigo Curso: %d\n", vet[meio].CO_CURSO);
    printf("Codigo IES: %d\n", vet[meio].CO_IES);
    printf("Categoria Administrativa: %d\n", vet[meio].CO_CATEGAD);
    printf("Organizacao Academica: %d\n", vet[meio].CO_ORGACAD);
    printf("Grupo (Area): %d\n", vet[meio].CO_GRUPO);
    printf("Modalidade: %d\n", vet[meio].CO_MODALIDADE);
    printf("Municipio: %d\n", vet[meio].CO_MUNIC_CURSO);
    printf("UF: %d\n", vet[meio].CO_UF_CURSO);
    printf("Regiao: %d\n", vet[meio].CO_REGIAO_CURSO);
}

void opcaoArquivo1(RegistroArq1 *arq1, int tam1)
{
    int opcao;
    int codigo;

    do
    {
        printf("\n================= MENU =================\n");
        printf("1 - Buscar curso especifico\n");
        printf("2 - Listar cursos por IES\n");
        printf("3 - Listar cursos por UF\n");
        printf("4 - Listar cursos por Grupo (Area)\n");
        printf("0 - Sair\n");
        printf("Opcao: ");

        if (scanf("%d", &opcao) != 1)
        {
            // Limpa o buffer em caso de entrada inválida
            while (getchar() != '\n')
                ;
            printf("Entrada invalida!\n");
            continue;
        }

        switch (opcao)
        {
        case 1:
            printf("Digite o codigo do curso: ");
            if (scanf("%d", &codigo) == 1)
            {
                int indice = buscarCurso(arq1, tam1, codigo);
                // Ordenar por código de curso antes de buscar
                qsort(arq1, tam1, sizeof(RegistroArq1), compararPorCurso);
                if (indice != -1)
                    imprimeResultadoArq1(indice, arq1);
                else
                    printf("Curso com codigo %d nao encontrado.\n", codigo);
            }
            else
            {
                while (getchar() != '\n')
                    ;
                printf("Codigo invalido!\n");
            }
            break;
        case 2:
            printf("Digite o codigo da IES: ");
            if (scanf("%d", &codigo) == 1)
            {
                listarCursosPorIES(arq1, tam1, codigo);
            }
            else
            {
                while (getchar() != '\n')
                    ;
                printf("Codigo invalido!\n");
            }
            break;
        case 3:
            printf("Digite o codigo da UF: ");
            if (scanf("%d", &codigo) == 1)
            {
                listarCursosPorUF(arq1, tam1, codigo);
            }
            else
            {
                while (getchar() != '\n')
                    ;
                printf("Codigo invalido!\n");
            }
            break;
        case 4:
            printf("Digite o codigo do Grupo: ");
            if (scanf("%d", &codigo) == 1)
            {
                listarCursosPorGrupo(arq1, tam1, codigo);
            }
            else
            {
                while (getchar() != '\n')
                    ;
                printf("Codigo invalido!\n");
            }
            break;
        case 0:
            printf("Encerrando...\n");
            break;
        default:
            printf("Opcao invalida!\n");
        }
    } while (opcao != 0);
}

RegistroArq3 *buscarCursoHash(int curso, HashTab *tabela)
{
    int h = hash(curso);

    while (tabela[h].usado)
    {
        if (tabela[h].curso == curso)
            return &tabela[h].info;
        h = (h + 1) % TAM_HASH;
    }
    return NULL;
}

void listarMediasNotas(int codigo, RegistroArq3 *arq3, int tam3, HashTab *tabela)
{

    RegistroArq3 *ci = buscarCursoHash(codigo, tabela);

    if (!ci)
    {
        printf("Curso %d não encontrado.\n", codigo);
        return;
    }

    printf("\nNotas do curso %d:\n", codigo);
    for (int i = 0; i < ci->QUA_AL; i++)
    {
        if (!(ci->NT_GER[i] <= 0 || ci->NT_CE <= 0))
            printf("GER: %.2f | ESP: %.2f\n", ci->NT_GER[i], ci->NT_CE[i]);
    }

    printf("\n--- MÉDIAS DO CURSO %d ---\n", codigo);
    printf("Média Geral: %.2f\n", ci->somaGer / ci->QUA_AL);
    printf("Média Específica: %.2f\n", ci->somaCe / ci->QUA_AL);
}

void maioresNotas(int quantidade, HashTab *tabela, RegistroArq1 *arq1, int tam1)
{
    // Contar quantos cursos estão na tabela
    int count = 0;
    for (int i = 0; i < TAM_HASH; i++)
        if (tabela[i].usado)
            count++;

    if (count == 0)
    {
        printf("Tabela vazia.\n");
        return;
    }

    // --- Criar vetor de ponteiros ---
    HashTab **aux = malloc(count * sizeof(HashTab*));
    int j = 0;
    for (int i = 0; i < TAM_HASH; i++)
        if (tabela[i].usado)
            aux[j++] = &tabela[i];

    // Vetor de resultados (até 2*N cursos)
    HashTab **resultado = malloc(2 * quantidade * sizeof(HashTab*));
    int k = 0;

    // --- Ordenar por nota geral ---
    qsort(aux, count, sizeof(HashTab*), cmpNtGerPtr);
    for (int i = 0; i < quantidade && i < count; i++)
        resultado[k++] = aux[i];

    // --- Ordenar por nota específica ---
    qsort(aux, count, sizeof(HashTab*), cmpNtCePtr);
    for (int i = 0; i < quantidade && i < count; i++)
        resultado[k++] = aux[i];

    // --- Ordenar resultado pelo maior entre Geral/Específica ---
    qsort(resultado, k, sizeof(HashTab*), cmpMaiorNotaPtr);

    // --- Imprimir ---
    printf("\n--- TOP %d MELHORES CURSOS ---\n", quantidade);

    // Ordenar arq1 por CO_CURSO para busca binária
    qsort(arq1, tam1, sizeof(RegistroArq1), compararPorCurso);

    for (int i = 0; i < quantidade && i < k; i++)
    {
        float nota = maiorNota(&resultado[i]->info);
        int idx = buscarCurso(arq1, tam1, resultado[i]->info.CO_CURSO);
        int ies = (idx != -1) ? arq1[idx].CO_IES : -1;
        printf("Curso %d | Maior Nota: %.2f | IES: %d\n",
               resultado[i]->info.CO_CURSO, nota, ies);
    }

    free(aux);
    free(resultado);
}


void opcaoArquivo3(RegistroArq3 *arq3, int tam3, HashTab *tabela, RegistroArq1 *arq1, int tam1)
{
    int opcao;
    int codigo;
    int quantidade;
    do
    {
        printf("\n================= MENU =================\n");
        printf("1 - Listar Notas e Médias de um Curso específico\n");
        printf("2 - Mostrar Maiores Notas do ENADE (Geral ou Específica)\n");
        printf("0 - Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);
        switch (opcao)
        {
        case 1:
            printf("Digite o Código do Curso que Deseja Verificar! \n");
            scanf("%d", &codigo);

            listarMediasNotas(codigo, arq3, tam3, tabela);
            break;
        case 2:
            printf("Quantas notas deseja mostrar? \n");
            scanf("%d", &quantidade);

            maioresNotas(quantidade, tabela, arq1, tam1);
            break;
        case 0:
            printf("Encerrando...\n");
            break;
        default:
            printf("Opcao invalida!\n");
            break;
        }
    } while (opcao != 0);
}

void inicializaHash(HashTab *tabela)
{
    for (int i = 0; i < TAM_HASH; i++)
        tabela[i].usado = 0;
}

int main(void)
{
    int tam1, tam3;
    HashTab *tabela = malloc(TAM_HASH * sizeof(HashTab));
    inicializaHash(tabela);

    printf("================= LEITURA DOS ARQUIVOS =================\n");
    RegistroArq1 *arq1 = lerArq1("microdados2023_arq1.txt", &tam1);
    RegistroArq3 *arq3 = lerArq3("microdados2023_arq3.txt", &tam3, tabela);

    printf("Registros arq1: %d\n", tam1);
    printf("Registros arq3: %d\n", tam3);

    int numArquivo = 0;

    printf("Em qual arquivo quer mexer? \n 1 - arq1.txt \n 2 - arq3.txt \n");

    while (numArquivo != 1 && numArquivo != 2)
    {
        scanf("%d", &numArquivo);
        if (numArquivo != 1 && numArquivo != 2)
        {
            printf("Comando Inexistente \n");
        }
    }

    if (numArquivo == 1)
    {
        opcaoArquivo1(arq1, tam1);
    }
    else
    {
        opcaoArquivo3(arq3, tam3, tabela, arq1, tam1);
    }

    free(arq1);
    free(arq3);

    return 0;
}
