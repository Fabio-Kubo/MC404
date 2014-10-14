#include <stdio.h>
#include<stdlib.h>
#include "readASM.h"

typedef struct Posicao{
	int linha;
	int isEsquerda;
} Posicao;

int main(int argc, char *argv[]){

	char * codigoAssembly;
	char ** controleLinhas;
	int tamanhoCodigo;

	tamanhoCodigo = 0;
	codigoAssembly = NULL;
	controleLinhas = NULL;

    if (argc <= 1) {
	    printf("Informe pelo menos um arquivo de entrada.\n");
        exit(EXIT_FAILURE);
    }

    read_ASM_file(argv[1], &codigoAssembly, &controleLinhas, &tamanhoCodigo);
    return 0;
}
