#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "readASM.h"
#include "ConversorBase.h"
#include "Montador.h"

const char * separadores = " 	,\n";

/*Metodo que aloca um rotulo endereco*/
Rotulo * alocaRotulo(){

	Rotulo * rotulo;

	rotulo = (Rotulo *) malloc(sizeof(Rotulo));
	if(rotulo == NULL) {
		printf("Erro ao alocar o rotulo\n");
		exit(EXIT_FAILURE);
	}

	return rotulo;
}

/*Metodo que aloca um simbolo*/
Simbolo * alocaSimbolo(){

	Simbolo * novoSimbolo;

	novoSimbolo = (Simbolo *) malloc(sizeof(Simbolo));
	if(novoSimbolo == NULL) {
		printf("Erro ao alocar o novoSimbolo\n");
		exit(EXIT_FAILURE);
	}

	return novoSimbolo;
}

/*Metodo que aloca uma palavra*/
Palavra * alocaPalavra(){

	Palavra * palavra;

	palavra = (Palavra *) malloc(sizeof(Palavra));
	if(palavra == NULL) {
		printf("Erro ao alocar uma nova palavra\n");
		exit(EXIT_FAILURE);
	}

	return palavra;
}

/*Metodo que aloca e inicializa o rotulo*/
Rotulo * alocaEInicializaRotulo(char * nomeRotulo){

	int tamanho;
	Rotulo * novoRotulo;

	novoRotulo = alocaRotulo();
	tamanho = strlen(nomeRotulo);

	(* novoRotulo).nome = alocaVetorChar(tamanho + 1);
	strcpy((* novoRotulo).nome, nomeRotulo);
	(* novoRotulo).nome[tamanho - 1] = '\0'; //retira os dois pontos(":")
	(* novoRotulo).endereco.linha = posicaoMemoriaAtual.linha;
	(* novoRotulo).endereco.isEsquerda = posicaoMemoriaAtual.isEsquerda;
	(* novoRotulo).prox = NULL;

	return novoRotulo;
}

/*Metodo que valida o tamanho do nome do rotulo*/
void validaTamanhoNomeRotulo(char * nmRotulo){

	if(strlen(nmRotulo) >= 64){
		printf("Erro: foi especificado um rotulo com mais de 64 caracteres(incluindo os ':').\n");
		exit(EXIT_FAILURE);
	}
}

