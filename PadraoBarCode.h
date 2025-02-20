#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Valores para os dígitos do lado esquerdo do código de barras
const char *L_Code[] = {
    "0001101", "0011001", "0010011", "0111101", "0100011",
    "0110001", "0101111", "0111011", "0110111", "0001011"
};

// Valores para os dígitos do lado direito do código de barras
const char *R_Code[] = {
    "1110010", "1100110", "1101100", "1000010", "1011100",
    "1001110", "1010000", "1000100", "1001000", "1110100"
};

// Struct do do tipo do Código de Barras
typedef struct {
    char nome_arquivo[100];
    char id_codigo[9]; // Código de 8 dígitos
    int largura_codigo; // = 67 | Valor das Áreas a ser multiplicado pelo valor de pixel por área
    int altura_padrao; // = 50
    int margem_padrao; // = 4
    int largura_totalPadrao; // = 209 | 67 de Areas * 3 de Pixel padrão + 8 de Margem 
    int altura_totalPadrao; // = 58 | 50 Pixels de altura + 8 de margem
    int pixels_areaPadrao; // = 3
} EAN8;

EAN8 BarCode;
void init_BarCode(EAN8 *e) {
    e->largura_codigo = 67;
    e->altura_padrao = 50;
    e->margem_padrao = 4;
    e->largura_totalPadrao = 209;
    e->altura_totalPadrao = 58;
    e->pixels_areaPadrao = 3;
}
/* Para compilar: gcc nomearquivo1.c nomearquivo.h -o nomeexecutavel -g -W     */