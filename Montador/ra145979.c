#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "readASM.h"

typedef struct Endereco{
	int linha;
	int isEsquerda;	
} Endereco;


typedef struct RotuloEndereco{
	char * rotulo;
	Endereco endereco;
	struct RotuloEndereco * prox;
} RotuloEndereco;

typedef struct Palavra{
	Endereco endereco;
	int conteudo;
	struct Palavra * prox;
} Palavra;

Endereco posicaoMemoriaAtual;
Palavra * palavras;
RotuloEndereco * mapaRotulosEnderecos;
const char * separadores = " 	\n";
	
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

void adicionaRotulo(char * novoRotulo){

	RotuloEndereco * novoRotuloEndereco, * ultimoRotulo;

	novoRotuloEndereco = alocaRotuloEndereco();

	(* novoRotuloEndereco).rotulo = novoRotulo;
	(* novoRotuloEndereco).endereco.linha = posicaoMemoriaAtual.linha;
	(* novoRotuloEndereco).endereco.isEsquerda = posicaoMemoriaAtual.isEsquerda;
	(* novoRotuloEndereco).prox = NULL;

	if(mapaRotulosEnderecos == NULL){
		mapaRotulosEnderecos = novoRotuloEndereco;
	}
	else{

		ultimoRotulo = mapaRotulosEnderecos;

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
	int tamanho, numero;
	
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
			exit(EXIT_FAILURE);
		}
	}
 return numero;
}

void atualizaPosicaoMemoria(int linha, int isEsquerda){

	posicaoMemoriaAtual.linha = linha;
	posicaoMemoriaAtual.isEsquerda = isEsquerda;
}

int isInstrucao(char * termo){

	int i;
	
	char instrucoes[17][8];
	strcpy(instrucoes[0], "ld");
	strcpy(instrucoes[1], "ld_n");
	strcpy(instrucoes[2], "ld_abs");
	strcpy(instrucoes[3], "ld_mq");
	strcpy(instrucoes[4], "ld_mqmx");
	strcpy(instrucoes[5], "st");
	strcpy(instrucoes[6], "jmp");
	strcpy(instrucoes[7], "jge");
	strcpy(instrucoes[8], "add");
	strcpy(instrucoes[9], "add_abs");
	strcpy(instrucoes[10], "sub");
	strcpy(instrucoes[11], "sub_abs");
	strcpy(instrucoes[12], "mul");
	strcpy(instrucoes[13], "div");
	strcpy(instrucoes[14], "lsh");
	strcpy(instrucoes[15], "rsh");
	strcpy(instrucoes[16], "st_addr");

	alteraParaMinusculo(termo);
	
	for (i = 0; i < 17; ++i) {

		if(strcmp(instrucoes[i], termo) == 0){
			return i;
		}
	}

	return -1;
}

int retornaCodigoOperacao(int instrucaoId){
	int codigo;

	switch(instrucaoId){
		/*load m(x)*/
		case 0:
			codigo = 1;
			break;
		/*load -m(x)*/
		case 1:
			codigo = 2;
			break;
		/*load |m(x)|*/
		case 2:
			codigo = 3;
			break;
		/*load mq*/
		case 3:
			codigo = 10;
			break;
		/*LOAD MQ,M(X)*/
		case 4:
			codigo = 9;
			break;
		/*STOR M(X)*/
		case 5:
			codigo = 33;
			break;
		/*JUMP*/
		case 6:
			//TODO
			break;
		/*JUMP+*/
		case 7:
			//TODO
			break;
		/*ADD M(X)*/
		case 8:
			codigo = 5;
			break;
		/*ADD |M(X)|*/
		case 9:
			codigo = 7;
			break;
		/*SUB M(X)*/
		case 10:
			codigo = 6;
			break;
		/*SUB |M(X)|*/
		case 11:
			codigo = 8;
			break;
		/*MUL M(X)*/
		case 12:
			codigo = 11;
			break;
		/*DIV M(X)*/
		case 13:
			codigo = 12;
			break;
		/*LSH*/
		case 14:
			codigo = 20;
			break;
		/*RSH*/
		case 15:
			codigo = 21;
			break;
		/*STOR M(X,8:19) ou STOR M(X,28:39)*/
		case 16:
			//TODO
			break;
	}

	return codigo;
}

