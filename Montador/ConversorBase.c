#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "readASM.h"
#include "ConversorBase.h"

unsigned long long int retornaMaiorValorHexadecimal40Bits(){

	return 1099511627775;
}

/*Metodo que verifica se o digito eh valido para base hexadecimal. Caso seja, retorna seu valor decimal, 
caso contrario, retorna -1*/
int isDigitoHexadecimal(char digito){

	int isHexadecimal;

	isHexadecimal = -1;
	if(digito == 'A'){
		isHexadecimal = 10;
	}
	else if(digito == 'B'){
		isHexadecimal = 11;
	}
	else if(digito == 'C'){
		isHexadecimal = 12;
	}
	else if(digito == 'D'){
		isHexadecimal = 13;
	}
	else if(digito == 'E'){
		isHexadecimal = 14;
	}
	else if(digito == 'F'){
		isHexadecimal = 15;
	}
	else if(isdigit(digito)){
		isHexadecimal = digito - '0';
	}

	return isHexadecimal;
}

/*Metodo que verifica se o digito eh valido para base octal. Caso seja, retorna seu valor decimal, 
caso contrario, retorna -1*/
int isDigitoOctal(char digito){

	int isOctal, aux;

	isOctal = -1;
	if(isdigit(digito)){
		aux = digito - '0';
		if(aux < 8){
			isOctal = aux;
		}
	}

	return isOctal;
}

/*Metodo que verifica se o digito eh valido para base binaria. Caso seja, retorna seu valor decimal, 
caso contrario, retorna -1*/
int isDigitoBinario(char digito){

	int isBinario, aux;

	isBinario = -1;
	if(isdigit(digito)){
		aux = digito - '0';
		if(aux < 2){
			isBinario = aux;
		}
	}

	return isBinario;
}

