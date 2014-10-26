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
int isHexadecimal(char digito){

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
int isOctal(char digito){

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
int isBinario(char digito){

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

int toInt(char * numeroString) {

	int tamanho, numero;
	
	numero = 0;
	if(strlen(numeroString) > 2 && numeroString[0] == '0' && numeroString[1] == 'X'){

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

/*Metodo que valida se todos os caracteres sao validos para a base hexadecimal*/
void validaNumeroHexadecimal(char * numero){

	int i;

	for (i = 0; i < strlen(numero); i++){
		if(isHexadecimal(numero[i]) == -1){
			printf("Erro: o numero %s possui um digito nao valido para a base hexadecimal\n", numero);
			exit(EXIT_FAILURE);
		}
	}
}

/*Metodo que valida se todos os caracteres sao validos para a base binaria*/
void validaNumeroBinario(char * numero){

	int i;

	for (i = 0; i < strlen(numero); i++){
		if(isBinario(numero[i]) == -1){
			printf("Erro: o numero %s possui um digito nao valido para a base binaria\n", numero);
			exit(EXIT_FAILURE);
		}
	}
}

/*Metodo que valida se todos os caracteres sao validos para a base octal*/
void validaNumeroOctal(char * numero){

	int i;

	for (i = 0; i < strlen(numero); i++){
		if(isOctal(numero[i]) == -1){
			printf("Erro: o numero %s possui um digito nao valido para a base octal\n", numero);
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
	
	n = strtoull(numeroOctal, NULL, 2);
	sprintf(hexadecimal, "%llX", n);

	return hexadecimal;
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

/*Metodo que converte o numero para hexadecimal*/
char * toHexadecimal(char * numero){

	char * hexadecimal;

	hexadecimal = alocaVetorChar(14);
	if(strlen(numero) > 2 && numero[0] == '0' && numero[1] == 'X'){
		numero += 2; //ignora os "0x" inicial
		hexadecimal = alocaVetorChar(sizeof(numero) + 1);
		strcpy(hexadecimal, numero);
	}
	else if (strlen(numero) > 2 && numero[0] == '0' && numero[1] == 'O'){
		hexadecimal = octalToHexadecimal(numero);
	}
	else if (strlen(numero) > 2 && numero[0] == '0' && numero[1] == 'B'){
		hexadecimal = binarioToHexadecimal(numero);
	}
	else{
		hexadecimal = decimalToHexadecimal(numero);
	}
	
	return hexadecimal;	
}

