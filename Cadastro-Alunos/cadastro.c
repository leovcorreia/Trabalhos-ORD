#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALUNOS 44
#define TAM_NOME 50
#define TAM_DISCIPLINA 50

typedef struct {
    int RA;
    char nome[TAM_NOME];
    char disciplina[TAM_DISCIPLINA];
    float nota1;
    float nota2;
    float nota3;
    char situacao[TAM_NOME];
} Aluno;

void calcularSituacaoAluno(Aluno *aluno) {
    float media = (aluno->nota1 + 2 * aluno->nota2 + 3 * aluno->nota3) / 6.0;

    if (media >= 6.0)
        strcpy(aluno->situacao, "Aprovado");
    else if (media > 2 && media < 6)
        strcpy(aluno->situacao, "Em Exame");
    else
        strcpy(aluno->situacao, "Reprovado");
}

void cadastrarNovoAluno(FILE *arquivo) {
    Aluno aluno;
    printf("Digite o RA do aluno: ");
    scanf("%d", &aluno.RA);

    // Verifica se o RA já existe no arquivo
    rewind(arquivo); // Volta p/ início do arquivo
    Aluno temp;
    while (fread(&temp, sizeof(Aluno), 1, arquivo) == 1) {
        if (temp.RA == aluno.RA) {
            printf("RA ja cadastrado. Cadastro não concluido!\n");
            return;
        }
    }

    fseek(stdin,0,SEEK_END); // ------------- limpeza de buffer 
    printf("Digite o nome do aluno: ");
    gets(aluno.nome);
    fseek(stdin,0,SEEK_END); // ------------- limpeza de buffer

    printf("Digite a disciplina do aluno: ");
    gets(aluno.disciplina);
    fseek(stdin,0,SEEK_END); // ------------- limpeza de buffer

    do {
        printf("Digite a nota 1 do aluno (de 0 a 10): ");
        scanf("%f", &aluno.nota1);
    } while (aluno.nota1 < 0 || aluno.nota1 > 10);
    do {
        printf("Digite a nota 2 do aluno (de 0 a 10): ");
        scanf("%f", &aluno.nota2);
    } while (aluno.nota2 < 0 || aluno.nota2 > 10);
    do {
        printf("Digite a nota 3 do aluno (de 0 a 10): ");
        scanf("%f", &aluno.nota3);
    } while (aluno.nota3 < 0 || aluno.nota3 > 10);

    calcularSituacaoAluno(&aluno); 
        
    fseek(arquivo, 0, SEEK_END);
    fwrite(&aluno, sizeof(Aluno), 1, arquivo);

    printf("Aluno cadastrado com sucesso!\n");
}

void alterarAluno(FILE *arquivo) {
    int ra;
    printf("Digite o RA do aluno que deseja alterar os dados: ");
    scanf("%d", &ra);

    rewind(arquivo);
    Aluno aluno;
    int encontrado = 0;
    while (fread(&aluno, sizeof(Aluno), 1, arquivo) == 1) {
        if (aluno.RA == ra) {
            encontrado = 1;
            printf("Aluno encontrado. Os dados atuais dele sao:\n");
            printf("Nome: %s\n", aluno.nome);
            printf("Disciplina: %s\n", aluno.disciplina);
            printf("Nota 1: %.2f\n", aluno.nota1);
            printf("Nota 2: %.2f\n", aluno.nota2);
            printf("Nota 3: %.2f\n", aluno.nota3);

            fseek(stdin,0,SEEK_END); // ------------- limpeza de buffer
            printf("Digite os novos dados do aluno:\n");
            printf("Digite o nome do aluno: ");
            gets(aluno.nome);
            fseek(stdin,0,SEEK_END); // ------------- limpeza de buffer

            printf("Digite a disciplina do aluno: ");
            gets(aluno.disciplina);
            fseek(stdin,0,SEEK_END); // ------------- limpeza de buffer

            do {
                printf("Digite a nota 1 do aluno (de 0 a 10): ");
                scanf("%f", &aluno.nota1);
            } while (aluno.nota1 < 0 || aluno.nota1 > 10);
            do {
                printf("Digite a nota 2 do aluno (de 0 a 10): ");
                scanf("%f", &aluno.nota2);
            } while (aluno.nota2 < 0 || aluno.nota2 > 10);
            do {
                printf("Digite a nota 3 do aluno (de 0 a 10): ");
                scanf("%f", &aluno.nota3);
            } while (aluno.nota3 < 0 || aluno.nota3 > 10);

            calcularSituacaoAluno(&aluno);

            // Atualiza o registro do aluno no arquivo
            fseek(arquivo, -sizeof(Aluno), SEEK_CUR);
            fwrite(&aluno, sizeof(Aluno), 1, arquivo);

            printf("Aluno alterado com sucesso!\n");
            break;
        }
    }

    if (!encontrado)
        printf("Aluno nao encontrado no sistema!\n");
}

void listarAluno(FILE *arquivo) {
    int ra;
    printf("Digite o RA do aluno que deseja ver os dados: ");
    scanf("%d", &ra);

    rewind(arquivo);
    Aluno aluno;
    int encontrado = 0;
    while (fread(&aluno, sizeof(Aluno), 1, arquivo) == 1) {
        if (aluno.RA == ra) {
            encontrado = 1;
            printf("Dados do aluno:\n");
            printf("RA: %d\n", aluno.RA);
            printf("Nome: %s\n", aluno.nome);
            printf("Disciplina: %s\n", aluno.disciplina);
            printf("Nota 1: %.2f\n", aluno.nota1);
            printf("Nota 2: %.2f\n", aluno.nota2);
            printf("Nota 3: %.2f\n", aluno.nota3);

            float media = (aluno.nota1 * 1 + aluno.nota2 * 2 + aluno.nota3 * 3) / 6.0;
            printf("Media: %.2f\n", media);

            printf("Situacao: %s\n", aluno.situacao);
            break;
        }
    }

    if (!encontrado)
        printf("Aluno nao encontrado no sistema!\n");
}

int main() {
    FILE *arquivo;
    arquivo = fopen("alunos.dat", "r+b");
    if (arquivo == NULL) {
        arquivo = fopen("alunos.dat", "w+b");
        if (arquivo == NULL) {
            printf("Erro ao criar o arquivo.\n");
            return 1;
        }
    }

    int opcao;
    do {
        printf("\nMENU DO SISTEMA:\n");
        printf("1) Cadastrar novo aluno\n");
        printf("2) Alterar dados de um aluno\n");
        printf("3) Listar dados de um aluno\n");
        printf("4) Fechar o programa\n");
        printf("Selecione uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                cadastrarNovoAluno(arquivo);
                break;
            case 2:
                alterarAluno(arquivo);
                break;
            case 3:
                listarAluno(arquivo);
                break;
            case 4:
                printf("Saindo do programa!\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente!\n");
        }
    } while (opcao != 4);

    fclose(arquivo);
    return 0;
}
