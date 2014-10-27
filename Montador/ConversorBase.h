#ifndef CONVERSORBASE_H 
#define CONVERSORBASE_H

int hexaToInt(char c);
char * toHexadecimal(char * numero);
char * retornaHexadecimalFormatoQuarentaBits(char * hexadecimal);
char * retornaHexadecimalComZerosAEsquerda(int numero, int tamanho);
int isHexadecimal(char * numero);
int isOctal(char * numero);
int isBinario(char * numero);
int isDecimal(char * numero);
int stringHexadecimalToInt(char * numero);
int stringOctalToInt(char * numero);
int stringBinarioToInt(char * numero);

int toInt(char * numero, long long int min, long long int max);
int toDecimal(char * numero);

#endif