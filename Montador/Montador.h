#ifndef MONTADOR_H 
#define MONTADOR_H

typedef struct Endereco{
	int linha;
	int isEsquerda;	
} Endereco;

typedef struct Rotulo{
	char * nome;
	Endereco endereco;
	struct Rotulo * prox;
} Rotulo;

typedef struct Simbolo{
	char * nome;
	char * valor;
	struct Simbolo * prox;
} Simbolo;

typedef struct Palavra{
	int linha;
	int instrucaoEsq;
	int parametroEsq;
	int instrucaoDir;
	int parametroDir;
	int isQuarentaBits;
	char * valorQuarentaBits;
	struct Palavra * prox;
} Palavra;

Endereco posicaoMemoriaAtual;
Palavra * listaPalavras;
Rotulo * listaRotulos;
Simbolo * listaSimbolos;

void imprimeArquivo(char * nomeArqSaida);
void mapeaRotulosEDiretivaSet(char ** controleLinhas, int qtdLinhas);
void processamentoCodigoPrincipal(char ** controleLinhas, int qtdLinhas);
void liberaMemoriaDasEstruturas();
void atualizaPosicaoMemoriaAtual();
char * retornaNomeSaida(char * nomeEntrada);

#endif