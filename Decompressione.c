#include <stdio.h>
#include <stdlib.h>
#include "Albero.h"
#include "Decompressione.h"
#include "ParteComune.h"

/*
 * Lettura bit per bit:
 * vengono passati il file codificato e il buffer con le rispettive dimensioni
 * il parametro leggiBit ci informa quanti bit dovremo leggere prima di fermarci
 */
int leggiBitPerBit(FILE *fp, unsigned char *buffer, int *dimBuffer,
		long int dimFile, int leggiBit) {
	/*
	 * viene controllata la dimensione del buffer: qualora risulti 0 viene reimpostata a 8 e viene letto l'intero buffer
	 */
	if (*dimBuffer == 0) {
		*dimBuffer = 8;
		fread(buffer, sizeof(unsigned char), 1, fp);
	}

	/*
	 * avviene un controllo sulla lettura dei bit
	 * se il parametro leggiBit è 8 allora il file verrà letto interamente, senza interruzioni
	 */
	if (leggiBit != 8) {
		/*
		 * ftell(fp) == dimFile ci informa se siamo alla fine del file
		 * leggiBit <= (8 - *dimBuffer) ci informa se abbiamo letto correttamente tutti i bit
		 * in questo caso ho finito di leggere e ritorno -1
		 */
		if (ftell(fp) == dimFile && leggiBit == (8 - *dimBuffer)) {
			return -1;
		}
	}
	/*
	 * Andiamo quindi a controllare se ci sono errori:
	 * ftell(fp) > dimFile ci informa se siamo oltre alla dimensione del file
	 * feof(fp)) ci informa se abbiamo raggiunto la fine del file
	 */
	if (ftell(fp) > dimFile || feof(fp)) {
		return -1;
	}

	//decrementiamo quindi la dimensione del buffer per eseguire correttamente la lettura con lo shift
	(*dimBuffer)--;

	/*
	 * leggo un bit alla volta:
	 * metto il contenuto dello shift all'interno di lettura
	 * eseguo una and tra lettura e a, in modo da ottenere un 1 o 0
	 */
	int lettura = (*buffer >> *dimBuffer);
	lettura &= 1;
	//il valore di ritorno è 1 o 0
	return lettura;
}

/*
 * leggo un nuovo carattere (quindi 8 bit)
 * questo metodo viene utilizzato quando in decodifica leggiamo il percorso del nodo NYT
 * il decompressore quindi si accorge che i prossimi 8 bit saranno un nuovo carattere
 * vengono passati gli stessi parametri della funzione leggiBitPerBit dal momento che viene sfruttata nel metodo
 */
char leggiNuovoCarattere(FILE *fp, unsigned char *buffer, int *dimBuffer,
		long int dimFile, int leggiBit) {

	/*
	 * creo le variabili bit e carattereLetto
	 * la prima conterrà i bit letti, la seconda l'insieme degli 8 bit letti (quindi il char)
	 */
	int bit;
	char carattereLetto = 0;
	/*
	 * eseguo la lettura di un bit alla volta per 8 volte
	 */
	for (int i = 0; i < 8; i++) {
		//bit contiene il bit letto
		bit = leggiBitPerBit(fp, buffer, dimBuffer, dimFile, leggiBit);
		/*
		 * shift bit a sinistra di 7-i (indice iterazione) per riempire l'intero da sinistra verso destra
		 * alla prima iterazione quindi shifta di 7 posizioni a sinistra (mette il primo bit)
		 */
		bit = bit << (7 - i);
		//inserisco il bit all'interno della variabile carattereLetto sfruttando l'operatore logico di or
		carattereLetto |= bit;
	}
	//ritorno il carattere letto
	return carattereLetto;
}

/*
 * funzione di decompressione:
 * passo il file da decomprimere e il file di output
 */
void decompressione(FILE *fp_in, FILE *fp_out) {
	/*
	 * OPERAZIONI DI INIZIALIZZAZIONE E PULIZIA:
	 * creo l'albero in modo identico alla compressione (contenente quindi il solo nodo NYT e un puntatore al nodo NYT
	 */
	Nodo *root = creaAlbero();
	Nodo *puntatoreNyt = root;
	/*
	 * prendo la dimensione del file escludendo l'ultima posizione
	 * come sappiamo l'ultimo byte è stato utilizzato per porre una condizione di arresto nella lettura
	 */
	fseek(fp_in, -1, SEEK_END);
	int dimFile = ftell(fp_in);
	/* inizializzo il buffer a 0 e la sua dimensione a 0 */
	unsigned char buffer = 0;
	int dimBuffer = 0;
	/* pulisco l'array di caratteri mettendo tutto a 0 con la calloc */
	Carattere **caratteri = calloc(256, sizeof(Carattere*));

	/* OPERAZIONI DI LETTURA:
	 * leggo l'ultimo byte che mi informa quanti bit devono essere letti nel penultimo byte
	 */
	unsigned char leggiBit;
	fread(&leggiBit, sizeof(unsigned char), 1, fp_in);
	rewind(fp_in);
	/*
	 * quindi torno indietro e avvio la lettura vera e propria
	 * scorrendo il file fino alla fine
	 * creo di volta in volta un nodo che faccio partire dalla radice
	 * la variabile eof (end of file) serve come controllo
	 */
	while (!feof(fp_in)) {

		Nodo *nodoLetto = root;
		int eof = 0;
		/*
		 * comincio a leggere bit per bit finchè il mio nodo non è un nodo foglia
		 * in funzione al bit ricevuto scorro il nodo verso il figlio di destra (bit = 1) o sinistra (bit = 0)
		 * quando raggiungiamo una foglia andremo quindi ad analizzare i vari casi
		 */
		while (!nodoLetto->foglia && !eof) {
			int bit = leggiBitPerBit(fp_in, &buffer, &dimBuffer, dimFile,
					leggiBit);
			if (bit == 0)
				nodoLetto = nodoLetto->sx;
			else if (bit == 1)
				nodoLetto = nodoLetto->dx;
			else
				eof = 1;
		}
		if (eof)
			break;
		/*
		 * siamo all'interno di un nodo foglia, ora è necessario controllare che tipo di nodo è:
		 * se il nodo letto è il nodo NYT allora il prossimo byte e un nuovo carattere, leggo quindi gli 8 bit con il metodo precedentmente creato,
		 * lo aggiungo quindi all'insieme dei caratteri e lo scrivo sul file
		 * se il nodo non è NYT scrivo semplicemente il suo contenuto sul file
		 * in ogni caso ci sarà un'operazione di aggiornamento dell'albero, aggiornando i pesi e bilanciando secondo le adeguate proprietà
		 */
		unsigned char c;
		if (nodoLetto->nyt) {
			c = leggiNuovoCarattere(fp_in, &buffer, &dimBuffer, dimFile,
					leggiBit);
			nodoLetto = aggiungiCarattere(c, &puntatoreNyt, caratteri);
		} else
			c = nodoLetto->carattere;
		fwrite(&c, sizeof(unsigned char), 1, fp_out);
		aggiorna(nodoLetto, root);
	}
}
