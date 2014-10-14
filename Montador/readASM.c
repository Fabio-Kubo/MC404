#include <stdio.h>
#include <stdlib.h>
#include "readASM.h"

char * alocaVetorChar(int tamanho){
	
	char * vetor;
	
	vetor = (char *) malloc(sizeof(char) * tamanho);

	if(vetor == NULL) {
		printf("Erro ao alocar o vetor de char\n");
		exit(EXIT_FAILURE);
	}

	return vetor;
}

char ** alocaVetorPonteiroDeChar(int tamanho){
	
	char ** vetor;
	
	vetor = (char **) malloc(sizeof(char *) * tamanho);
	
	if(vetor == NULL) {
		printf("Erro ao alocar o vetor de char\n");
		exit(EXIT_FAILURE);
	}

	return vetor;
}

FILE * abreArquivoLeitura(char * nomeArquivo){

	FILE * arquivo;

	arquivo = fopen(nomeArquivo, "r");
	
	if(arquivo == NULL) {
		printf("Erro ao abrir o arquivo.");
		exit(EXIT_FAILURE);
	}

	return arquivo;
}

char ** retornaControleLinhas(char * codigoAssembly){

	char ** controleLinhas;
	int tamanho, i, iControlePalavras;

	tamanho = 2048;
	controleLinhas = alocaVetorPonteiroDeChar(tamanho);

	iControlePalavras = 0;
	for (i = 0; codigoAssembly[i] != '\0'; i++) {
		if(codigoAssembly[i] == '\n'){
			controleLinhas[iControlePalavras] = &codigoAssembly[i + 1];
			iControlePalavras++;
		}
	}

	return controleLinhas;
}

char * retornaTextoArquivo(char * nomeArquivo, int * tamanhoCodigo) {

	FILE * arquivoAsm;
	char * codigoAssembly;

	arquivoAsm =  abreArquivoLeitura(nomeArquivo);

	//calcula o tamanho do arquivo
    fseek(arquivoAsm, 0, SEEK_END);
    (* tamanhoCodigo) = ftell(arquivoAsm) + 1;
    rewind(arquivoAsm);

    //le todo o arquivo
	codigoAssembly = alocaVetorChar((* tamanhoCodigo));
    fread(codigoAssembly, 1, (* tamanhoCodigo), arquivoAsm);
    codigoAssembly[(* tamanhoCodigo)] = '\0';

    fclose(arquivoAsm);

    return codigoAssembly;
}

void retiraComentario(char * codigoAssembly){

	int i;

	for (i = 0; codigoAssembly[i] != '\0'; i++) {
		if(codigoAssembly[i] == '#' || codigoAssembly[i] == '\n')
			codigoAssembly[i] = '\0';
	}
}

void read_ASM_file(char * nomeArquivo, char **codigoAssembly, char *** controleLinhas, int * tamanhoCodigo){

	(* codigoAssembly) = retornaTextoArquivo(nomeArquivo, tamanhoCodigo);
	(* controleLinhas) = retornaControleLinhas((* codigoAssembly));
	retiraComentario((* codigoAssembly));
}