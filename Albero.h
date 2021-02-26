#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef Albero
#define Albero

/* nodo di un albero */
struct nodo {
	int nyt;
	int root;
	int foglia;

	struct nodo *parent;
	struct nodo *sx;
	struct nodo *dx;

	unsigned char carattere;
	int peso;
	int valore;
};
typedef struct nodo Nodo;

/* contiene simbolo + puntatore al nodo dell'albero in cui è presente */
struct carattere {
	char carattere;
	Nodo *nodo;
};
typedef struct carattere Carattere;

#endif
