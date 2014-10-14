#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "readASM.h"

typedef struct Posicao{
	int linha;
	int isEsquerda;
} Posicao;

void alteraParaMinusculo(char * string){

	int i;

	for (i = 0; string[i] != '\0'; i++){
		string[i] = tolower(string[i]);
	}
}



int isDiretiva(char * termo){

	int i;
	
	char diretivas[5][6];
	strcpy(diretivas[0], "org");
	strcpy(diretivas[1], "align");
	strcpy(diretivas[2], "wfill");
	strcpy(diretivas[3], "word");
	strcpy(diretivas[4], "set");

	alteraParaMinusculo(termo);
	
	for (i = 0; i < count; ++i) {

		if(strcmp(diretivas[i], termo) == 0){
			return i;
		}
	}

	return -1;
}


void processaLinha(){
	
}

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

    free(codigoAssembly);
    free(controleLinhas);
    return 0;
}
