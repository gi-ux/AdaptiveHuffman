#include <stdio.h>
#include <stdlib.h>

#ifndef DECOMPRESSIONE_H_
#define DECOMPRESSIONE_H_
/*
 * Metodo per la lettura del file codificato un bit alla volta
 */
int leggiBitPerBit(FILE *fp, unsigned char *buffer, int *dimBuffer,
		long int dimFile, int leggiBit);
/*
 * Metodo per la lettura di un intero carattere dal file codificato (8 bit)
 */
char leggiNuovoCarattere(FILE *fp, unsigned char *buffer, int *dimBuffer,
		long int dimFile, int leggiBit);
/*
 * Metodo che contiene la decodifica (lettura bit a bit, lettura carattere, scorrimento e aggiunta nell'albero)
 */
void decompressione(FILE *fp_in, FILE *fp_out);

#endif
