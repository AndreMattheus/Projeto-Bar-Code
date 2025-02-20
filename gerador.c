#include "PadraoBarCode.h"

// Valida se o código de 8 dígitos é numérico e verifica o dígito final
int validar_codigo(const char *codigo) {
    // Verificar se o código tem exatamente 8 dígitos
    if (strlen(codigo) != 8) {
        printf("O Codigo deve ter 8 digitos.\n");
        return 0;
    }

    // Verificar se o código é feito apenas de números
    int i;
    for (i = 0; i < 8; i++) {
        if (!isdigit(codigo[i])) {
            printf("O Codigo informado contem caracteres nao numericos.\n");
            return 0;
        }
    }

    // Determina a soma ponderada dos dígitos
    int soma = 0;
    for (i = 0; i < 7; i++) {
        int digito = codigo[i] - '0';
        // Se o índice for par, adiciona o dígito *1. Se for ímpar, adiciona o dígito *3
        soma += ((i + 1) % 2 == 0) ? digito * 1 : digito * 3;
    }
    // Determina se o dígito verificador condiz com o código
    int digito_verificador = (10 - (soma % 10));
    if(digito_verificador != codigo[7] - '0') {
        printf("O digito verificador nao condiz com o codigo.\n");
        return 0;
    }
    return 1;
}

int obter_inteiro() {
    int valor;
    while (1) {
        // Verifica se o valor lido pode ser convertido em número
        if (scanf("%d", &valor) == 1) {
            break;
        }
        else {
            printf("Entrada invalida. Por favor, digite um numero.\n");
            // Limpa o buffer de entrada
            while (getchar() != '\n')
                ;
        }
    }
    return valor;
}

// Obtém o código de 8 dígitos e retorna já validado
int obter_codigo_valido(char *codigo) {
    printf("Digite o codigo de 8 digitos: ");
    scanf("%9s", codigo);
    if (!validar_codigo(codigo)) {
        printf("Codigo invalido.\n");
        return 0;
    }
    return 1;
}

void criar_codigo_barras(const char *codigo, FILE *arquivo) {
    int largura_total, altura_total, margem, pixels_area, altura; 
    char respostaParametros;
    // Loop para obter uma resposta válida
    do {
        printf("Deseja definir parametros da imagem? (s/n): ");
        scanf(" %c", &respostaParametros);
        if (tolower(respostaParametros) != 's' && tolower(respostaParametros) != 'n') {
            printf("Insira uma resposta valida!\n");
        }
    } while (tolower(respostaParametros) != 's' && tolower(respostaParametros) != 'n');

    // Define valores da imagem para valores padrão com a resposta "n"
    if (tolower(respostaParametros) == 'n') {
        largura_total = BarCode.largura_totalPadrao;
        altura_total = BarCode.altura_totalPadrao;
        pixels_area = BarCode.pixels_areaPadrao;
        margem = BarCode.margem_padrao;
        // Define valores da imagem customizados com a resposta "s"
    }
    else if (tolower(respostaParametros) == 's') {
        printf("Insira o valor de pixels por area: ");
        pixels_area = obter_inteiro();
        printf("Insira o valor, em pixels, da altura: ");
        altura = obter_inteiro();
        printf("Insira o valor, em pixels, da margem: ");
        margem = obter_inteiro();

        largura_total = BarCode.largura_codigo * pixels_area + (2 * margem);
        altura_total = altura + (2 * margem);
    }
    // Cabeçalho da imagem PBM
    fprintf(arquivo, "P1\n%d %d\n", largura_total, altura_total);
    int x, y;
    // Preenchimento do arquivo linha por linha
    // Onde altura = quantd. de linhas e largura = quantd. de colunas
    for (y = 0; y < altura_total; y++) {
        for (x = 0; x < largura_total; x++) {
            int dentro_margem_vertical = (y >= margem && y < altura_total - margem);
            int dentro_margem_horizontal = (x >= margem && x < largura_total - margem);

            // Verifica se o x ou y não está dentro da área da margem
            // Se não estiver, deixa a linha em branco (0) e vai para a próxima iteração
            if (!dentro_margem_vertical || !dentro_margem_horizontal) {
                fprintf(arquivo, "0 ");
                continue;
            }

            // Coordenada ajustada ao início do código de barras
            int posicao = (x - margem) / pixels_area;
            char bit = '0';

            // Seleciona qual padrão de bits utilizar com base na posição
            // Posição considerada com base na quantidade de áreas * pixel
            if (posicao < 3) { // Início "101"
                bit = "101"[posicao];
            }
            else if (posicao < 31) { // Lado esquerdo (primeiros 4 dígitos)
                // -3 porque é o 101 já utilizado /7 porque é o tamanho de cada padrão %7 para pegar o último valor. Lógica de MIN/MAX para escrever tudo nesse intervalo
                bit = L_Code[codigo[(posicao - 3) / 7] - '0'][(posicao - 3) % 7];
            }
            else if (posicao < 36) { // Separador "01010"
                bit = "01010"[posicao - 31];
            }
            else if (posicao < 64) { // Lado direito (últimos 4 dígitos)
                bit = R_Code[codigo[(posicao - 36) / 7 + 4] - '0'][(posicao - 36) % 7];
            }
            else { // Final "101"
                bit = "101"[posicao - 64];
            }

            fprintf(arquivo, "%c ", bit);
        }
        fprintf(arquivo, "\n");
    }
}

