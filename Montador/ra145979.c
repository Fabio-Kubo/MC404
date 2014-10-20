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
	int linha;
	int instrucaoEsq;
	int parametroEsq;
	int instrucaoDir;
	int parametroDir;
	int isQuarentaBits;
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

Palavra * alocaPalavra(){

	Palavra * palavra;
	palavra = (Palavra *) malloc(sizeof(Palavra));

	if(palavra == NULL) {
		printf("Erro ao alocar uma nova palavra\n");
		exit(EXIT_FAILURE);
	}

	return palavra;
}

Palavra * alocaPalavraComInstrucaoEsq(int codInstrucao, int parametro){

	Palavra * palavra; 
	palavra = alocaPalavra();
	(* palavra).linha = posicaoMemoriaAtual.linha;
	(* palavra).instrucaoEsq = codInstrucao;
	(* palavra).parametroEsq = parametro;
	(* palavra).isQuarentaBits = 0;
	(* palavra).prox = NULL;

	return palavra;
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
		tamanho = strlen(numeroString);

		while (numeroString[0] != '\0'){
			tamanho--;
			numero+= hexaToInt(numeroString[0]) * pow(16, tamanho);
			numeroString++;
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
				printf("Erro na instrucao .word: posicao da memoria atual esta na instrucao da direita.\n");
				exit(EXIT_FAILURE);
			}

			linha = posicaoMemoriaAtual.linha + toInt(parametro);
			atualizaPosicaoMemoria(linha, 1);
			break;
		/*word*/
		case 3:
			if(posicaoMemoriaAtual.isEsquerda != 1){
				printf("Erro na instrucao .word: posicao da memoria atual esta na instrucao da direita.\n");
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

			if(linhaQuebrada[strlen(linhaQuebrada) - 1] == ':'){ /*Se for rotulo*/
				adicionaRotulo(linhaQuebrada);
				linhaQuebrada = strtok(NULL, separadores);
			}
			/*Se for diretiva*/
			else if(linhaQuebrada != NULL && linhaQuebrada[0] == '.'){
				diretivaId = isDiretiva(&linhaQuebrada[1]);
				linhaQuebrada = strtok(NULL, separadores);
				stringToLower(linhaQuebrada);
				atualizaPosicaoMemoriaDiretiva(diretivaId, linhaQuebrada);
			}
			else if(linhaQuebrada != NULL){ /*Se for instrucao*/
				if(posicaoMemoriaAtual.isEsquerda == 0){
					posicaoMemoriaAtual.linha++;
					posicaoMemoriaAtual.isEsquerda = 1;
				}
				else{
					posicaoMemoriaAtual.isEsquerda = 0;	
				}
			}
		}
	}

	while(mapaRotulosEnderecos != NULL){
		printf("%s linha: %d\n", (* mapaRotulosEnderecos).rotulo, (* mapaRotulosEnderecos).endereco.linha);
		mapaRotulosEnderecos = (* mapaRotulosEnderecos).prox;
	}
}

void adicionaDiretiva(int diretivaId, char * parametro){

}

void adicionaInstrucaoNasPalavras(int codInstrucao, int parametro){

	Palavra * anterior, * novo;
	if(palavras != NULL){
		
		anterior = NULL;
		while((* palavras).linha < posicaoMemoriaAtual.linha){
			anterior = palavras;
			palavras++;
		}

		if((* palavras).linha == posicaoMemoriaAtual.linha){

			if(posicaoMemoriaAtual.isEsquerda == 1){
				(* palavras).instrucaoEsq = codInstrucao;
				(* palavras).parametroEsq = parametro;
			}
			else{
				(* palavras).instrucaoDir = codInstrucao;
				(* palavras).parametroDir = parametro;
			}
		}
		else{
			novo = alocaPalavraComInstrucaoEsq(codInstrucao, parametro);
		 
			if (anterior == NULL){
				(* novo).prox = palavras;
				palavras = novo;
			}
			else{
				(* novo).prox = (* anterior).prox;
				(* anterior).prox = novo;
			}

		}
	}
	else{

		palavras = alocaPalavraComInstrucaoEsq(codInstrucao, parametro);
	}
}

Endereco retornaEnderecoParaInstrucao(char * parametro){
	Endereco endereco;
	RotuloEndereco * rotuloEndereco;

	if(parametro != NULL){

		/*Se for rotulo*/
		if(strlen(parametro) > 2 && parametro[0] == 'm' && parametro[0] == '('){
			endereco.linha = retornaNumeroSemFormatacao(parametro);
			endereco.isEsquerda = 1;
		}
		else{
			rotuloEndereco = retornaRotuloEndereco(parametro);
			endereco.linha = (* rotuloEndereco).endereco.linha;
			endereco.isEsquerda = (* rotuloEndereco).endereco.isEsquerda;
		}
	}
	return endereco;
}

