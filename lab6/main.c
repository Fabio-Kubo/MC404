#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <endian.h>

#define	TAM_STR	12

extern void troca_endianness_imprime(uint32_t *vetor);

	
int main(int argc, char *argv[]) {

	FILE *ifd=NULL;
	uint32_t i=0, sz=0, tmp=0;
	uint32_t *vet;
	char str_read[TAM_STR];
	
	if (argc != 2) {
		printf ("\nUsage: %s input_filename\n",argv[0]);
		return 1;
	}
	
	ifd = fopen(argv[1],"r");
	if (ifd == NULL) {
		printf ("\nError in input file opening...aborting\n");
		return 2;
	}

	memset(str_read,0,TAM_STR);
	
	fgets(str_read,TAM_STR,ifd);
	fflush(ifd);
	
	tmp = (uint32_t)strtoul(str_read,NULL,10);
	sz = be32toh(tmp);
	
	if ( sz < 1) {
		printf("\nProblem with the size value in input file, after endianness conv. - ABORTING\n");
		return 3;
	}
	
	vet = (uint32_t*) calloc( sz+1,sizeof(uint32_t) );
	
	if (vet == NULL) {
		printf("\nProblem with the array allocation of %u bytes - ABORTING\n",sz+1);
		return 4;	
	}
	
	vet[0] = tmp;
	
	for (i=1; i<=sz; ++i) {
		memset(str_read,0,TAM_STR);
		
		fgets(str_read,TAM_STR,ifd);
		fflush(ifd);
		
		tmp = (uint32_t)strtoul(str_read,NULL,10);
			
		vet[i]=tmp;
	}

	troca_endianness_imprime(vet);

	if (ifd != NULL)
		fclose(ifd);
	
	free(vet);
	return 0;
}