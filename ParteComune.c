#include <stdio.h>
#include <stdlib.h>
#include "Albero.h"
#include "ParteComune.h"

/* funzione per pulire i riferimenti di un nodo*/
Nodo* pulisciNodo(Nodo *nodo) {
	nodo->parent = NULL;
	nodo->sx = NULL;
	nodo->dx = NULL;
	return nodo;
}

/*
 * funzione di creazione dell'albero:
 * l'albero inizialmente creato contiene solamente il nodo NYT
 * viene infatti inizializzato il nodo NYT con le relative proprietà
 * la funzione ritorna il nodo creato
 */
Nodo* creaAlbero() {
	Nodo *nodo = malloc(sizeof(Nodo));
	nodo->nyt = 1;
	nodo->root = 1;
	nodo->foglia = 1; //essendo solo questo nodo presente è sia foglia che root che nyt
	//pulisco i riferimenti a padre e figli
	nodo = pulisciNodo(nodo);
	nodo->carattere = -1; //il nodo NYT contiene un carattere che non può mai essere trovato in codifica
	nodo->peso = 0; //peso 0
	nodo->valore = 256 * 2; //il valore al massimo può essere 512
	return nodo;
}

/*
 * funzione che ci serve per ottenere un puntatore a una foglia
 * se il nostro array contiene quel carattere
 * ritorno un puntatore al nodo
 * altrimenti ritorno NULL
 */
Nodo* nodoDaCarattere(unsigned char carattere, Carattere **caratteri) {

	Carattere *puntatoreCarattere = caratteri[(unsigned int) carattere];
	if (!puntatoreCarattere)
		return NULL;
	return puntatoreCarattere->nodo;
}
/*
 * Dato che il metodo codiceNodo ci restituisce il path che dal nodo va alla root
 * questo metodo "gira" l'array in modo tale da ottenere il percorso che dalla root va al nodo (codifica corretta)
 * viene passata la dimensione del codice e il codice stesso
 * il metodo effettua una modifica sul codice, senza ritorno
 */
void invertiCodice(int *codice, int dimCodice) {

	int *inizio = codice;
	int *fine = codice + (dimCodice - 1);

	/* operazione di inversione del codice */
	while (inizio < fine) {
		int temp = *inizio;
		*inizio = *fine;
		*fine = temp;
		inizio++;
		fine--;
	}
}

/*
 * funzione che serve per otterenere il path che dal nodo arriva alla root
 * vengono passati il nodo di cui vogliamo ottenere il percorso e la dimensione del codice
 */
int* codiceNodo(Nodo *nodo, int *dimensione) {
	Nodo *nodoDaCercare = nodo;
	//instanzio il codice con profondità massima, per non avere problemi anche nel caso peggiore
	int *codice = malloc(sizeof(int) * 256 * 2);

	int i = 0;
	while (!nodoDaCercare->root) {  //dal nodo andiamo alla root
		Nodo *parent = nodoDaCercare->parent;
		if (parent->sx == nodoDaCercare) { //se il nodo analizzato è il nodo di sinsitra allora metto 0
			codice[i] = 0;
		} else {
			codice[i] = 1; //altrimenti metto 1
		}
		nodoDaCercare = nodoDaCercare->parent;
		i++;  //incremento della dimensione del codice
	}
	//una volta ottenuto il codice è opportuno invertirlo
	invertiCodice(codice, i);
	*dimensione = i;
	return codice;
}

/*
 * questo metodo serve ad inserire un carattere all'interno dell'albero
 * viene sfruttata la funzione inizializzaNodo
 * il metodo genera due nodi: nuovoSx e nuovoDx, i quali saranno generati dalla divisione del nodo NYTù
 * il nodo nuovoSx è il nuovo nodo NYT, mentre il nodo nuovoDx conterrà il carattere inserito
 */
