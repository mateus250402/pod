#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LINHA_MAX 4096

typedef struct {
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

typedef struct {
    int NU_ANO;
    int CO_CURSO;
    float NT_GER;  // Nota geral
    float NT_CE;   // Nota específica
} RegistroArq3;

// Funções de comparação para qsort
int compararPorCurso(const void* a, const void* b) {
    return ((RegistroArq1*)a)->CO_CURSO - ((RegistroArq1*)b)->CO_CURSO;
}

int compararPorIES(const void* a, const void* b) {
    return ((RegistroArq1*)a)->CO_IES - ((RegistroArq1*)b)->CO_IES;
}

int compararPorUF(const void* a, const void* b) {
    return ((RegistroArq1*)a)->CO_UF_CURSO - ((RegistroArq1*)b)->CO_UF_CURSO;
}

int compararPorGrupo(const void* a, const void* b) {
    return ((RegistroArq1*)a)->CO_GRUPO - ((RegistroArq1*)b)->CO_GRUPO;
}

RegistroArq1* lerArq1(const char* nomeArquivo, int* tam) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) {
        printf("Erro ao abrir %s\n", nomeArquivo);
        exit(1);
    }

    RegistroArq1 *vet = NULL;
    char linha[LINHA_MAX];
    *tam = 0;

    fgets(linha, LINHA_MAX, f); // descarta cabeçalho

    while (fgets(linha, LINHA_MAX, f)) {
        vet = realloc(vet, (*tam + 1) * sizeof(RegistroArq1));
        if (!vet) {
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
            &vet[*tam].CO_REGIAO_CURSO
        );

        (*tam)++;
    }

    fclose(f);
    printf("Arquivo %s lido com sucesso!\n", nomeArquivo);
    return vet;
}

RegistroArq3* lerArq3(const char* nomeArquivo, int* tam) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) {
        printf("Erro ao abrir %s\n", nomeArquivo);
        exit(1);
    }

    RegistroArq3 *vet = NULL;
    char linha[LINHA_MAX];
    *tam = 0;

    fgets(linha, LINHA_MAX, f); // descarta cabeçalho

    while (fgets(linha, LINHA_MAX, f)) {
        vet = realloc(vet, (*tam + 1) * sizeof(RegistroArq3));
        if (!vet) {
            printf("Erro no realloc\n");
            exit(1);
        }

        sscanf(linha, "%d;%d;%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%f;%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%*[^;];%f",
            &vet[*tam].NU_ANO,
            &vet[*tam].CO_CURSO,
            &vet[*tam].NT_GER,
            &vet[*tam].NT_CE
        );

        (*tam)++;
    }

    fclose(f);
    printf("Arquivo %s lido com sucesso!\n", nomeArquivo);
    return vet;
}

// Busca binária por código de curso
void buscarCurso(RegistroArq1* vet, int tam, int codigoCurso) {
    // Ordenar por código de curso antes de buscar
    qsort(vet, tam, sizeof(RegistroArq1), compararPorCurso);
    
    int inicio = 0, fim = tam - 1;
    int encontrado = 0;
    
    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        
        if (vet[meio].CO_CURSO == codigoCurso) {
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
            encontrado = 1;
            break;
        } else if (vet[meio].CO_CURSO < codigoCurso) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    
    if (!encontrado) {
        printf("Curso com codigo %d nao encontrado.\n", codigoCurso);
    }
}

int compararPorIESeCurso(const void* a, const void* b) {
    int diffIES = ((RegistroArq1*)a)->CO_IES - ((RegistroArq1*)b)->CO_IES;
    if (diffIES != 0) return diffIES;
    return ((RegistroArq1*)a)->CO_CURSO - ((RegistroArq1*)b)->CO_CURSO;
}

// Função auxiliar para comparar por UF e depois por Curso
int compararPorUFeCurso(const void* a, const void* b) {
    int diffUF = ((RegistroArq1*)a)->CO_UF_CURSO - ((RegistroArq1*)b)->CO_UF_CURSO;
    if (diffUF != 0) return diffUF;
    return ((RegistroArq1*)a)->CO_CURSO - ((RegistroArq1*)b)->CO_CURSO;
}

// Função auxiliar para comparar por Grupo e depois por Curso
int compararPorGrupoeCurso(const void* a, const void* b) {
    int diffGrupo = ((RegistroArq1*)a)->CO_GRUPO - ((RegistroArq1*)b)->CO_GRUPO;
    if (diffGrupo != 0) return diffGrupo;
    return ((RegistroArq1*)a)->CO_CURSO - ((RegistroArq1*)b)->CO_CURSO;
}

void listarCursosPorIES(RegistroArq1* vet, int tam, int codigoIES) {
    // Ordenar por IES e depois por Curso
    qsort(vet, tam, sizeof(RegistroArq1), compararPorIESeCurso);
    
    printf("\n--- Cursos da IES %d (Busca Binaria) ---\n", codigoIES);
    int count = 0;
    
    // Busca binária para encontrar primeiro registro
    int inicio = 0, fim = tam - 1;
    int indiceInicial = -1;
    
    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        if (vet[meio].CO_IES == codigoIES) {
            indiceInicial = meio;
            fim = meio - 1;
        } else if (vet[meio].CO_IES < codigoIES) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    
    if (indiceInicial != -1) {
        int ultimoCurso = -1;
        for (int i = indiceInicial; i < tam && vet[i].CO_IES == codigoIES; i++) {
            if (vet[i].CO_CURSO != ultimoCurso) {
                printf("Curso: %d | Grupo: %d | UF: %d\n", 
                       vet[i].CO_CURSO, vet[i].CO_GRUPO, vet[i].CO_UF_CURSO);
                ultimoCurso = vet[i].CO_CURSO;
                count++;
            }
        }
    }
    
    printf("Total de cursos unicos: %d\n", count);
    if (count == 0) printf("Nenhum curso encontrado para esta IES.\n");
}

