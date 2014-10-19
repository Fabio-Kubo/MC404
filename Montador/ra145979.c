#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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

typedef struct Palavra{
	int endereco;
	int isEsquerda;
	int conteudo;
	struct Palavra * prox;
} Palavra;

PosicaoMemoria posicaoMemoriaAtual;

	
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

	if(string != NULL){
		
		for (i = 0; string[i] != '\0'; i++) {
			string[i] = tolower(string[i]);
		}
	}
}

void adicionaRotulo(char * novoRotulo, RotuloEndereco ** mapaRotulosEnderecos){

	RotuloEndereco * novoRotuloEndereco, * ultimoRotulo;

	novoRotuloEndereco = alocaRotuloEndereco();

	(* novoRotuloEndereco).rotulo = novoRotulo;
	(* novoRotuloEndereco).endereco = posicaoMemoriaAtual.linha;
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

int hexaToInt(char c){
	
	int numero;

	if(c == 'a'){
		numero = 10;
	}
	else if(c == 'b'){
		numero = 11;
	}
	else if(c == 'c'){
		numero = 12;
	}
	else if(c == 'd'){
		numero = 13;
	}
	else if(c == 'e'){
		numero = 14;
	}
	else if(c == 'f'){
		numero = 15;
	}
	else if(isdigit(c)){
		numero = c - '0';
	}
	else{
		printf("Erro ao converter um hexadecimal para decimal\n");
		exit(EXIT_FAILURE);
	}

	return numero;
}

int toInt(char * numeroString) {
	int c, tamanho, numero;
	
	numero = 0;
	if(strlen(numeroString) > 2 && numeroString[0] == '0' && numeroString[1] == 'x'){

		numeroString += 2;
		tamanho = strlen(numeroString) - 1;

		while (numeroString != NULL){
			numero+= hexaToInt(numeroString[0]) * pow(16, tamanho);
			tamanho--;
		}
	}
	else{
		numero = atoi(numeroString);
		if(numero < 0 || numero > 1023){
			printf("A memoria nao pertence ao intervalo valido - Utilize 0 - 1023\n");
		}
	}
 return numero;
}

void atualizaPosicaoMemoria(char * enderecoMemoria){

	posicaoMemoriaAtual.linha = toInt(enderecoMemoria);
}

void processaLinhas(char ** controleLinhas, int qtdLinhas){

	int i, diretivaId;
	char * linhaQuebrada;
	RotuloEndereco * mapaRotulosEnderecos;
	const char * separadores = " 	\n";
	
	mapaRotulosEnderecos = NULL;

	for(i = 0; i < qtdLinhas; i++){

		printf("Conteudo --%s--\n", controleLinhas[i]);
		linhaQuebrada = strtok(controleLinhas[i], separadores);		
		stringToLower(linhaQuebrada);

		if(linhaQuebrada != NULL){
			/*Se for diretiva*/
			if(linhaQuebrada[0] == '.'){
					
				diretivaId = isDiretiva(&linhaQuebrada[1]);

				switch(diretivaId){
					/*org*/
					case 0:
						printf("%s\n", linhaQuebrada);
						//pega o endereco
						linhaQuebrada = strtok(NULL, separadores);
						stringToLower(linhaQuebrada);
						atualizaPosicaoMemoria(linhaQuebrada);
						break;
					/*align*/
					case 1:
						break;
					/*wfill*/
					case 2:
						break;
					/*word*/
					case 3:
						break;
					/*set*/
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
		}
	}

	/*while(mapaRotulosEnderecos != NULL){
		printf("%s\n", (* mapaRotulosEnderecos).rotulo);
		mapaRotulosEnderecos = (* mapaRotulosEnderecos).prox;
	}*/
}

int main(int argc, char *argv[]){

	char * codigoAssembly;
	char ** controleLinhas;
	int qtdLinhas;

	qtdLinhas = 0;
	codigoAssembly = NULL;
	controleLinhas = NULL;
	posicaoMemoriaAtual.linha = 0;
	posicaoMemoriaAtual.isEsquerda = 1;

    if (argc <= 1) {
	    printf("Informe pelo menos um arquivo de entrada.\n");
        exit(EXIT_FAILURE);
    }

    read_ASM_file(argv[1], &codigoAssembly, &controleLinhas, &qtdLinhas);
    processaLinhas(controleLinhas, qtdLinhas);

    free(codigoAssembly);
    free(controleLinhas);
    return 0;
}
