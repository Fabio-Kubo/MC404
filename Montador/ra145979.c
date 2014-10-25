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

typedef struct Simbolo{
	char * nome;
	int valor;
	struct Simbolo * prox;
} Simbolo;

typedef struct Palavra{
	int linha;
	int instrucaoEsq;
	int parametroEsq;
	int instrucaoDir;
	int parametroDir;
	int isQuarentaBits;
	unsigned long long int valor;
	struct Palavra * prox;
} Palavra;

Endereco posicaoMemoriaAtual;
Palavra * palavras;
RotuloEndereco * mapaRotulosEnderecos;
Simbolo * mapaSimbolos;

const char * separadores = " 	,\n";
	
RotuloEndereco * alocaRotuloEndereco(){

	RotuloEndereco * rotuloEndereco;
	rotuloEndereco = (RotuloEndereco *) malloc(sizeof(RotuloEndereco));

	if(rotuloEndereco == NULL) {
		printf("Erro ao alocar o rotuloEndereco\n");
		exit(EXIT_FAILURE);
	}

	return rotuloEndereco;
}

Simbolo * alocaSimbolo(){

	Simbolo * novoSimbolo;
	novoSimbolo = (Simbolo *) malloc(sizeof(Simbolo));

	if(novoSimbolo == NULL) {
		printf("Erro ao alocar o novoSimbolo\n");
		exit(EXIT_FAILURE);
	}

	return novoSimbolo;
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
	(* palavra).instrucaoDir = 0;
	(* palavra).parametroDir = 0;
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

Simbolo * retornaSimbolo(char * nomeSimboloBuscado){

	Simbolo * simboloBuscado, * aux;

	simboloBuscado = NULL;

	aux = mapaSimbolos;
	while(aux != NULL){
		if(strcmp((* aux).nome, nomeSimboloBuscado) == 0){
			simboloBuscado = aux;
			break;
		}
		aux = (* aux).prox;
	}

	return simboloBuscado;
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
	int tamanho;

	novoRotuloEndereco = alocaRotuloEndereco();
	tamanho = strlen(novoRotulo) + 1;
	(* novoRotuloEndereco).rotulo = alocaVetorChar(tamanho);
	strcpy((* novoRotuloEndereco).rotulo, novoRotulo);
	(* novoRotuloEndereco).rotulo[tamanho - 2] = '\0';//retira os dois pontos(":")
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

void adicionaSimbolo(char * nomeSimbolo, int valor){

	Simbolo * novoSimbolo, * ultimoSimbolo;

	novoSimbolo = alocaSimbolo();

	(* novoSimbolo).nome = alocaVetorChar(strlen(nomeSimbolo) + 1);
	strcpy((* novoSimbolo).nome, nomeSimbolo);
	(* novoSimbolo).valor = valor;
	(* novoSimbolo).prox = NULL;

	if(mapaSimbolos == NULL){
		mapaSimbolos = novoSimbolo;
	}
	else{

		ultimoSimbolo = mapaSimbolos;

		while((* ultimoSimbolo).prox != NULL){
			ultimoSimbolo = (* ultimoSimbolo).prox;
		}

		(* ultimoSimbolo).prox = novoSimbolo;
	}
}

void adicionaPalavra(int instrucaoEsq, int parametroEsq, int instrucaoDir, int parametroDir, int isQuarentaBits, unsigned long long int valor){

	Palavra * novaPalavra, * aux, * anterior; 
	novaPalavra = alocaPalavra();
	(* novaPalavra).linha = posicaoMemoriaAtual.linha;
	(* novaPalavra).instrucaoEsq = instrucaoEsq;
	(* novaPalavra).parametroEsq = parametroEsq;
	(* novaPalavra).instrucaoDir = instrucaoDir;
	(* novaPalavra).parametroDir = parametroDir;
	(* novaPalavra).isQuarentaBits = isQuarentaBits;
	(* novaPalavra).valor = valor;
	(* novaPalavra).prox = NULL;

	if(palavras == NULL){
		palavras = novaPalavra;
	}
	else{
		aux = palavras;
		anterior = NULL;
		
		while(aux != NULL && (* aux).linha < posicaoMemoriaAtual.linha){
			anterior = aux;
			aux = (* aux).prox;
		}
	 
		if (anterior == NULL){
			(* novaPalavra).prox = palavras;
			palavras = novaPalavra;
		}
		else{
			(* novaPalavra).prox = (* anterior).prox;
			(* anterior).prox = novaPalavra;
		}
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

void validaEnderecoPalavra(int decimal){

	if(decimal < 0 || decimal > 1023){
		printf("A memoria nao pertence ao intervalo valido - Utilize 0 - 1023\n");
		exit(EXIT_FAILURE);
	}
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
	}
 return numero;
}

unsigned long long int toLongLongInt(char * numeroString) {
	int tamanho;

	unsigned long long int numero;
	
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

	char * numeroString;
	int tamanho, i, numero;

	tamanho = strlen(numeroNoFormatoMParenteses) - 2;
	numeroString = alocaVetorChar(tamanho);
	
	numeroNoFormatoMParenteses += 2;
	for (i = 0; numeroNoFormatoMParenteses[0] != ')'; i++){
		numeroString[i] = numeroNoFormatoMParenteses[0];
		numeroNoFormatoMParenteses++;
	}

	numeroString[i] = '\0';

	numero = toInt(numeroString);
	free(numeroString);
	return numero;
}

RotuloEndereco * retornaRotuloEndereco(char * rotulo){

	RotuloEndereco * rotuloAtual;

	rotuloAtual = mapaRotulosEnderecos;

	while(rotuloAtual != NULL ){
		if(strcmp((* rotuloAtual).rotulo, rotulo) == 0){
			break;
		}

		rotuloAtual = (* rotuloAtual).prox;
	}

	return rotuloAtual;
}

int retornaLinhaEnderecoAlign(int numero){

	int linha;
	linha = posicaoMemoriaAtual.isEsquerda? posicaoMemoriaAtual.linha: posicaoMemoriaAtual.linha + 1;
	for (; linha < 1024; linha++){
		if((linha % numero) == 0){
			break;
		}
	}
	if(linha == 1024){
		printf("Erro na instrucao .align: calculo do endereco\n");
		exit(EXIT_FAILURE);
	}

	return linha;
}

void processamentoInicialDiretivas(int diretivaId, char * parametro){

	int linha, aux;
	char * nomeSimbolo;

	switch(diretivaId){
		/*org*/
		case 0:
			printf(".org\n");
			atualizaPosicaoMemoria(toInt(parametro), 1);
			break;
		/*align*/
		case 1:
			printf(".align %s\n", parametro);
			aux = toInt(parametro);
			atualizaPosicaoMemoria(retornaLinhaEnderecoAlign(aux), 1);
			break;
		/*wfill*/
		case 2:
			printf(".wfill %s\n", parametro);
			if(posicaoMemoriaAtual.isEsquerda != 1){
				printf("Erro na instrucao .word: posicao da memoria atual esta na instrucao da direita.\n");
				exit(EXIT_FAILURE);
			}

			linha = posicaoMemoriaAtual.linha + toInt(parametro);
			atualizaPosicaoMemoria(linha, 1);
			break;
		/*word*/
		case 3:
			printf(".word %s\n", parametro);
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
		/*.set*/
		case 4:
			printf(".set %s\n", parametro);
			nomeSimbolo = parametro;
			parametro = strtok(NULL, separadores);
			adicionaSimbolo(nomeSimbolo, toInt(parametro));
			break;
		default:
			printf("Diretiva incorreta\n");
			exit(EXIT_FAILURE);
	}
}

void mapeaRotulosEDiretivaSet(char ** controleLinhas, int qtdLinhas){

	int i, diretivaId;
	char * linhaQuebrada;
	char * copiaLinha;

	for(i = 0; i < qtdLinhas; i++){

		if(controleLinhas[i] != NULL){

			stringToLower(controleLinhas[i]);
			copiaLinha = alocaVetorChar(strlen(controleLinhas[i]) + 1);
			strcpy(copiaLinha, controleLinhas[i]);

			linhaQuebrada = strtok(copiaLinha, separadores);		
			
			if(linhaQuebrada != NULL){

				if(linhaQuebrada[strlen(linhaQuebrada) - 1] == ':'){ /*Se for rotulo*/
					adicionaRotulo(linhaQuebrada);
					linhaQuebrada = strtok(NULL, separadores);
				}
				/*Se for diretiva*/
				if(linhaQuebrada != NULL && linhaQuebrada[0] == '.'){
					diretivaId = isDiretiva(&linhaQuebrada[1]);
					linhaQuebrada = strtok(NULL, separadores);
					processamentoInicialDiretivas(diretivaId, linhaQuebrada);
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

			free(copiaLinha);
		}
	}
}

unsigned long long int retornaIntParametro(char * parametro){

	unsigned long long int valor = 0;
	RotuloEndereco * rotuloEndereco;
	Simbolo * simbolo;

	if(parametro == NULL){
		printf("Erro no processamento do parametro\n");
		exit(EXIT_FAILURE);
	}

	simbolo = retornaSimbolo(parametro);
	if(simbolo != NULL){
		valor = (* simbolo).valor;
	}
	else{

		rotuloEndereco = retornaRotuloEndereco(parametro);
		
		if(rotuloEndereco != NULL){
			valor = (* rotuloEndereco).endereco.linha;
		}
		else{
			valor = toInt(parametro);
		}

	}

	return valor;
}

void processaDiretiva(int diretivaId, char * parametro){

	int linha, i, aux;

	switch(diretivaId){
		/*org*/
		case 0:
			atualizaPosicaoMemoria(toInt(parametro), 1);
			break;
		/*align*/
		case 1:
			aux = toInt(parametro);
			atualizaPosicaoMemoria(retornaLinhaEnderecoAlign(aux), 1);
			break;
		/*wfill*/
		case 2:
			aux = toInt(parametro);
			parametro = strtok(NULL, separadores);
			
			for (i = 0; i < aux; i++){
				adicionaPalavra(0, 0, 0, 0, 1, retornaIntParametro(parametro));
			}
			linha = posicaoMemoriaAtual.linha + aux;
			atualizaPosicaoMemoria(linha, 1);
			break;
		/*word*/
		case 3:
			adicionaPalavra(0, 0, 0, 0, 1, retornaIntParametro(parametro));
			linha = posicaoMemoriaAtual.linha + 1;
			atualizaPosicaoMemoria(linha, 1);
			break;
	}
}

void adicionaInstrucaoNasPalavras(int codInstrucao, int parametro){

	Palavra * anterior, * novo, * aux;
	
	if(palavras == NULL){
		palavras = alocaPalavraComInstrucaoEsq(codInstrucao, parametro);
	}
	else{
		aux = palavras;
		anterior = NULL;
		
		while(aux != NULL && (* aux).linha < posicaoMemoriaAtual.linha){
			anterior = aux;
			aux = (* aux).prox;
		}

		if(aux != NULL && (* aux).linha == posicaoMemoriaAtual.linha){

			if(posicaoMemoriaAtual.isEsquerda == 1){
				(* aux).instrucaoEsq = codInstrucao;
				(* aux).parametroEsq = parametro;
			}
			else{
				(* aux).instrucaoDir = codInstrucao;
				(* aux).parametroDir = parametro;
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
}

Endereco retornaEnderecoParametro(char * parametro){
	Endereco endereco;
	RotuloEndereco * rotuloEndereco;
	Simbolo * simbolo;


	simbolo = retornaSimbolo(parametro);
	if(simbolo != NULL){
		endereco.linha = (* simbolo).valor;
		endereco.isEsquerda = 1;
	}
	else if(strlen(parametro) > 2 && parametro[0] == 'm' && parametro[1] == '('){
		endereco.linha = retornaNumeroSemFormatacao(parametro);
		endereco.isEsquerda = 1;
	}
	else{
		rotuloEndereco = retornaRotuloEndereco(parametro);
		if(rotuloEndereco == NULL){
			printf("Parametro incorreto: %s\n", parametro);
		}
		else{
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
			endereco = retornaEnderecoParametro(parametro);
			break;
		/*JUMP*/
		case 6:
			endereco = retornaEnderecoParametro(parametro);

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
			endereco = retornaEnderecoParametro(parametro);

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
			endereco = retornaEnderecoParametro(parametro);

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

void processamentoCodigoPrincipal(char ** controleLinhas, int qtdLinhas){

	int i, diretivaId, instrucaoId;
	char * linhaQuebrada;

	for(i = 0; i < qtdLinhas; i++){

		linhaQuebrada = strtok(controleLinhas[i], separadores);
		
		if(linhaQuebrada != NULL){

			/*Se for rotulo, nao faz nada, apenas vai para o proximo item da linha*/
			if(linhaQuebrada[strlen(linhaQuebrada) - 1] == ':'){
				linhaQuebrada = strtok(NULL, separadores);
			}
			/*Se for diretiva*/
			if(linhaQuebrada != NULL && linhaQuebrada[0] == '.'){
				diretivaId = isDiretiva(&linhaQuebrada[1]);

				//.set foi processado na primeira
				if(diretivaId != 4){
					linhaQuebrada = strtok(NULL, separadores);
					processaDiretiva(diretivaId, linhaQuebrada);
				}
			}
			/*Se for instrucao*/
			else if(linhaQuebrada != NULL){
				instrucaoId = isInstrucao(linhaQuebrada);
				linhaQuebrada = strtok(NULL, separadores);
				processaInstrucao(instrucaoId, linhaQuebrada);
			}
			}
	}
}

void imprimePalavras(){
	Palavra * aux;

	aux = palavras;

	while(aux != NULL){
		printf("%X ", (* aux).linha);
		printf("%d ", (* aux).instrucaoEsq);
		printf("%X ", (* aux).parametroEsq);
		printf("%X ", (* aux).instrucaoDir);
		printf("%X\n", (* aux).parametroDir);

		aux = (* aux).prox;
	}
}

void liberaMemoriaDasEstruturas(){

	Simbolo * auxSimbolo;
	RotuloEndereco * auxRotulo;
	Palavra * aux;

	while(mapaSimbolos != NULL){
		auxSimbolo = (* mapaSimbolos).prox;
		free((* mapaSimbolos).nome);
		free(mapaSimbolos);
		mapaSimbolos = auxSimbolo;
	}

	while(mapaRotulosEnderecos != NULL){
		auxRotulo = (* mapaRotulosEnderecos).prox;
		free((* mapaRotulosEnderecos).rotulo);
		free(mapaRotulosEnderecos);
		mapaRotulosEnderecos = auxRotulo;
	}

	while(palavras != NULL){
		aux = (* palavras).prox;
		free(palavras);
		palavras = aux;
	}
}

char * retornaHexadecimalComZerosAEsquerda(int numero, int tamanho){

	char * hexaFormatado, * auxNumero;
	int iAuxNumero, iNumeroFormatado;

	hexaFormatado = alocaVetorChar(tamanho + 1);
	auxNumero = alocaVetorChar(tamanho + 1);

	sprintf(auxNumero, "%X", numero);

	iAuxNumero = strlen(auxNumero) - 1;
	for (iNumeroFormatado = tamanho - 1; iNumeroFormatado >= 0; iNumeroFormatado--){
		
		if(iAuxNumero < 0){
			hexaFormatado[iNumeroFormatado] = '0';
		}
		else{
			hexaFormatado[iNumeroFormatado] = auxNumero[iAuxNumero];
			iAuxNumero--;
		}
	}

	hexaFormatado[tamanho] = '\0';

	free(auxNumero);

	return hexaFormatado;
}

void imprimeArquivo(FILE * arqSaida){

	Palavra * aux;
	char * linhaMapaFormatado, * numeroHexadecimal, * numeroLinha;
	int iLinhaMapaFormatado, iNumeroHexa;
	linhaMapaFormatado = alocaVetorChar(14);
	linhaMapaFormatado[0] = '\0';

	aux = palavras;
	while(aux != NULL){

		if((* aux).isQuarentaBits == 1){
			numeroHexadecimal = alocaVetorChar(11);
			sprintf(numeroHexadecimal, "%X", (* aux).instrucaoEsq);

			iNumeroHexa = strlen(numeroHexadecimal) - 1;
			
			for (iLinhaMapaFormatado = 12; iLinhaMapaFormatado >= 0; iLinhaMapaFormatado--){

				if(iLinhaMapaFormatado == 2 || iLinhaMapaFormatado == 6|| iLinhaMapaFormatado == 9){
					linhaMapaFormatado[iLinhaMapaFormatado] = ' ';
				}
				else if(iNumeroHexa < 0){
					linhaMapaFormatado[iLinhaMapaFormatado] = '0';
				}
				else{
					linhaMapaFormatado[iLinhaMapaFormatado] = numeroHexadecimal[iNumeroHexa];
					iNumeroHexa --;
				}
			}

			linhaMapaFormatado[13] = '\0';
			free(numeroHexadecimal);
		}
		else{

			printf("Onde ta dando pau A\n");
			numeroHexadecimal = retornaHexadecimalComZerosAEsquerda((* aux).instrucaoEsq, 2);

			strcat(linhaMapaFormatado, numeroHexadecimal);
			free(numeroHexadecimal);
			
			strcat(linhaMapaFormatado, " ");
			
			printf("Onde ta dando pau B\n");
			numeroHexadecimal = retornaHexadecimalComZerosAEsquerda((* aux).parametroEsq, 3);
			strcat(linhaMapaFormatado, numeroHexadecimal);
			free(numeroHexadecimal);

			strcat(linhaMapaFormatado, " ");
			
			numeroHexadecimal = retornaHexadecimalComZerosAEsquerda((* aux).instrucaoDir, 2);
			strcat(linhaMapaFormatado, numeroHexadecimal);
			free(numeroHexadecimal);
			
			strcat(linhaMapaFormatado, " ");
			
			numeroHexadecimal = retornaHexadecimalComZerosAEsquerda((* aux).parametroDir, 3);
			strcat(linhaMapaFormatado, numeroHexadecimal);
			free(numeroHexadecimal);
		}

		numeroLinha = retornaHexadecimalComZerosAEsquerda((* aux).linha, 3);
		fprintf(arqSaida, "%s ", numeroLinha);
		free(numeroLinha);
		fprintf(arqSaida, "%s\n", linhaMapaFormatado);
		aux = (* aux).prox;
	}
		free(linhaMapaFormatado);
}

int main(int argc, char *argv[]){

	char * codigoAssembly,* nomeArqSaida;
	char ** controleLinhas;
	int qtdLinhas, alocou;
	FILE * arqSaida;

	qtdLinhas = 0;
	alocou = 0;
	codigoAssembly = NULL;
	controleLinhas = NULL;
	posicaoMemoriaAtual.linha = 0;
	posicaoMemoriaAtual.isEsquerda = 1;
	mapaRotulosEnderecos = NULL;
	palavras = NULL;

    if (argc <= 1) {
	    printf("Informe pelo menos um arquivo de entrada.\n");
        exit(EXIT_FAILURE);
    }

    if(argc == 2){
    	nomeArqSaida = alocaVetorChar(strlen(argv[1]) + 5);
    	strcpy(nomeArqSaida, argv[1]);
    	strcat(nomeArqSaida, ".hex\0");
    	alocou = 1;
    }
    else{
    	nomeArqSaida = argv[2];
    }

    read_ASM_file(argv[1], &codigoAssembly, &controleLinhas, &qtdLinhas);
    mapeaRotulosEDiretivaSet(controleLinhas, qtdLinhas);

    //re-inicializa a posicao de memoria
	posicaoMemoriaAtual.linha = 0;
	posicaoMemoriaAtual.isEsquerda = 1;

    processamentoCodigoPrincipal(controleLinhas, qtdLinhas);
    arqSaida = fopen(nomeArqSaida,"w");
    
    //imprimeArquivo(arqSaida);

    imprimePalavras();	

    fclose(arqSaida);
    liberaMemoriaDasEstruturas();

    if(alocou){
    	free(nomeArqSaida);
    }
	free(codigoAssembly);
    free(controleLinhas);
    return 0;
}