void listarCursosPorUF(RegistroArq1* vet, int tam, int codigoUF) {
    // Ordenar por UF e depois por Curso
    qsort(vet, tam, sizeof(RegistroArq1), compararPorUFeCurso);
    
    printf("\n--- Cursos da UF %d (Busca Binaria) ---\n", codigoUF);
    int count = 0;
    
    // Busca binária para encontrar primeiro registro
    int inicio = 0, fim = tam - 1;
    int indiceInicial = -1;
    
    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        if (vet[meio].CO_UF_CURSO == codigoUF) {
            indiceInicial = meio;
            fim = meio - 1;
        } else if (vet[meio].CO_UF_CURSO < codigoUF) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    
    if (indiceInicial != -1) {
        int ultimoCurso = -1;
        for (int i = indiceInicial; i < tam && vet[i].CO_UF_CURSO == codigoUF; i++) {
            if (vet[i].CO_CURSO != ultimoCurso) {
                printf("Curso: %d | IES: %d | Grupo: %d\n", 
                       vet[i].CO_CURSO, vet[i].CO_IES, vet[i].CO_GRUPO);
                ultimoCurso = vet[i].CO_CURSO;
                count++;
            }
        }
    }
    
    printf("Total de cursos unicos: %d\n", count);
    if (count == 0) printf("Nenhum curso encontrado para esta UF.\n");
}

void listarCursosPorGrupo(RegistroArq1* vet, int tam, int codigoGrupo) {
    // Ordenar por Grupo e depois por Curso
    qsort(vet, tam, sizeof(RegistroArq1), compararPorGrupoeCurso);
    
    printf("\n--- Cursos do Grupo (Area) %d (Busca Binaria) ---\n", codigoGrupo);
    int count = 0;
    
    // Busca binária para encontrar primeiro registro
    int inicio = 0, fim = tam - 1;
    int indiceInicial = -1;
    
    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        if (vet[meio].CO_GRUPO == codigoGrupo) {
            indiceInicial = meio;
            fim = meio - 1;
        } else if (vet[meio].CO_GRUPO < codigoGrupo) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    
    if (indiceInicial != -1) {
        int ultimoCurso = -1;
        for (int i = indiceInicial; i < tam && vet[i].CO_GRUPO == codigoGrupo; i++) {
            if (vet[i].CO_CURSO != ultimoCurso) {
                printf("Curso: %d | IES: %d | UF: %d\n", 
                       vet[i].CO_CURSO, vet[i].CO_IES, vet[i].CO_UF_CURSO);
                ultimoCurso = vet[i].CO_CURSO;
                count++;
            }
        }
    }
    
    printf("Total de cursos unicos: %d\n", count);
    if (count == 0) printf("Nenhum curso encontrado para este Grupo.\n");
}

int main(void){
    int tam1, tam3;

    printf("================= LEITURA DOS ARQUIVOS =================\n");

    RegistroArq1* arq1 = lerArq1("microdados2023_arq1.txt", &tam1);
    RegistroArq3* arq3 = lerArq3("microdados2023_arq3.txt", &tam3);

    printf("Registros arq1: %d\n", tam1);
    printf("Registros arq3: %d\n", tam3);

    int opcao;
    int codigo;

    do {
        printf("\n================= MENU =================\n");
        printf("1 - Buscar curso especifico\n");
        printf("2 - Listar cursos por IES\n");
        printf("3 - Listar cursos por UF\n");
        printf("4 - Listar cursos por Grupo (Area)\n");
        printf("0 - Sair\n");
        printf("Opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
            // Limpa o buffer em caso de entrada inválida
            while (getchar() != '\n');
            printf("Entrada invalida!\n");
            continue;
        }

        switch(opcao) {
            case 1:
                printf("Digite o codigo do curso: ");
                if (scanf("%d", &codigo) == 1) {
                    buscarCurso(arq1, tam1, codigo);
                } else {
                    while (getchar() != '\n');
                    printf("Codigo invalido!\n");
                }
                break;
            case 2:
                printf("Digite o codigo da IES: ");
                if (scanf("%d", &codigo) == 1) {
                    listarCursosPorIES(arq1, tam1, codigo);
                } else {
                    while (getchar() != '\n');
                    printf("Codigo invalido!\n");
                }
                break;
            case 3:
                printf("Digite o codigo da UF: ");
                if (scanf("%d", &codigo) == 1) {
                    listarCursosPorUF(arq1, tam1, codigo);
                } else {
                    while (getchar() != '\n');
                    printf("Codigo invalido!\n");
                }
                break;
            case 4:
                printf("Digite o codigo do Grupo: ");
                if (scanf("%d", &codigo) == 1) {
                    listarCursosPorGrupo(arq1, tam1, codigo);
                } else {
                    while (getchar() != '\n');
                    printf("Codigo invalido!\n");
                }
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while(opcao != 0);

    free(arq1);
    free(arq3);
    
    return 0;
}