/*Metodo que retorna o codigo da instrucao de acordo com o id(numero do sistema)
Observacao: ele so retorna aqueles que so possuem um unico codigo*/
int retornaCodigoInstrucaoSimples(int instrucaoId){

	int codInstrucao;
	
	switch(instrucaoId){
		/*load m(x)*/
		case 0:
			codInstrucao = 1;
			break;
		/*load -m(x)*/
		case 1:
			codInstrucao = 2;
			break;
		/*load |m(x)|*/
		case 2:
			codInstrucao = 3;
			break;
		/*load mq*/
		case 3:
			codInstrucao = 10;
			break;
		/*LOAD MQ,M(X)*/
		case 4:
			codInstrucao = 9;
			break;
		/*STOR M(X)*/
		case 5:
			codInstrucao = 33;
			break;
		/*ADD M(X)*/
		case 8:
			codInstrucao = 5;
			break;
		/*ADD |M(X)|*/
		case 9:
			codInstrucao = 7;
			break;
		/*SUB M(X)*/
		case 10:
			codInstrucao = 6;
			break;
		/*SUB |M(X)|*/
		case 11:
			codInstrucao = 8;
			break;
		/*MUL M(X)*/
		case 12:
			codInstrucao = 11;
			break;
		/*DIV M(X)*/
		case 13:
			codInstrucao = 12;
			break;
		/*LSH*/
		case 14:
			codInstrucao = 20;
			break;
		/*RSH*/
		case 15:
			codInstrucao = 21;
			break;
	}

	return codInstrucao;
}

void incrementaPosicaoMemoriaAtual(){
	
	if(posicaoMemoriaAtual.isEsquerda == 1){
		posicaoMemoriaAtual.isEsquerda = 0;
	}
	else{
		posicaoMemoriaAtual.linha++;
		posicaoMemoriaAtual.isEsquerda = 1;
	}
}

void processaInstrucao(int instrucaoId, char * parametro){

	int codInstrucao;
	Endereco endereco;

	if(instrucaoId < 0){
		printf("Instrucao Invalida\n");
		exit(EXIT_FAILURE);
	}
	
	switch(instrucaoId){
		/*load m(x)*/
		case 0:
		/*load -m(x)*/
		case 1:
		/*load |m(x)|*/
		case 2:
		/*load mq*/
		case 3:
		/*LOAD MQ,M(X)*/
		case 4:
		/*STOR M(X)*/
		case 5:
		/*ADD M(X)*/
		case 8:
		/*ADD |M(X)|*/
		case 9:
		/*SUB M(X)*/
		case 10:
		/*SUB |M(X)|*/
		case 11:
		/*MUL M(X)*/
		case 12:
		/*DIV M(X)*/
		case 13:	
			codInstrucao = retornaCodigoInstrucaoSimples(instrucaoId);
			endereco = retornaEnderecoParaInstrucao(parametro);
			break;
		/*JUMP*/
		case 6:
			endereco = retornaEnderecoParaInstrucao(parametro);

			if(endereco.isEsquerda == 1){
				codInstrucao = 13;
			}
			else{
				codInstrucao = 14;	
			}
			
			break;
			break;
		/*JUMP+*/
		case 7:
			endereco = retornaEnderecoParaInstrucao(parametro);

			if(endereco.isEsquerda == 1){
				codInstrucao = 15;
			}
			else{
				codInstrucao = 16;
			}

			break;
		/*LSH*/
		case 14:
		/*RSH*/
		case 15:
			codInstrucao = retornaCodigoInstrucaoSimples(instrucaoId);
			endereco.linha = 0;
			break;
		/*STOR M(X,8:19) ou STOR M(X,28:39)*/
		case 16:
			endereco = retornaEnderecoParaInstrucao(parametro);

			if(endereco.isEsquerda == 1){
				codInstrucao = 18;
			}
			else{
				codInstrucao = 19;
			}
			
			break;
	}

	adicionaInstrucaoNasPalavras(codInstrucao, endereco.linha);
	incrementaPosicaoMemoriaAtual();
}

void processaLinhas(char ** controleLinhas, int qtdLinhas){

	int i, diretivaId, instrucaoId;
	char * linhaQuebrada;
	
	mapaRotulosEnderecos = NULL;

	for(i = 0; i < qtdLinhas; i++){

		linhaQuebrada = strtok(controleLinhas[i], separadores);
		
		if(linhaQuebrada != NULL){
			stringToLower(linhaQuebrada);

			/*Se for rotulo, nao faz nada, apenas vai para o proximo item da linha*/
			if(linhaQuebrada[strlen(linhaQuebrada) - 1] == ':'){ 
				linhaQuebrada = strtok(NULL, separadores);
			}
			/*Se for diretiva*/
			else if(linhaQuebrada != NULL && linhaQuebrada[0] == '.'){
				diretivaId = isDiretiva(&linhaQuebrada[1]);
				linhaQuebrada = strtok(NULL, separadores);
				stringToLower(linhaQuebrada);
				adicionaDiretiva(diretivaId, linhaQuebrada);
			}
			/*Se for instrucao*/
			else if(linhaQuebrada != NULL){ 
				instrucaoId = isInstrucao(linhaQuebrada);
				linhaQuebrada = strtok(NULL, separadores);
				if(linhaQuebrada == NULL){
					printf("Deu ruim\n");
				}
				stringToLower(linhaQuebrada);
				processaInstrucao(instrucaoId, linhaQuebrada);
			}
		}
	}
}

int main(int argc, char *argv[]){

	char * codigoAssembly;
	char ** controleLinhas;
	int qtdLinhas, i;

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

    printf("1----------------------------\n");

    for (i = 0; i < qtdLinhas; i++){
    	printf("%s\n", controleLinhas[i]);
    }
	printf("----------------------------\n");

    mapeaRotulos(controleLinhas, qtdLinhas);

    printf("2----------------------------\n");

    for (i = 0; i < qtdLinhas; i++){
    	printf("%s\n", controleLinhas[i]);
    }
	printf("----------------------------\n");
    
    //processaLinhas(controleLinhas, qtdLinhas);

    free(codigoAssembly);
    free(controleLinhas);
    return 0;
}
