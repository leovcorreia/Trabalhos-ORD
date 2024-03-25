/*
    Trabalho 1 - Organização e Recuperação de Dados
    Professora: Valeria Delisandra Feltrim
    Alunos:
        Leonardo Venâncio Correia (RA: 129266)
        Murilo Luis Calvo Neves (RA: 129037)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define TAM_MINIMO_SOBRA 10  // Tamanho mínimo que a sobra de um registro deve possuir para ser reaproveitada
#define NOME_ARQUIVO_DADOS "dados.dat"

typedef enum {False, True} booleano;  // Enum booleano para deixar o código mais limpo

// Funções auxiliares
void inserir_espaco_na_led(int offset, short tamanho, FILE* arquivo_de_dados);

// Funções de registros
void inserir_registro(char* novo_registro, FILE* arquivo_de_dados);
void remover_registro(char* identificador, FILE* arquivo_de_dados);
void buscar_registro(char* identificador, FILE* arquivo_de_dados);

// Funções de modos de operações
void impressao_da_led(FILE* arquivo_de_dados);
void fazer_operacoes(FILE* arquivo_de_dados, FILE* arquivo_de_operacoes);

int main(int argc, char *argv[]) {
    /*
        Ponto de entrada principal do programa
    */

    FILE *arquivo_de_dados = fopen(NOME_ARQUIVO_DADOS, "rb+");

    if (arquivo_de_dados == NULL)
    {
        fprintf(stderr, "\nNao foi encontrado o arquivo de dados para leitura");
        exit(EXIT_FAILURE);
    }

    if (argc == 3 && strcmp(argv[1], "-e") == 0) {

        printf("Modo de execucao de operacoes ativado ... nome do arquivo = %s\n", argv[2]);
        FILE *arquivo_de_operacoes = fopen(argv[2], "rb");  // Como dito no enunciado, assume-se que esse arquivo sempre é aberto corretamente
        fazer_operacoes(arquivo_de_dados, arquivo_de_operacoes);
        fclose(arquivo_de_operacoes);

    } else if (argc == 2 && strcmp(argv[1], "-p") == 0) {

        printf("Modo de impressao da LED ativado ...\n");
        impressao_da_led(arquivo_de_dados);

    } else {
        fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s -e nome_arquivo\n", argv[0]);
        fprintf(stderr, "$ %s -p\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fclose(arquivo_de_dados);
    return 0;
}


void fazer_operacoes(FILE* arquivo_de_dados, FILE* arquivo_de_operacoes)
{
    /*
    Lê e interpreta cada linha de comando descrita no arquivo de operações e executa a função correspondente.

    Parâmetros:
        FILE* arquivo_de_dados: O descritor do arquivo contento os dados do registros sobre os quais as operações serão realizadas
        FILE* arquivo_de_operacoes: O descritor do arquivo contendo as operações a serem realizadas junto com os seus argumentos.
    */
    char comando;

    /* Como não foi especificado um tamanho máximo para os argumentos das operações no arquivo de entrada, assumiu-se que elas podem
    ter tamanho até o maior valor possível de ser descrito com um tipo short */
    char parametro[32767];

    while (comando = fgetc(arquivo_de_operacoes), comando != EOF)  // Enquanto ainda houver comandos para serem lidos
    {
        // Lê o caractere de operação no início do laço, pula o espaço e lê o argumento para a variável parâmetro.
        fseek(arquivo_de_operacoes, 1, SEEK_CUR);
        fgets(parametro, 1024, arquivo_de_operacoes);

        int tamanho_parametro = strlen(parametro);

        // fgets coloca a quebra de linha na string, com exceção nos casos em que a última linha termina o arquivo diretamente (sem criar nova linha)
        if (parametro[tamanho_parametro - 1] == '\n')
        {
            parametro[tamanho_parametro - 1] = '\0';
        }

        switch (comando)
        {
            case 'r':
                printf("\n");
                remover_registro(parametro, arquivo_de_dados);
                break;
            case 'i':
                printf("\n");
                inserir_registro(parametro, arquivo_de_dados);
                break;
            case 'b':
                printf("\n");
                buscar_registro(parametro, arquivo_de_dados);
                break;
            default:
                printf("\n");
                printf("\nA operacao '%c' nao e uma operacao valida", comando);
                break;
        }
    }
}

void ler_identificador_registro(char *registro, char *nome)
{
    /*
    Recupera a string que serve como identificador de um registro

    Parâmetros:
        char* registro: String contendo o registro a ser lido
        char* nome: String que irá receber o identificador do registro
    */

    if (registro[0] == '*')
    {
        nome[0] = '\0';
    }

    int i = 0;
    while (registro[i] != '|' && registro[i] != '\0')
    {
        nome[i] = registro[i];
        i++;
    }
    nome[i] = '\0';
}


void buscar_registro(char* identificador, FILE* arquivo_de_dados) {
    /*
    Busca um registro no arquivo e mostra o resultado na saída padrão

    Parâmetros:
        char* identificador: String com o identificador
        FILE* arquivo_de_dados: Descritor do arquivo de dados no qual será realizada a busca
    */
    printf("\nBusca pelo registro de chave \"%s\"", identificador);

    char identificador_atual[64];
    char buffer[256];

    fseek(arquivo_de_dados, sizeof(int), SEEK_SET);  // Garantindo que o ponteiro de entrada esteja no início do primeiro registro

    short tamanho_registro;

    int res = 1;

    int posicao_do_ponteiro_de_leitura = sizeof(int);

    do
    {
        fread(&tamanho_registro, sizeof(short), 1, arquivo_de_dados);

        fread(buffer, sizeof(char), tamanho_registro, arquivo_de_dados);

        buffer[tamanho_registro] = '\0';

        ler_identificador_registro(buffer, identificador_atual);

        int res = strcmp(identificador_atual, identificador);

        // printf("\nComparando id \"%s\" com %s", identificador, identificador_atual);

        if (res == 0)
        {
            printf("\n%s (%d bytes)", buffer, tamanho_registro);
            printf("\nLocal: offset = %d bytes", posicao_do_ponteiro_de_leitura);
            return;
        }

        posicao_do_ponteiro_de_leitura += sizeof(short) + tamanho_registro;

    } while (0 == feof(arquivo_de_dados));

    printf("\nO registro nao foi encontrado");
}


void le_dados_led(int offset, FILE* arquivo_de_dados, short *tamanho, int *proximo_ponteiro)
{
    /*
    Recupera os dados de tamanho e de próximo offset de um dado elemento da LED

    Parâmetros:
        int offset: O inteiro no qual o elemento da LED a ser lido se inicia
        FILE* arquivo_de_dados: O arquivo com os dados a ser trabalhado
        short* tamanho: Uma variável short que irá receber o tamanho do elemento lido
        int* proximo_ponteiro: Uma variável int que irá receber o offset do próximo elemento da LED
    */
    fseek(arquivo_de_dados, offset, SEEK_SET);
    fread(tamanho, sizeof(short), 1, arquivo_de_dados);
    fseek(arquivo_de_dados, 1, SEEK_CUR);  // Pulando o '*'
    fread(proximo_ponteiro, sizeof(int), 1, arquivo_de_dados);
}


void inserir_espaco_na_led(int offset, short tamanho, FILE* arquivo_de_dados)
{
    /*
    Insere um espaço novamente na LED de forma ordenada.

    Parâmetros:
        int offset: O offset inicial de onde se inicia o espaço a ser inserido na LED
        short tamanho: O tamanho do local a ser inserido na LED (tamanho do registro + 2 bytes para anotá-lo)
        FILE* arquivo_de_dados: O arquivo com os dados a ser trabalhado
    */
    fseek(arquivo_de_dados, offset, SEEK_SET);
    short tamanho_para_registro = tamanho - sizeof(short);
    fwrite(&tamanho_para_registro, sizeof(short), 1, arquivo_de_dados);
    fwrite("*", sizeof(char), 1, arquivo_de_dados);

    // Conectar na LED
    short tamanho_antigo = -1;
    int aponta_antigo = -1;

    short tamanho_atual = -1;
    int aponta_atual = -1;

    short tamanho_proximo = -1;
    int aponta_proximo = -1;

    fseek(arquivo_de_dados, 0, SEEK_SET); // Vai para o inicio do arquivo
    fread(&aponta_proximo, sizeof(int), 1, arquivo_de_dados);  // Lê o primeiro ponteiro da LED

    // Inserção de espaço quando a LED está vazia
    if (aponta_proximo == -1)
    {
        fseek(arquivo_de_dados, 0, SEEK_SET);
        fwrite(&offset, sizeof(int), 1, arquivo_de_dados);
        fseek(arquivo_de_dados, offset + sizeof(short) + 1, SEEK_SET);
        int ptr = -1;
        fwrite(&ptr, sizeof(int), 1, arquivo_de_dados);
        return;
    }

    aponta_atual = aponta_proximo;
    le_dados_led(aponta_atual, arquivo_de_dados, &tamanho_proximo, &aponta_proximo);

    while (aponta_proximo != -1 && tamanho_proximo > tamanho_para_registro)
    {
        tamanho_antigo = tamanho_atual;
        tamanho_atual = tamanho_proximo;

        aponta_antigo = aponta_atual;
        aponta_atual = aponta_proximo;

        le_dados_led(aponta_atual, arquivo_de_dados, &tamanho_proximo, &aponta_proximo);
    }

    if (tamanho_proximo < tamanho_para_registro)  // Inserção entre os elementos anterior e atual
    {
        // Possíveis casos
        if (aponta_antigo != -1)
        {
            fseek(arquivo_de_dados, aponta_antigo + sizeof(short) + 1, SEEK_SET);
        }
        else
        {
            fseek(arquivo_de_dados, 0, SEEK_SET);
        }

        fwrite(&offset, sizeof(int), 1, arquivo_de_dados);

        fseek(arquivo_de_dados, offset + sizeof(short) + 1, SEEK_SET);
        fwrite(&aponta_atual, sizeof(int), 1, arquivo_de_dados);
    }
    else  // Inserção entre os elementos atual e próximo
    {
        fseek(arquivo_de_dados, aponta_atual + sizeof(short) + 1, SEEK_SET);
        fwrite(&offset, sizeof(int), 1, arquivo_de_dados);

        fseek(arquivo_de_dados, offset + sizeof(short) + 1, SEEK_SET);
        fwrite(&aponta_proximo, sizeof(int), 1, arquivo_de_dados);
    }
}


void inserir_registro(char* novo_registro, FILE* arquivo_de_dados)
{
    /*
    Insere um novo registro no arquivo no arquivo de dados

    Parâmetros:
        char* novo_registro: string contendo o novo registro a ser adicionado
        FILE* arquivo_de_dados: O arquivo de dados a ser trabalhado
    */
    char nome_registro[64];

    ler_identificador_registro(novo_registro, nome_registro);

    short tamanho_novo_registro = strlen(novo_registro);

    printf("\nInsercao do registro de chave \"%s\" (%d bytes)", nome_registro, tamanho_novo_registro);

    fseek(arquivo_de_dados, 0, SEEK_SET);  // Coloca ponteiro de leitura no inicio

    int offset_atual_led;
    short tamanho_atual_led;

    fread(&offset_atual_led, sizeof(int), 1, arquivo_de_dados);  // Lê o offset do primeiro elemento da LED

    if (offset_atual_led == -1)
    {
        // Inserção quando ainda não foi feita nenhuma remoção
        printf("\nLocal: Fim do arquivo");
        fseek(arquivo_de_dados, 0, SEEK_END);
        fwrite(&tamanho_novo_registro, sizeof(short), 1, arquivo_de_dados);
        fwrite(novo_registro, tamanho_novo_registro, 1, arquivo_de_dados);
    }
    else
    {
        fseek(arquivo_de_dados, offset_atual_led, SEEK_SET);  // Vai até a posição de inserção
        fread(&tamanho_atual_led, sizeof(short), 1, arquivo_de_dados);

        if (tamanho_atual_led < tamanho_novo_registro)  // Sem espaço vazio que caiba o elemento a ser adicionado
        {
            printf("\nLocal: Fim do arquivo");
            fseek(arquivo_de_dados, 0, SEEK_END);
            fwrite(&tamanho_novo_registro, sizeof(short), 1, arquivo_de_dados);
            fwrite(novo_registro, tamanho_novo_registro, 1, arquivo_de_dados);
        }
        else
        {
            fseek(arquivo_de_dados, 1, SEEK_CUR); // Pulando o '*'
            int proximo_ponteiro_led;

            fread(&proximo_ponteiro_led, sizeof(int), 1, arquivo_de_dados); // Lendo o proximo ponteiro da led
            fseek(arquivo_de_dados, offset_atual_led, SEEK_SET); // Voltando para o íncio do local onde o registro deverá ser escrito

            printf("\nLocal de insercao: offset = %d bytes", offset_atual_led);

            booleano sobrou_espaco_suficiente = False;

            // Escrevendo o novo registro

            short sobra = tamanho_atual_led - tamanho_novo_registro;

            if (sobra > TAM_MINIMO_SOBRA)
            {
                sobrou_espaco_suficiente = True;
                // Só escreve um novo tamanho caso haja suficiente para ser reutilizado
                fwrite(&tamanho_novo_registro, sizeof(short), 1, arquivo_de_dados);
            }
            else
            {
                // Caso contrário, só passa adiante
                fseek(arquivo_de_dados, sizeof(short), SEEK_CUR);
            }

            fwrite(novo_registro, tamanho_novo_registro, 1, arquivo_de_dados);

            fseek(arquivo_de_dados, 0, SEEK_SET); // Voltando para o início do arquivo
            fwrite(&proximo_ponteiro_led, sizeof(int), 1, arquivo_de_dados); // Conectando a ponta inicial da LED com a próxima

            fseek(arquivo_de_dados, offset_atual_led + tamanho_novo_registro + sizeof(short), SEEK_SET); // Voltando para onde o espaço vazio está

            printf("\nTamanho do espaco reutilizado: %d ", tamanho_atual_led);
            if (sobra > TAM_MINIMO_SOBRA)
            {
                printf("(sobra de %d bytes)", sobra - sizeof(short));
                // Caso ainda haja espaço utilizável
                inserir_espaco_na_led(offset_atual_led + tamanho_novo_registro + sizeof(short),
                tamanho_atual_led - tamanho_novo_registro, arquivo_de_dados);
            }
        }
    }
}


void remover_registro(char* identificador, FILE* arquivo_de_dados)
{
    /*
    Remove um registro do arquivo de dados

    Parâmetros:
        char* identificador: string contendo o identificador do elemento a ser removido
        FILE* arquivo_de_dados: O arquivo de dados a ser trabalhado
    */

    printf("\nRemocao do registro de chave \"%s\"", identificador);

    char identificador_atual[64];
    char buffer[256];

    fseek(arquivo_de_dados, sizeof(int), SEEK_SET);  // Garantindo que o ponteiro de entrada esteja no início do primeiro registro

    short tamanho_registro;

    int res = 1;

    int posicao_do_ponteiro_de_leitura = sizeof(int);

    do
    {
        fread(&tamanho_registro, sizeof(short), 1, arquivo_de_dados);

        fread(buffer, sizeof(char), tamanho_registro, arquivo_de_dados);

        ler_identificador_registro(buffer, identificador_atual);

        int res = strcmp(identificador_atual, identificador);

        if (strcmp(identificador_atual, identificador) == 0)
        {
            printf("\nRegistro removido! Tamanho: %d bytes", tamanho_registro);
            printf("\nLocal: offset = %d bytes", posicao_do_ponteiro_de_leitura);
            inserir_espaco_na_led(posicao_do_ponteiro_de_leitura, tamanho_registro + sizeof(short), arquivo_de_dados);
            return;
        }

        posicao_do_ponteiro_de_leitura += sizeof(short) + tamanho_registro;

    } while (0 == feof(arquivo_de_dados));

    printf("\nErro: o registro nao foi encontrado");
}

void impressao_da_led(FILE* arquivo_de_dados)
{
    /*
    Imprime os espaços disponíveis na LED

    Parâmetros:
        FILE* arquivo_de_dados: Arquivo de dados com a LED
    */
    printf("\n LED -> ");
    int total_espacos_disponiveis = 0;

    short tamanho_lido = 0;
    int ponteiro_lido = 0;

    fseek(arquivo_de_dados, 0, SEEK_SET);

    fread(&ponteiro_lido, sizeof(int), 1, arquivo_de_dados);

    while (ponteiro_lido != -1)
    {
        printf("[offset: %d", ponteiro_lido);
        fseek(arquivo_de_dados, ponteiro_lido, SEEK_SET);
        fread(&tamanho_lido, sizeof(short), 1, arquivo_de_dados);
        printf(", tam: %d] -> ", tamanho_lido);
        fseek(arquivo_de_dados, 1, SEEK_CUR);
        fread(&ponteiro_lido, sizeof(int), 1, arquivo_de_dados);
        total_espacos_disponiveis += 1;
    }

    printf("[offset: -1]\nTotal: %d tamanhos disponiveis", total_espacos_disponiveis);
}
