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
	
	for (i = 0; i < 5; ++i) {

		if(strcmp(diretivas[i], termo) == 0){
			return i;
		}
	}

	return -1;
}

/*Metodo que verifica se o char passado como parametro indica o fim de um comando*/
int isCharFimComando(char a){

	return a == '\t' || a == '\n' || a == ' ' || a == '\0';
}

/*Metodo que verifica se o char passado como parametro indica o fim de uma linha*/
int isCharFimLinha(char a){

	return a == '\n' || a == '\0';
}

void stringToLower(char * string){
	int i;

	for (i = 0; string[i] != '\0'; i++) {
		string[i] = tolower(string[i]);
	}
}


void processaLinha(char ** controleLinhas, int qtdLinhas){

	int i, j, k;
	char * diretiva, * linhaQuebrada;
	const char * separadores = " 	\n";
	
	//percorre as linhas
	for(i = 0; i < qtdLinhas; i++){

		for (j = 0; controleLinhas[i][j] != '\0' && controleLinhas[i][j] != '\n'; j++) {

			linhaQuebrada = strtok(controleLinhas[i][j], separadores);

			while(linhaQuebrada != NULL){

				linhaQuebrada = strtok(NULL, separadores);
			}




			//se começar com ponto, vai vir uma diretiva
			if(controleLinhas[i][j] == '.'){

				diretiva = alocaVetorChar(6);

				for (k = 0; !isCharFimComando(controleLinhas[i][j]); k++, j++){
					
					if(k > 5){
						printf("Diretiva incorreta, linha %d \n", i);
						exit(EXIT_FAILURE);
					}

					diretiva[k] = tolower(controleLinhas[i][j]);
				}




				free(diretiva);

				if(isCharFimLinha(controleLinhas[i][j]))
					break;
			}

			if(controleLinhas[i][j] != '\0' && controleLinhas[i][j] != '\n')
				break;

			
		}
	}
	
}

int main(int argc, char *argv[]){

	char * codigoAssembly;
	char ** controleLinhas;
	int qtdLinhas;

	qtdLinhas = 0;
	codigoAssembly = NULL;
	controleLinhas = NULL;

    if (argc <= 1) {
	    printf("Informe pelo menos um arquivo de entrada.\n");
        exit(EXIT_FAILURE);
    }

    read_ASM_file(argv[1], &codigoAssembly, &controleLinhas, &qtdLinhas);

    free(codigoAssembly);
    free(controleLinhas);
    return 0;
}