int retornaNumeroSemFormatacao(char * numeroNoFormatoMParenteses){

	char * numero;
	int tamanho, i;

	tamanho = strlen(numeroNoFormatoMParenteses) - 2;
	numero = alocaVetorChar(tamanho);
	
	numeroNoFormatoMParenteses += 2;
	for (i = 0; i < (numeroNoFormatoMParenteses[0] != ')'); i++){
		numero[i] = numeroNoFormatoMParenteses[0];
		numeroNoFormatoMParenteses++;
	}

	numero[tamanho - 1] = '\0';

	return toInt(numero);
}

RotuloEndereco * retornaRotuloEndereco(char * rotulo){

	RotuloEndereco * rotuloAtual;

	rotuloAtual = mapaRotulosEnderecos;;
	while(rotuloAtual != NULL ){

		if(strcmp((* rotuloAtual).rotulo, rotulo) == 0){
			break;
		}

		rotuloAtual = (* rotuloAtual).prox;
	}

	return rotuloAtual;
}

int getEnderecoDaInstrucao(char * linhaQuebrada){

	RotuloEndereco * rotuloEndereco;
	int codigo = 0;

	if (linhaQuebrada != NULL){
		stringToLower(linhaQuebrada);

		if(linhaQuebrada[0] == 'm' && linhaQuebrada[1] == '('){
			
			codigo = retornaNumeroSemFormatacao(linhaQuebrada);
		}
		else{
			rotuloEndereco = retornaRotuloEndereco(linhaQuebrada);
			codigo = (* rotuloEndereco).endereco.linha;
		}
	}
	return codigo;
}

void adicionaPalavra(int codigoInstrucao, char * linhaQuebrada){
}

void processaInstrucao(char * linhaQuebrada){

	int instrucaoId;

	instrucaoId = isInstrucao(linhaQuebrada);

	if(instrucaoId < 0){
		printf("Instrucao Invalida\n");
		exit(EXIT_FAILURE);
	}

	linhaQuebrada = strtok(NULL, separadores);
	
	switch(instrucaoId){
		/*load m(x)*/
		case 0:
			adicionaPalavra(1, linhaQuebrada);
			break;
		/*load -m(x)*/
		case 1:
			adicionaPalavra(2, linhaQuebrada);
			break;
		/*load |m(x)|*/
		case 2:
			adicionaPalavra(3, linhaQuebrada);
			break;
		/*load mq*/
		case 3:
			adicionaPalavra(10, linhaQuebrada);
			break;
		/*LOAD MQ,M(X)*/
		case 4:
			adicionaPalavra(9, linhaQuebrada);
			break;
		/*STOR M(X)*/
		case 5:
			adicionaPalavra(33, linhaQuebrada);
			break;
		/*JUMP*/
		case 6:
			//TODO
			break;
		/*JUMP+*/
		case 7:
			//TODO
			break;
		/*ADD M(X)*/
		case 8:
			adicionaPalavra(5, linhaQuebrada);
			break;
		/*ADD |M(X)|*/
		case 9:
			adicionaPalavra(7, linhaQuebrada);
			break;
		/*SUB M(X)*/
		case 10:
			adicionaPalavra(6, linhaQuebrada);
			break;
		/*SUB |M(X)|*/
		case 11:
			adicionaPalavra(8, linhaQuebrada);
			break;
		/*MUL M(X)*/
		case 12:
			adicionaPalavra(11, linhaQuebrada);
			break;
		/*DIV M(X)*/
		case 13:
			adicionaPalavra(12, linhaQuebrada);
			break;
		/*LSH*/
		case 14:
			adicionaPalavra(20, linhaQuebrada);
			break;
		/*RSH*/
		case 15:
			adicionaPalavra(21, linhaQuebrada);
			break;
		/*STOR M(X,8:19) ou STOR M(X,28:39)*/
		case 16:
			//TODO
			break;
	}
}

