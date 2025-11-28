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

        // captura apenas os campos usados
        // os pontos e valores vazios após NT_GER e NT_CE são ignorados
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

int main(void){
    int tam1, tam3;

    printf("================= LEITURA DOS ARQUIVOS =================\n");

    RegistroArq1* arq1 = lerArq1("microdados2023_arq1.txt", &tam1);
    RegistroArq3* arq3 = lerArq3("microdados2023_arq3.txt", &tam3);

    printf("Registros arq1: %d\n", tam1);
    printf("Registros arq3: %d\n", tam3);

    free(arq1);
    free(arq3);
    
    return 0;                                                                                                                
}