Nodo* aggiungiCarattere(unsigned char carattere, Nodo **puntatoreNyt,
		Carattere **caratteri) {

	/*
	 * INIZIALIZZAZIONE E CREAZIONE DEI NODI FIGLI
	 * il nodo nuovoSx è il nuovo nodo NYT
	 * il nodo nuovoDx è il nodo che contiene il nuovo carattere
	 */
	Nodo *nuovoSx = malloc(sizeof(Nodo));
	nuovoSx = pulisciNodo(nuovoSx);
	nuovoSx->nyt = 1;
	nuovoSx->root = 0;
	nuovoSx->foglia = 1;
	nuovoSx->parent = *puntatoreNyt;
	nuovoSx->carattere = -1;
	nuovoSx->peso = 0;
	nuovoSx->valore = (*puntatoreNyt)->valore - 2;

	Nodo *nuovoDx = malloc(sizeof(Nodo));
	nuovoDx = pulisciNodo(nuovoDx);
	nuovoDx->nyt = 0;
	nuovoDx->root = 0;
	nuovoDx->foglia = 1;
	nuovoDx->parent = *puntatoreNyt;
	nuovoDx->carattere = carattere;
	nuovoDx->peso = 1;
	nuovoDx->valore = (*puntatoreNyt)->valore - 1;

	//effettuo le modifiche ai puntatori, i nodi avranno differenti figli e caratteristiche
	Nodo *vecchioNyt = *puntatoreNyt;
	(*puntatoreNyt)->nyt = 0;
	(*puntatoreNyt)->foglia = 0;
	(*puntatoreNyt)->sx = nuovoSx;
	(*puntatoreNyt)->dx = nuovoDx;
	//aggiungo il nuovo carattere all'array di caratteri e gli faccio puntare il nodo che lo contiene
	unsigned int posizioneCarattere = (unsigned int) carattere;
	caratteri[posizioneCarattere] = malloc(sizeof(Carattere));
	caratteri[posizioneCarattere]->carattere = carattere;
	caratteri[posizioneCarattere]->nodo = nuovoDx;
	*puntatoreNyt = nuovoSx;
	return vecchioNyt;
}
/*
 * in questo metodo viene sfruttato il campo peso
 * ogni inserimento andiamo a cercare il nodo all'interno dell'albero con lo stesso peso del nodo in input
 * andiamo a controllare se esiste un nodo con lo stesso peso, altrimenti ritorno NULL
 */
Nodo* trovaNodo(Nodo *daTrovare, Nodo *root) {

	//il nodo trovato corrisponde al nodo con lo stesso peso nell'albero da trovare
	Nodo *nodoTrovato = daTrovare;
	/*
	 * controllo se il peso del nodo da trovare ha peso minore della root
	 * generalmente la root è il nodo con peso massimmo, quindi a meno che non sia la root la condizione è vera
	 * eseguiamo questa operazione su tutti nodi non foglia
	 */
	if (root->peso > nodoTrovato->peso && !root->foglia) {
		//scorriamo la root verso sinistra ricorisvamente
		Nodo *maggioreSx = trovaNodo(nodoTrovato, root->sx);
		if (maggioreSx)
			nodoTrovato = maggioreSx;
		//analogamente scorriamo la root verso destra ricorsivamente
		Nodo *maggioreDx = trovaNodo(nodoTrovato, root->dx);
		if (maggioreDx)
			nodoTrovato = maggioreDx;
		/*
		 * andiamo avanti a scorrere l'albero finchè il peso del nodo radice (che cambia ricorsivamente)
		 * è maggiore del nodo da trovare.
		 * una volta terminata la ricorsione quindi abbiamo una situazione in cui il nodo da trovare è:
		 * o il maggioreDx o il minoreDx oppure non esiste
		 */

		//effettuo un controllo sul nodo usato per scorrere l'albero (inizalmente root, per poi cambiare con la ricorsione)
	} else if (root->peso == nodoTrovato->peso
			&& root->valore > nodoTrovato->valore)
		nodoTrovato = root;
	//controllo se il nodo iniziale è il nodo che abbiamo trovato (quindi non abbiamo ottenuto nulla)
	//quindi ritorno il nodo da trovare in caso di condizione falsa (vera in questo caso dal momento che controlliamo che siano diversi)
	//o null se non vi è stata corrispondenza
	if (nodoTrovato != daTrovare)
		return nodoTrovato;
	else
		return NULL;
}
/*
 * questo metodo effettua le operazioni di swap
 * viene effettuato solo lo scambio dei riferimenti, quindi non si modifica il contenuto dei nodi
 */
void swap(Nodo *n1, Nodo *n2) {
	int temp = n1->valore;
	n1->valore = n2->valore;
	n2->valore = temp;
	if (n1->parent->sx == n1)
		n1->parent->sx = n2;
	else
		n1->parent->dx = n2;
	if (n2->parent->sx == n2)
		n2->parent->sx = n1;
	else
		n2->parent->dx = n1;
	Nodo *temp2 = n1->parent;
	n1->parent = n2->parent;
	n2->parent = temp2;
}
/*
 * funzione per l'aggiornamento dell'albero:
 * qui vengono effettuate le operazioni di swap e l'aggiornamento dei pesi
 */
void aggiorna(Nodo *nuovoNodo, Nodo *root) {

	while (!nuovoNodo->root) {
		Nodo *nodoPresente = trovaNodo(nuovoNodo, root);
		if (nodoPresente && nuovoNodo->parent != nodoPresente)
			swap(nuovoNodo, nodoPresente);
		(nuovoNodo->peso)++;
		nuovoNodo = nuovoNodo->parent;
	}

	(nuovoNodo->peso)++;
}