void atualizaPosicaoMemoriaDiretiva(int diretivaId, char * parametro){

	int linha, i, aux;

	switch(diretivaId){
		/*org*/
		case 0:
			atualizaPosicaoMemoria(toInt(parametro), 1);
			break;
		/*align*/
		case 1:
			aux = toInt(parametro);
			for (i = posicaoMemoriaAtual.linha; i < 1024; ++i){
				if(i % aux){
					break;
				}
			}
			if(i == 1024){
				printf("Erro na instrucao .align: calculo do endereco\n");
				exit(EXIT_FAILURE);
			}
			atualizaPosicaoMemoria(i, 1);
			break;
		/*wfill*/
		case 2:
			if(posicaoMemoriaAtual.isEsquerda != 1){
				printf("Erro na instrucao .word: posicao da memoria atual esta na palavra da direita.\n");
				exit(EXIT_FAILURE);
			}

			linha = posicaoMemoriaAtual.linha + toInt(parametro);
			atualizaPosicaoMemoria(linha, 1);
			break;
		/*word*/
		case 3:
			if(posicaoMemoriaAtual.isEsquerda != 1){
				printf("Erro na instrucao .word: posicao da memoria atual esta na palavra da direita.\n");
				exit(EXIT_FAILURE);
			}
			linha = posicaoMemoriaAtual.linha + 1;

			if(linha > 1023){
				printf("Erro na instrucao .word: Overflow de memoria.\n");
				exit(EXIT_FAILURE);
			}

			atualizaPosicaoMemoria(linha, 1);
			break;
		/*set - nao influencia na memoria*/
		case 4:
			break;
		default:
			printf("Diretiva incorreta\n");
			exit(EXIT_FAILURE);
	}
}

void mapeaRotulos(char ** controleLinhas, int qtdLinhas){

	int i, diretivaId;
	char * linhaQuebrada;
	
	mapaRotulosEnderecos = NULL;

	for(i = 0; i < qtdLinhas; i++){

		linhaQuebrada = strtok(controleLinhas[i], separadores);		
		
		if(linhaQuebrada != NULL){
			stringToLower(linhaQuebrada);

			/*Se for diretiva*/
			if(linhaQuebrada[0] == '.'){
					
				diretivaId = isDiretiva(&linhaQuebrada[1]);
				linhaQuebrada = strtok(NULL, separadores);
				stringToLower(linhaQuebrada);
				atualizaPosicaoMemoriaDiretiva(diretivaId, linhaQuebrada);
			}
			else if(linhaQuebrada[strlen(linhaQuebrada) - 1] == ':'){ /*Se for rotulo*/
				adicionaRotulo(linhaQuebrada);
			}
			else{ /*Nesse caso, eh uma instrucao*/

				processaInstrucao(linhaQuebrada);
			}
		}
	}

	while(mapaRotulosEnderecos != NULL){
		printf("%s\n", (* mapaRotulosEnderecos).rotulo);
		mapaRotulosEnderecos = (* mapaRotulosEnderecos).prox;
	}
}

void processaLinhas(char ** controleLinhas, int qtdLinhas){

	int i, diretivaId;
	char * linhaQuebrada;
	
	mapaRotulosEnderecos = NULL;

	for(i = 0; i < qtdLinhas; i++){

		linhaQuebrada = strtok(controleLinhas[i], separadores);		
		
		if(linhaQuebrada != NULL){
			stringToLower(linhaQuebrada);

			/*Se for diretiva*/
			if(linhaQuebrada[0] == '.'){
					
				diretivaId = isDiretiva(&linhaQuebrada[1]);

				switch(diretivaId){
					/*org*/
					case 0:
						//pega o endereco
						linhaQuebrada = strtok(NULL, separadores);
						stringToLower(linhaQuebrada);
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
				adicionaRotulo(linhaQuebrada);
			}
			else{ /*Nesse caso, eh uma instrucao*/

				processaInstrucao(linhaQuebrada);
			}
		}
	}

	while(mapaRotulosEnderecos != NULL){
		printf("%s\n", (* mapaRotulosEnderecos).rotulo);
		mapaRotulosEnderecos = (* mapaRotulosEnderecos).prox;
	}
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
