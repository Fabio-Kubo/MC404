#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "readASM.h"

typedef struct Posicao{
	int linha;
	int isEsquerda;
} PosicaoMemoria;

typedef struct RotuloEndereco{
	char * rotulo;
	int endereco;
	struct RotuloEndereco * prox;
} RotuloEndereco;

	
RotuloEndereco * alocaRotuloEndereco(){

	RotuloEndereco * rotuloEndereco;	
	rotuloEndereco = (RotuloEndereco *) malloc(sizeof(RotuloEndereco));

	if(rotuloEndereco == NULL) {
		printf("Erro ao alocar o rotuloEndereco\n");
		exit(EXIT_FAILURE);
	}

	return rotuloEndereco;
}

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

void adicionaRotulo(char * novoRotulo, RotuloEndereco ** mapaRotulosEnderecos){

	RotuloEndereco * novoRotuloEndereco, * ultimoRotulo;

	novoRotuloEndereco = alocaRotuloEndereco();

	(* novoRotuloEndereco).rotulo = novoRotulo;
	(* novoRotuloEndereco).endereco = 13213131;
	(* novoRotuloEndereco).prox = NULL;

	if((* mapaRotulosEnderecos) == NULL){
		(* mapaRotulosEnderecos) = novoRotuloEndereco;
	}
	else{

		ultimoRotulo = (* mapaRotulosEnderecos);

		while((* ultimoRotulo).prox != NULL){
			ultimoRotulo = (* ultimoRotulo).prox;
		}

		(* ultimoRotulo).prox = novoRotuloEndereco;
	}

}


void processaLinha(char ** controleLinhas, int qtdLinhas){

	int i, diretivaId;
	char * linhaQuebrada;
	RotuloEndereco * mapaRotulosEnderecos;
	const char * separadores = " 	\n";
	
	mapaRotulosEnderecos = NULL;


	for(i = 0; i < qtdLinhas; i++){

		linhaQuebrada = strtok(controleLinhas[i], separadores);			
		stringToLower(linhaQuebrada);

		while(linhaQuebrada != NULL){

			/*Se for diretiva*/
			if(linhaQuebrada[0] == '.'){
					
				diretivaId = isDiretiva(&linhaQuebrada[1]);

				switch(diretivaId){
					case 0:
						break;
					case 1:
						break;
					case 2:
						break;
					case 3:
						break;
					case 4:
						break;
					default:
						printf("Diretiva incorreta, linha %d \n", i);
						exit(EXIT_FAILURE);
				}

			}
			else if(linhaQuebrada[strlen(linhaQuebrada) - 1] == ':'){ /*Se for rotulo*/
				adicionaRotulo(linhaQuebrada, &mapaRotulosEnderecos);
			}

			linhaQuebrada = strtok(NULL, separadores);
			
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
