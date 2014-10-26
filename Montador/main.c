#include <stdio.h>
#include <stdlib.h>
#include "readASM.h"
#include "Montador.h"

int main(int argc, char *argv[]){

	char * codigoAssembly,* nomeArqSaida;
	char ** controleLinhas;
	int qtdLinhas;

	qtdLinhas = 0;
	codigoAssembly = NULL;
	controleLinhas = NULL;
	listaRotulos = NULL;
	listaPalavras = NULL;

    if (argc <= 1) {
        printf("Erro: informe um arquivo de entrada.\n");
        exit(EXIT_FAILURE);
    }
    read_ASM_file(argv[1], &codigoAssembly, &controleLinhas, &qtdLinhas);
    
    //Realiza a primeira parte do processamento
    atualizaPosicaoMemoriaAtual(0, 1);
    mapeaRotulosEDiretivaSet(controleLinhas, qtdLinhas);

    //re-inicializa a posicao de memoria
    atualizaPosicaoMemoriaAtual(0, 1);
    processamentoCodigoPrincipal(controleLinhas, qtdLinhas);
    
    if(argc == 2){
        nomeArqSaida = retornaNomeSaida(argv[1]);
    	imprimeArquivo(nomeArqSaida);
        free(nomeArqSaida);
    }
    else{
    	imprimeArquivo(argv[2]);
    }

    liberaMemoriaDasEstruturas();
	free(codigoAssembly);
    free(controleLinhas);
    return 0;
}