int main(){
    init_BarCode(&BarCode);
    // Chama a função que recebe os dígitos do código de barras e define na sub-variável id_codigo da varíavel BarCode
    if (!obter_codigo_valido(BarCode.id_codigo)) {
        return 0;
    }

    // Definir o nome do arquivo a ser salvo o PBM
    char respostaNome;
    do {
        printf("Deseja definir um nome para o arquivo de imagem? (s/n): ");
        scanf(" %c", &respostaNome);
        if (tolower(respostaNome) != 's' && tolower(respostaNome) != 'n')
        {
            printf("Insira uma resposta valida!\n");
        }
    } while (tolower(respostaNome) != 's' && tolower(respostaNome) != 'n');

    if (tolower(respostaNome) == 's') {
        printf("Digite o nome do arquivo: ");
        scanf(" %s", BarCode.nome_arquivo);
        // Verifica se o nome já termina com .pbm
    if (strstr(BarCode.nome_arquivo, ".pbm") == NULL) {
        // Adiciona .pbm ao final, se não tiver
        strcat(BarCode.nome_arquivo, ".pbm");
    }
    }
    else {
        strcpy(BarCode.nome_arquivo, "arquivoDoCodigo.pbm\0");
    }
    // Verifica se um arquivo com esse nome já existe
    FILE *arquivo = fopen(BarCode.nome_arquivo, "r");
    if (arquivo) {
        fclose(arquivo);
        char respostaSobrescrever;
        do {
            printf("Arquivo %s ja existe. Deseja sobrescrever? (s/n): ", BarCode.nome_arquivo);
            scanf(" %c", &respostaSobrescrever);
            if (tolower(respostaSobrescrever) != 's' && tolower(respostaSobrescrever) != 'n')
            {
                printf("Insira uma resposta valida!\n");
            }
        } while (tolower(respostaSobrescrever) != 's' && tolower(respostaSobrescrever) != 'n');

        if (tolower(respostaSobrescrever) == 'n')
        {
            printf("Arquivo resultante ja existe!");
            return 1;
        }
    }
    arquivo = fopen(BarCode.nome_arquivo, "w");
    // Cria
    criar_codigo_barras(BarCode.id_codigo, arquivo);
    fclose(arquivo);

    printf("Codigo de barras salvo em %s.\n", BarCode.nome_arquivo);

    return 0;
}