void validaCaracteresRotuloOuSimbolo(char * nmRotulo){
	
	int codAsc, i;

	if(isdigit(nmRotulo[0])){
		printf("Erro: rotulo e simbolo nao podem iniciar com numero. Digitado: %s\n", nmRotulo);
		exit(EXIT_FAILURE);
	}
	else{

		for (i = 0; i < strlen(nmRotulo); i++){
			
			if(!isdigit(nmRotulo[i])){
				codAsc = nmRotulo[i];

				if(nmRotulo[i] !='_' && (codAsc < 65 || codAsc > 90)){
					printf("Erro: rotulo e simbolo so podem conter alfanumericos e underscores. Digitado: %s\n", nmRotulo);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
}

void adicionaRotulo(Rotulo * novoRotulo){

	Rotulo * ultimoRotulo;

	validaTamanhoNomeRotulo((* novoRotulo).nome);
	validaCaracteresRotuloOuSimbolo((* novoRotulo).nome);

	if(listaRotulos == NULL){
		listaRotulos = novoRotulo;
	}
	else{
		ultimoRotulo = listaRotulos;

		while((* ultimoRotulo).prox != NULL){
			ultimoRotulo = (* ultimoRotulo).prox;
		}

		(* ultimoRotulo).prox = novoRotulo;
	}
}

/*Metodo que retorna um rotulo de acordo com o nome*/
Rotulo * retornaRotulo(char * rotulo){

	Rotulo * rotuloAtual;

	rotuloAtual = listaRotulos;

	while(rotuloAtual != NULL ){
		if(strcmp((* rotuloAtual).nome, rotulo) == 0){
			break;
		}

		rotuloAtual = (* rotuloAtual).prox;
	}

	return rotuloAtual;
}

/*Metodo que aloca e inicializa um simbolo*/
Simbolo * alocaEinicializaSimbolo(char * nomeSimbolo, char * valor){
	
	Simbolo * novoSimbolo;

	novoSimbolo = alocaSimbolo();

	(* novoSimbolo).nome = alocaVetorChar(strlen(nomeSimbolo) + 1);
	strcpy((* novoSimbolo).nome, nomeSimbolo);
	(* novoSimbolo).valor = valor;
	(* novoSimbolo).prox = NULL;

	return novoSimbolo;
}

/*Metodo que adiciona um simbolo na lista de simbolos*/
void adicionaSimbolo(Simbolo * novoSimbolo){

	Simbolo * ultimoSimbolo;

	validaCaracteresRotuloOuSimbolo((* novoSimbolo).nome);
	if(listaSimbolos == NULL){
		listaSimbolos = novoSimbolo;
	}
	else{

		ultimoSimbolo = listaSimbolos;

		while((* ultimoSimbolo).prox != NULL){
			ultimoSimbolo = (* ultimoSimbolo).prox;
		}

		(* ultimoSimbolo).prox = novoSimbolo;
	}
}

/*Metodo que retorna um simbolo de acordo com o nome*/
Simbolo * retornaSimbolo(char * nomeSimboloBuscado){

	Simbolo * simboloBuscado;

	simboloBuscado = listaSimbolos;
	while(simboloBuscado != NULL){
		if(strcmp((* simboloBuscado).nome, nomeSimboloBuscado) == 0){
			break;
		}
		simboloBuscado = (* simboloBuscado).prox;
	}

	return simboloBuscado;
}

/*Metodo que aloca uma palavra e inicializa com os valores dos parametros*/
Palavra * alocaEInicializaPalavra(int instrucaoEsq, int parametroEsq, int instrucaoDir, int parametroDir, int isQuarentaBits, char * valor){
	
	Palavra * novaPalavra;

	novaPalavra = alocaPalavra();
	
	(* novaPalavra).linha = posicaoMemoriaAtual.linha;
	(* novaPalavra).instrucaoEsq = instrucaoEsq;
	(* novaPalavra).parametroEsq = parametroEsq;
	(* novaPalavra).instrucaoDir = instrucaoDir;
	(* novaPalavra).parametroDir = parametroDir;
	(* novaPalavra).isQuarentaBits = isQuarentaBits;
	(* novaPalavra).valorQuarentaBits = valor;
	(* novaPalavra).prox = NULL;

	return novaPalavra;
}

/*Metodo que aloca uma palavra, inicializando com valores da instrucao da esquerda*/
Palavra * alocaEInicializaPalavraComInstrucaoEsq(int instrucaoEsq, int parametro){

	Palavra * palavra;

	palavra = alocaEInicializaPalavra(instrucaoEsq, parametro, 0, 0, 0, NULL);

	return palavra;
}

/*Metodo que valida o range de enderecos*/
void validaIntervaloEnderecoPalavra(int decimal){

	if(decimal < 0 || decimal > 1023){
		printf("A memoria nao pertence ao intervalo valido - Utilize 0 - 1023\n");
		exit(EXIT_FAILURE);
	}
}

/*Metodo que adiciona a palavra na lista de palavras*/
void adicionaPalavra(Palavra * novaPalavra){

	Palavra * aux, * anterior;

	validaIntervaloEnderecoPalavra((* novaPalavra).linha);

	if(listaPalavras == NULL){
		listaPalavras = novaPalavra;
	}
	else{
		aux = listaPalavras;
		anterior = NULL;
		
		while(aux != NULL && (* aux).linha < posicaoMemoriaAtual.linha){
			anterior = aux;
			aux = (* aux).prox;
		}
	 
		if (anterior == NULL){
			(* novaPalavra).prox = listaPalavras;
			listaPalavras = novaPalavra;
		}
		else{
			(* novaPalavra).prox = (* anterior).prox;
			(* anterior).prox = novaPalavra;
		}
	}
}

/*Metodo que coloca todas as letras da string em caixa alta*/
void stringToUpper(char * string){

	int i;

	if(string != NULL){
		
		for (i = 0; string[i] != '\0'; i++) {
			string[i] = toupper(string[i]);
		}
	}
}

/*Metodo que verifica se o termo eh uma diretiva. Caso seja, a funcao retorna o idSistema da diretiva, 
caso contrario, retorna -1*/
int isDiretiva(char * termo){

	int i;
	char diretivas[5][6];

	strcpy(diretivas[0], "ORG");
	strcpy(diretivas[1], "ALIGN");
	strcpy(diretivas[2], "WFILL");
	strcpy(diretivas[3], "WORD");
	strcpy(diretivas[4], "SET");
	
	for (i = 0; i < 5; ++i) {

		if(strcmp(diretivas[i], termo) == 0){
			return i;
		}
	}

	return -1;
}

/*Metodo que verifica se o termo eh uma instrucao. Caso seja, a funcao retorna o idSistema da instrucao, 
caso contrario, retorna -1*/
int isInstrucao(char * termo){

	int i;
	char instrucoes[17][8];
	
	strcpy(instrucoes[0], "LD");
	strcpy(instrucoes[1], "LD_N");
	strcpy(instrucoes[2], "LD_ABS");
	strcpy(instrucoes[3], "LD_MQ");
	strcpy(instrucoes[4], "LD_MQMX");
	strcpy(instrucoes[5], "ST");
	strcpy(instrucoes[6], "JMP");
	strcpy(instrucoes[7], "JGE");
	strcpy(instrucoes[8], "ADD");
	strcpy(instrucoes[9], "ADD_ABS");
	strcpy(instrucoes[10], "SUB");
	strcpy(instrucoes[11], "SUB_ABS");
	strcpy(instrucoes[12], "MUL");
	strcpy(instrucoes[13], "DIV");
	strcpy(instrucoes[14], "LSH");
	strcpy(instrucoes[15], "RSH");
	strcpy(instrucoes[16], "ST_ADDR");

	for (i = 0; i < 17; ++i) {

		if(strcmp(instrucoes[i], termo) == 0){
			return i;
		}
	}

	return -1;
}

/*Metodo que atualiza a posicao de memoria atual com os valores do parametro*/
void atualizaPosicaoMemoriaAtual(int linha, int isEsquerda){

	posicaoMemoriaAtual.linha = linha;
	posicaoMemoriaAtual.isEsquerda = isEsquerda;
}

/*Metodo que incrementa a posicao de memoria atual, ou seja, seta para o endereco da proxima instrucao*/
void incrementaPosicaoMemoriaAtual(){
	
	if(posicaoMemoriaAtual.isEsquerda == 1){
		atualizaPosicaoMemoriaAtual(posicaoMemoriaAtual.linha, 0);
	}
	else{
		atualizaPosicaoMemoriaAtual(posicaoMemoriaAtual.linha + 1, 1);
	}
}

/*Metodo que retorna o codigo da instrucao de acordo com o isSistema da instrucao
Observacao: ele so retorna daqueles que so possuem um unico codigo(que nao varia de acordo com a memoria)*/
int retornaCodigoInstrucao(int instrucaoId){

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

char * retornaNumeroSemFormatacao(char * numeroNoFormatoMParenteses){

	char * numeroString;
	int tamanho, i;

	tamanho = strlen(numeroNoFormatoMParenteses) - 2;
	numeroString = alocaVetorChar(tamanho);
	
	numeroNoFormatoMParenteses += 2;
	for (i = 0; numeroNoFormatoMParenteses[0] != ')'; i++){
		numeroString[i] = numeroNoFormatoMParenteses[0];
		numeroNoFormatoMParenteses++;
	}

	numeroString[i] = '\0';

	return numeroString;
}

/*Metodo que calcula o endereco da palavra seguindo as regras da instrucao .Align*/
int retornaEnderecoPalavraAlign(int numero){

	int linha;
	numero = numero == 0? 1: numero;
	linha = posicaoMemoriaAtual.isEsquerda? posicaoMemoriaAtual.linha: posicaoMemoriaAtual.linha + 1;
	for (; linha < 1024; linha++){
		if((linha % numero) == 0){
			break;
		}
	}
	if(linha == 1024){
		printf("Erro: Ao realizar a instrucao .Align, ultrapassou-se as 1023 palavras\n");
		exit(EXIT_FAILURE);
	}

	return linha;
}

void validaRangeAlign(int parametro, int min, int max){

	if(parametro < min || parametro > max){

		printf("Erro: existe pelo menos um parametro que nao esta dentro do range permitido\n");
		exit(EXIT_FAILURE);
	}
}

void processamentoInicialDiretivas(int diretivaId, char * parametro){

	int aux;
	char * nomeSimbolo;
	Simbolo * simbolo;

	switch(diretivaId){
		/*org*/
		case 0:
			if(isHexadecimal(parametro)){
				aux = stringHexadecimalToInt(parametro);
			}
			else if (isBinario(parametro)){
				aux = stringOctalToInt(parametro);
			}
			else if(isOctal(parametro)){
				aux = stringBinarioToInt(parametro);
			}
			else{
				aux = toInt(parametro, 0, 1023);
			}
			atualizaPosicaoMemoriaAtual(aux, 1);
			break;
		/*align*/
		case 1:
			if(isDecimal(parametro)){
				aux = toInt(parametro, 0, 1023);
			}
			else{
				printf("Erro: a instrucao .align so permite parametro na base 10\n");
				exit(EXIT_FAILURE);
			}
			atualizaPosicaoMemoriaAtual(retornaEnderecoPalavraAlign(aux), 1);
			break;
		/*wfill*/
		case 2:
			if(posicaoMemoriaAtual.isEsquerda != 1){
				printf("Erro: Instrucao .wfill: posicao da memoria atual esta na instrucao da direita.\n");
				exit(EXIT_FAILURE);
			}

			if(isDecimal(parametro)){
				aux = toInt(parametro, 1, 1023);
			}
			else{
				printf("Erro: a instrucao .wfill so permite valor na base 10 em seu primeiro parametro\n");
				exit(EXIT_FAILURE);
			}

			atualizaPosicaoMemoriaAtual(posicaoMemoriaAtual.linha + aux, 1);
			break;
		/*word*/
		case 3:
			if(posicaoMemoriaAtual.isEsquerda != 1){
				printf("Erro: Instrucao .word: posicao da memoria atual esta na instrucao da direita.\n");
				exit(EXIT_FAILURE);
			}
			atualizaPosicaoMemoriaAtual(posicaoMemoriaAtual.linha + 1, 1);
			break;
		/*.set*/
		case 4:
			nomeSimbolo = parametro;
			parametro = strtok(NULL, separadores);

			if(isDecimal(parametro)){
				toInt(parametro, pow(-2, 31), pow(2,31) - 1);
			}
			simbolo = alocaEinicializaSimbolo(nomeSimbolo, parametro);
			adicionaSimbolo(simbolo);
			break;
		default:
			printf("Erro: Diretiva nao foi encontrada\n");
			exit(EXIT_FAILURE);
	}
}

void mapeaRotulosEDiretivaSet(char ** controleLinhas, int qtdLinhas){

	int i, diretivaId;
	char * linhaQuebrada, * copiaLinha;
	Rotulo * rotulo;
	for(i = 0; i < qtdLinhas; i++){

		if(controleLinhas[i] != NULL){

			stringToUpper(controleLinhas[i]);
			copiaLinha = alocaVetorChar(strlen(controleLinhas[i]) + 1);
			strcpy(copiaLinha, controleLinhas[i]);

			linhaQuebrada = strtok(copiaLinha, separadores);		
			
			if(linhaQuebrada != NULL){

				if(linhaQuebrada[strlen(linhaQuebrada) - 1] == ':'){ /*Se for rotulo*/
					rotulo = alocaEInicializaRotulo(linhaQuebrada);
					adicionaRotulo(rotulo);
					linhaQuebrada = strtok(NULL, separadores);
				}
				if(linhaQuebrada != NULL){
					/*Se for diretiva*/
					if(linhaQuebrada[0] == '.'){
						diretivaId = isDiretiva(&linhaQuebrada[1]);
						linhaQuebrada = strtok(NULL, separadores);
						processamentoInicialDiretivas(diretivaId, linhaQuebrada);
					}
					/*Se for instrucao*/
					else{
						incrementaPosicaoMemoriaAtual();
					}
				}
			}

			free(copiaLinha);
		}
	}
}

void processaDiretiva(int diretivaId, char * parametro){

	int i, aux;
	char * linhaRotulo, * auxValor, * auxValorFormatado;
	Palavra * novaPalavra;
	Rotulo * rotulo;

	switch(diretivaId){
		/*org*/
		case 0:

			if(isHexadecimal(parametro)){
				aux = stringHexadecimalToInt(parametro);
			}
			else if (isBinario(parametro)){
				aux = stringOctalToInt(parametro);
			}
			else if(isOctal(parametro)){
				aux = stringBinarioToInt(parametro);
			}
			else{
				aux = toInt(parametro, 0, 1023);
			}
			atualizaPosicaoMemoriaAtual(aux, 1);
			break;
		/*align*/
		case 1:
			aux = toInt(parametro, 0, 1023);
			atualizaPosicaoMemoriaAtual(retornaEnderecoPalavraAlign(aux), 1);
			break;
		/*wfill*/
		case 2:
			aux = toInt(parametro, 1, 1023);

			parametro = strtok(NULL, separadores);
			rotulo = retornaRotulo(parametro);
			for (i = 0; i < aux; i++){
				if(rotulo != NULL){
					linhaRotulo = alocaVetorChar(5);
					sprintf(linhaRotulo, "%d", (* rotulo).endereco.linha);
					auxValor = toHexadecimal(linhaRotulo);
					auxValorFormatado = retornaHexadecimalFormatoQuarentaBits(auxValor);
					free(linhaRotulo);
					free(auxValor);
				}
				else{
					if(isDecimal(parametro)){
						toInt(parametro, pow(-2, 31), pow(2, 31) - 1); // realiza a validacao do range
					}
					auxValor = toHexadecimal(parametro);
					auxValorFormatado = retornaHexadecimalFormatoQuarentaBits(auxValor);
					free(auxValor);
				}
				novaPalavra = alocaEInicializaPalavra(0, 0, 0, 0, 1, auxValorFormatado);
				adicionaPalavra(novaPalavra);
				atualizaPosicaoMemoriaAtual(posicaoMemoriaAtual.linha + 1, 1);
			}
			break;
		/*word*/
		case 3:
			rotulo = retornaRotulo(parametro);;
			if(rotulo != NULL){
				linhaRotulo = alocaVetorChar(5);
				sprintf(linhaRotulo, "%d", (* rotulo).endereco.linha);
				auxValor = toHexadecimal(linhaRotulo);
				auxValorFormatado = retornaHexadecimalFormatoQuarentaBits(auxValor);
				free(linhaRotulo);
				free(auxValor);
			}
			else{

				if(isDecimal(parametro)){
					toInt(parametro, 0, pow(2, 32) - 1); // realiza a validacao do range
				}
				auxValor = toHexadecimal(parametro);
				auxValorFormatado = retornaHexadecimalFormatoQuarentaBits(auxValor);
				free(auxValor);
			}
			
			novaPalavra = alocaEInicializaPalavra(0, 0, 0, 0, 1, auxValorFormatado);
			adicionaPalavra(novaPalavra);

			atualizaPosicaoMemoriaAtual(posicaoMemoriaAtual.linha + 1, 1);
			break;
	}
}

void adicionaInstrucaoNasPalavras(int codInstrucao, int parametro){

	Palavra * anterior, * novo, * aux;
	
	if(listaPalavras == NULL){
		listaPalavras = alocaEInicializaPalavraComInstrucaoEsq(codInstrucao, parametro);
	}
	else{
		aux = listaPalavras;
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

			novo = alocaEInicializaPalavraComInstrucaoEsq(codInstrucao, parametro);
		 
			if (anterior == NULL){
				(* novo).prox = listaPalavras;
				listaPalavras = novo;
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
	Rotulo * rotuloEndereco;
	Simbolo * simbolo;
	char * aux;

	simbolo = retornaSimbolo(parametro);
	if(simbolo != NULL){
		endereco.linha = toDecimal((* simbolo).valor);
		endereco.isEsquerda = 1;
	}
	else if(strlen(parametro) > 2 && parametro[0] == 'M' && parametro[1] == '('){
		aux = retornaNumeroSemFormatacao(parametro);
		endereco.linha = toDecimal(aux);
		free(aux);
		endereco.isEsquerda = 1;
	}
	else{
		rotuloEndereco = retornaRotulo(parametro);
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
			codInstrucao = retornaCodigoInstrucao(instrucaoId);
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
		/*load mq*/
		case 3:
		/*LSH*/
		case 14:
		/*RSH*/
		case 15:
			codInstrucao = retornaCodigoInstrucao(instrucaoId);
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

	aux = listaPalavras;

	while(aux != NULL){
		printf("%X ", (* aux).linha);

		if((* aux).isQuarentaBits == 1){
			printf("%s\n", (* aux).valorQuarentaBits);
		}
		else{
			printf("%d ", (* aux).instrucaoEsq);
			printf("%X ", (* aux).parametroEsq);
			printf("%X ", (* aux).instrucaoDir);
			printf("%X\n", (* aux).parametroDir);
		}
		aux = (* aux).prox;
	}
}

/*Metodo que libera a memoria das estruturas principais utiliza no Montador*/
void liberaMemoriaDasEstruturas(){

	Simbolo * auxSimbolo;
	Rotulo * auxRotulo;
	Palavra * aux;

	while(listaSimbolos != NULL){
		auxSimbolo = (* listaSimbolos).prox;
		free((* listaSimbolos).nome);
		free(listaSimbolos);
		listaSimbolos = auxSimbolo;
	}

	while(listaRotulos != NULL){
		auxRotulo = (* listaRotulos).prox;
		free((* listaRotulos).nome);
		free(listaRotulos);
		listaRotulos = auxRotulo;
	}

	while(listaPalavras != NULL){
		aux = (* listaPalavras).prox;

		if((* listaPalavras).isQuarentaBits == 1){
			free((* listaPalavras).valorQuarentaBits);
		}
		free(listaPalavras);
		listaPalavras = aux;
	}
}

void imprimeArquivo(char * nomeArqSaida){

	Palavra * aux;
	char * numeroHexadecimal, * numeroLinha;
	FILE * arqSaida;

	arqSaida = fopen(nomeArqSaida,"w");
	
	aux = listaPalavras;
	while(aux != NULL){

		if((* aux).isQuarentaBits == 1){
			numeroLinha = retornaHexadecimalComZerosAEsquerda((* aux).linha, 3);
			fprintf(arqSaida, "%s ", numeroLinha);
			free(numeroLinha);
			fprintf(arqSaida, "%s\n", (* aux).valorQuarentaBits);
		}
		else{

			numeroLinha = retornaHexadecimalComZerosAEsquerda((* aux).linha, 3);
			fprintf(arqSaida, "%s ", numeroLinha);
			free(numeroLinha);
			
			numeroHexadecimal = retornaHexadecimalComZerosAEsquerda((* aux).instrucaoEsq, 2);
			fprintf(arqSaida, "%s ", numeroHexadecimal);
			free(numeroHexadecimal);
		
			numeroHexadecimal = retornaHexadecimalComZerosAEsquerda((* aux).parametroEsq, 3);
			fprintf(arqSaida, "%s ", numeroHexadecimal);
			free(numeroHexadecimal);
			
			numeroHexadecimal = retornaHexadecimalComZerosAEsquerda((* aux).instrucaoDir, 2);
			fprintf(arqSaida, "%s ", numeroHexadecimal);
			free(numeroHexadecimal);
			
			numeroHexadecimal = retornaHexadecimalComZerosAEsquerda((* aux).parametroDir, 3);
			fprintf(arqSaida, "%s\n", numeroHexadecimal);
			free(numeroHexadecimal);
		}
		aux = (* aux).prox;
	}

	fclose(arqSaida);
}

void inicializaPosicaoMemoriaAtual(){
    posicaoMemoriaAtual.linha = 0;
    posicaoMemoriaAtual.isEsquerda = 1;
}

char * retornaNomeArqSaida(char * nomeEntrada){
    
    char * nomeArqSaida;
    
    nomeArqSaida = alocaVetorChar(strlen(nomeEntrada) + 5);
    strcpy(nomeArqSaida, nomeEntrada);
    strcat(nomeArqSaida, ".hex\0");

    return nomeArqSaida;
}