/*Metodo que retorna o valor decimal de um digito hexadecimal*/
int hexaToInt(char c){
	
	int numero;

	if(c == 'A'){
		numero = 10;
	}
	else if(c == 'B'){
		numero = 11;
	}
	else if(c == 'C'){
		numero = 12;
	}
	else if(c == 'D'){
		numero = 13;
	}
	else if(c == 'E'){
		numero = 14;
	}
	else if(c == 'F'){
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

/*Metodo que valida se todos os caracteres sao validos para a base hexadecimal*/
void validaNumeroHexadecimal(char * numero){

	int i;

	for (i = 0; i < strlen(numero); i++){
		if(isDigitoHexadecimal(numero[i]) == -1){
			printf("Erro: o numero %s possui um digito nao valido para a base hexadecimal\n", numero);
			exit(EXIT_FAILURE);
		}
	}
}

/*Metodo que valida se todos os caracteres sao validos para a base binaria*/
void validaNumeroBinario(char * numero){

	int i;

	for (i = 0; i < strlen(numero); i++){
		if(isDigitoBinario(numero[i]) == -1){
			printf("Erro: o numero %s possui um digito nao valido para a base binaria\n", numero);
			exit(EXIT_FAILURE);
		}
	}
}

/*Metodo que valida se todos os caracteres sao validos para a base octal*/
void validaNumeroOctal(char * numero){

	int i;

	for (i = 0; i < strlen(numero); i++){
		if(isDigitoOctal(numero[i]) == -1){
			printf("Erro: o numero %s possui um digito nao valido para a base octal\n", numero);
			exit(EXIT_FAILURE);
		}
	}
}

/*Metodo que valida se todos os caracteres sao validos para a base decimal*/
void validaNumeroDecimal(char * numero){

	int i;
	
	for (i = 0; i < strlen(numero); i++){
		if(!isdigit(numero[i]) && !(numero[i] == '-' && i == 0)){
			printf("Erro: o numero %s possui um digito nao valido para a base decimal\n", numero);
			exit(EXIT_FAILURE);
		}
	}
}

/*Metodo que recebe um numero na base 10, e retorna na base 16*/
char * decimalToHexadecimal(char * numeroDecimal){

	char * hexadecimal;
	unsigned long long int n;

	hexadecimal = alocaVetorChar(11);
	
	if(numeroDecimal[0] != '-'){
		n = strtoull(numeroDecimal, NULL, 10);
		sprintf(hexadecimal, "%llX", n);
	}
	else{
		n = strtoull(&numeroDecimal[1], NULL, 10);
		n = retornaMaiorValorHexadecimal40Bits() - n + 1;
		sprintf(hexadecimal, "%llX", n);
	}

	return hexadecimal;
}

/*Metodo que recebe um numero na base 2, e retorna na base 16*/
char * binarioToHexadecimal(char * numeroBinario){

	char * hexadecimal;
	unsigned long long int n;

	hexadecimal = alocaVetorChar(11);
	
	n = strtoull(numeroBinario, NULL, 2);
	sprintf(hexadecimal, "%llX", n);

	return hexadecimal;
}

/*Metodo que recebe um numero na base 8, e retorna na base 16*/
char * octalToHexadecimal(char * numeroOctal){

	char * hexadecimal;
	unsigned long long int n;

	hexadecimal = alocaVetorChar(11);
	
	n = strtoull(numeroOctal, NULL, 8);
	sprintf(hexadecimal, "%llX", n);

	return hexadecimal;
}

int isHexadecimal(char * numero){

	return strlen(numero) > 2 && numero[0] == '0' && numero[1] == 'X';
}

int isOctal(char * numero){

	return strlen(numero) > 2 && numero[0] == '0' && numero[1] == 'O';
}

int isBinario(char * numero){

	return strlen(numero) > 2 && numero[0] == '0' && numero[1] == 'B';
}

int isDecimal(char * numero){

	return !isBinario(numero) && !isHexadecimal(numero) && !isOctal(numero);
}

int toDecimal(char * numero){

	int i, tamanho;
	char * hexadecimal;
	long long int decimal;

	if(isHexadecimal(numero)){
		numero += 2; //ignora os "0x" inicial
		validaNumeroHexadecimal(numero);
		hexadecimal = alocaVetorChar(strlen(numero) + 1);
		strcpy(hexadecimal, numero);
	}
	else if(isOctal(numero)){
		numero += 2; //ignora os "0o" inicial
		validaNumeroOctal(numero);
		hexadecimal = octalToHexadecimal(numero);
	}
	else if(isBinario(numero)){
		numero += 2; //ignora os "0b" inicial
		validaNumeroBinario(numero);
		hexadecimal = binarioToHexadecimal(numero);
	}
	else{
		printf("Erro: formato invalido no endereco de uma instrucao: %s\n", numero);
		exit(EXIT_FAILURE);
	}

	tamanho = strlen(hexadecimal);
	decimal = 0;
	for (i = 0; hexadecimal[i] != '\0' ; i++){
		tamanho--;
		decimal += hexaToInt(hexadecimal[i]) * pow(16, tamanho);
	}
	free(hexadecimal);

	if(decimal > 1023){
		printf("Erro: endereco de instrucao maior que 1023: %s\n", numero);
		exit(EXIT_FAILURE);
	}

	return (int) decimal;
}

/*Metodo que converte o numero para hexadecimal*/
char * toHexadecimal(char * numero){

	char * hexadecimal;

	if(isHexadecimal(numero)){
		numero += 2; //ignora os "0x" inicial
		validaNumeroHexadecimal(numero);
		hexadecimal = alocaVetorChar(strlen(numero) + 1);
		strcpy(hexadecimal, numero);
	}
	else if(isOctal(numero)){
		numero += 2; //ignora os "0o" inicial
		validaNumeroOctal(numero);
		hexadecimal = octalToHexadecimal(numero);
	}
	else if(isBinario(numero)){
		numero += 2; //ignora os "0b" inicial
		validaNumeroBinario(numero);
		hexadecimal = binarioToHexadecimal(numero);
	}
	else{
		validaNumeroDecimal(numero);
		hexadecimal = decimalToHexadecimal(numero);
	}
	
	return hexadecimal;	
}

int stringHexadecimalToInt(char * numeroString){

	int numero, tamanho;

	numeroString += 2;
	numero = 0;
	tamanho = strlen(numeroString);

	while (numeroString[0] != '\0'){
		tamanho--;
		numero+= hexaToInt(numeroString[0]) * pow(16, tamanho);
		numeroString++;
	}

	return numero;
}

int stringOctalToInt(char * numeroString){

	int numero, tamanho;

	numeroString += 2;
	numero = 0;
	tamanho = strlen(numeroString);

	while (numeroString[0] != '\0'){
		tamanho--;
		numero+= hexaToInt(numeroString[0]) * pow(8, tamanho);
		numeroString++;
	}

	return numero;
}

int stringBinarioToInt(char * numeroString){

	int numero, tamanho;

	numeroString += 2;
	numero = 0;
	tamanho = strlen(numeroString);

	while (numeroString[0] != '\0'){
		tamanho--;
		numero+= hexaToInt(numeroString[0]) * pow(2, tamanho);
		numeroString++;
	}

	return numero;
}

/*Metodo que converte o numero para decimal*/
int toInt(char * numero, long long int min, long long int max){

	long long int n;

	validaNumeroDecimal(numero);
	
	n = strtoll(numero, NULL, 10);

	if(n < min || n > max){
		printf("Erro: existe um numero decimal que esta fora do intervalo permitido.\n");
		exit(EXIT_FAILURE);
	}

	return (int) n;
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

/*Metodo que formata o numero, colocando 0 a esquerda
e separando por espacos para ficar no padrao: DD DDD DD DDD*/
char * retornaHexadecimalFormatoQuarentaBits(char * hexadecimal){

	int i, iEntrada;
	char * numeroFormatado;

	numeroFormatado = alocaVetorChar(14);

	iEntrada = strlen(hexadecimal) - 1;

	for (i = 12; i >= 0; i--){
		
		if(i == 2 || i == 6|| i == 9){
			numeroFormatado[i] = ' ';
		}
		else if(iEntrada < 0){
			numeroFormatado[i] = '0';
		}
		else{
			numeroFormatado[i] = hexadecimal[iEntrada];
			iEntrada--;
		}
	}

	numeroFormatado[13] = '\0';

	return numeroFormatado;
}