#include <stdio.h>
#include <stdlib.h>

#ifndef PARTECOMUNE_H_
#define PARTECOMUNE_H_

Nodo* pulisciNodo(Nodo *nodo);

Nodo* creaAlbero();

Nodo* nodoDaCarattere(unsigned char carattere, Carattere **caratteri);

void invertiCodice(int *codice, int dimCodice);

int* codiceNodo(Nodo *nodo, int *dimensione);

Nodo* aggiungiCarattere(unsigned char carattere, Nodo **puntatoreNyt,
		Carattere **caratteri);

Nodo* trovaNodo(Nodo *daTrovare, Nodo *root);

void swap(Nodo *n1, Nodo *n2);

void aggiorna(Nodo *nuovoNodo, Nodo *root);

#endif
