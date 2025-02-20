#include "PadraoBarCode.h"

// Função para carregar imagem PBM
int **ler_imagem(const char *arquivo_imagem, int *largura_imagem, int *altura_imagem) {
    // Abrir o arquivo com nome informado para leitura
    FILE *arquivo_leitura = fopen(arquivo_imagem, "r");
    if (!arquivo_leitura) {
        printf("Erro: O arquivo %s nao foi encontrado.\n", arquivo_imagem);
        return NULL;
    }

    // Verificar a existência do cabeçalho padrão para arquivos PBM
    char tipo_arquivo[3];
    fscanf(arquivo_leitura, "%2s", tipo_arquivo);
    if (strcmp(tipo_arquivo, "P1") != 0) {
        printf("Formato de imagem invalido. O tipo deve ser PBM.\n");
        fclose(arquivo_leitura);
        return NULL;
    }

    // Ler a largura e altura total da imagem
    fscanf(arquivo_leitura, "%d %d", largura_imagem, altura_imagem);

    // Alocar memória a ser utilizada para os pixels da imagem
    int **matriz_imagem = malloc(*altura_imagem * sizeof(int *));
    for (int i = 0; i < *altura_imagem; i++) {
        matriz_imagem[i] = malloc(*largura_imagem * sizeof(int));
        for (int j = 0; j < *largura_imagem; j++) {
            fscanf(arquivo_leitura, "%d", &matriz_imagem[i][j]);
        }
    }

    fclose(arquivo_leitura);
    return matriz_imagem;
}

// Função para calcular a margem superior
void calcular_margem(int **matriz, int largura, int altura, int *margem_sup) {
    *margem_sup = 0;

    // Calcular a margem superior (Linhas superiores vazias)
    for (int linha = 0; linha < altura; linha++) {
        int soma_pixels = 0;
        for (int coluna = 0; coluna < largura; coluna++) {
            soma_pixels += matriz[linha][coluna];
        }
        // Se todos os pixels não forem 0, então a linha não é vazia e não será margem
        if (soma_pixels > 0) {
            break;  
        }
        // Definir e atualizar o valor da margem encontrado a cada iteração
        (*margem_sup)++;
    }
}

// Função para decodificar o código de barras
void decodificar_barras(int **matriz, int largura_img, int altura_img) {
    int margem_superior;
    // Chamar a função para definir o valor de margem do código de barras
    calcular_margem(matriz, largura_img, altura_img, &margem_superior);

    // Definir a largura total do codigo, retirando a margem, o valor de pixels por área, dividindo pelo valor de largura padrão dos códigos
    // e o valor da linha média
    int largura_total_barras = largura_img - (2 * margem_superior);
    int pixelsPorArea = largura_total_barras / BarCode.largura_codigo;
    int linha_media = margem_superior + altura_img / 2;
    char sequencia_bits[68] = {0};

    // Identificar os bits
    for (int i = 0; i < BarCode.largura_codigo; i++) {
        int total_brancos = 0;
        int total_pretos = 0;

        for (int p = 0; p < pixelsPorArea; p++) {
            int valor_pixel = matriz[linha_media][margem_superior + i * pixelsPorArea + p];
            if (valor_pixel == 0) {
                total_brancos++;
            } else {
                total_pretos++;
            }
        }
        // Definir se o bit é '0' ou '1'
        sequencia_bits[i] = (total_pretos > total_brancos) ? '1' : '0';
    }

    // Validar os marcadores padrão de início, meio e fim
    if (strncmp(sequencia_bits, "101", 3) != 0 || strncmp(sequencia_bits + 64, "101", 3) != 0 || strncmp(sequencia_bits + 31, "01010", 5) != 0) {
        printf("Erro: Estrutura de codigo de barras invalida.\n");
        return;
    }

    printf("Codigo de barras decodificado: ");
    
    // Decodificar números
    for (int i = 0; i < 2; i++) {
        int inicio = (i == 0) ? 3 : 36;
        int fim = (i == 0) ? 31 : 64;
        const char **padrao = (i == 0) ? L_Code : R_Code;

        for (int j = inicio; j < fim; j += 7) {
            int encontrado = 0;
            for (int k = 0; k < 10; k++) {
                if (strncmp(sequencia_bits + j, padrao[k], 7) == 0) {
                    printf("%d", k);
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado) {
                printf("?");
            }
        }
    }
    printf("\n");
}

int main() {
    init_BarCode(&BarCode);
    // Receber o nome do arquivo PBM a ser lido
    char nome_imagem[100];
    printf("Informe o nome do arquivo PBM com o codigo de barras: ");
    scanf(" %99s", nome_imagem);

    // Verifica se o nome termina com .pbm
char *ext = strstr(nome_imagem, ".pbm");
if (ext == NULL || strcmp(ext, ".pbm") != 0) {
    // Se não terminar com .pbm, adiciona ou substitui a extensão para .pbm
    if (ext != NULL) {
        // Se existir uma extensão, substitui por .pbm
        strcpy(ext, ".pbm");
    } else {
        // Se não houver extensão, adiciona .pbm ao final
        strcat(nome_imagem, ".pbm");
    }
}

    int largura, altura;
    // Verificar se há um código de barras válido no arquivo PBM
    int **imagem = ler_imagem(nome_imagem, &largura, &altura);
    if (!imagem) {
        printf("O codigo de barras nao foi encontrado.");
        return 1;
    }

    // Decifrar o PBM em um código de barras numérico
    decodificar_barras(imagem, largura, altura);

    // Liberar memória alocada
    for (int i = 0; i < altura; i++) {
        free(imagem[i]);
    }
    free(imagem);

    return 0;
}