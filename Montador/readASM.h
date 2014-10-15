#ifndef READASM_H   /* Include guard */
#define READASM_H

void read_ASM_file(char * nomeArquivo, char **codigoAssembly, char *** controleLinhas, int * qtdLinhas);
char * alocaVetorChar(int tamanho);

#endif