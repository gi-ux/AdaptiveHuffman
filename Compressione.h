#include <stdio.h>
#include <stdlib.h>

#ifndef COMPRESSIONE_H_
#define COMPRESSIONE_H_

unsigned char scriviCodifica(int *codice, int dimCodice, FILE *file,
		unsigned char buf, int *dimBuffer);
unsigned char scriviCarattere(char byteLetto, FILE *file, unsigned char buf,
		int *dimBuffer);

void scriviByteFinale(FILE *file, unsigned char buffer, int dimBuffer);

void compressione(FILE *fp_in, FILE *fp_out);

#endif
