#include <stdio.h>
#include <stdlib.h>
#include "Albero.h"
#include "Compressione.h"
#include "ParteComune.h"

/*
 * utilizziamo un buffer da 8 bit (1 byte) per scrivere un byte alla volta sul file di output (compresso)
 * la dimensione del buffer è gestitina globalmente (passata per parametro)
 * ogni metodo che modifica la dimensione, la modifica anche per gli altri metodi.
 */

/*
 * funzione che scrive la codifica (percorso nell'albero) di un nodo
 */
unsigned char scriviCodifica(int *codice, int dimCodice, FILE *file,
		unsigned char buf, int *dimBuffer) {

	//creo il buffer che contiene il buffer passato come argomento in testata
	unsigned char buffer = buf;
	/*
	 * OPERAZIONE DI CODIFICA
	 * ciclo finchè non scrivo tutto il codice del nodo
	 * ogni iterazione shifto verso sinistra, il buffer viene riempito da sinistra verso destra un bit alla volta
	 */
	for (int i = 0; i < dimCodice; i++) {
		unsigned char bit = ((unsigned char) codice[i]) << (*dimBuffer - 1);
		//uso una operazione logica di or per riempire il buffer un bit alla volta
		buffer = buffer | bit;
		//decremento la dimensione del buffer (spazio libero)
		(*dimBuffer)--;
		//quando il buffer è pieno scrivo sul file il contenuto, reimpostando il buffer
		if (*dimBuffer == 0) {
			fwrite(&buffer, sizeof(unsigned char), 1, file);
			//porto il contenuto a 8 e lo spazio libero a 8
			buffer = 0;
			*dimBuffer = 8;
		}
	}
	//ritorno il buffer
	return buffer;
}

/*
 * funzione che scrive la codifica (ASCII) di un carattere sul file
 * dal momento che andiamo a scrivere generalmente la codifica prima del carattere, non abbiamo la
 * garanzia di avere disponibile un intero byte, quindi devo controllare la dimensione libera del buffer.
 * avviene quindi uno shift per raggiungere la posizione libera e usarlo tutto senza sprecare null, per poi
 * tornare indietro dello shift iniziale
 */
unsigned char scriviCarattere(char byteLetto, FILE *file, unsigned char buf,
		int *dimBuffer) {

	//creo il buffer che contiene il buffer passato come argomento in testata
	unsigned char buffer = buf;
	//creo un byte temporaneo che viene shiftato verso destra dello spazio libero ancora disponibile
	unsigned char tempByte = ((unsigned char) byteLetto) >> (8 - *dimBuffer);
	//riempio il buffer con il contenuto del byte temporaneo e lo scrivo su un file
	buffer = buffer | tempByte;
	fwrite(&buffer, sizeof(unsigned char), 1, file);
	//dopo la scrittura shifto indietro dello spazio precedentemente libero
	buffer = byteLetto << *dimBuffer;
	//se il buffer è complemtamente vuoto ritorno 0, altrimenti il buffer
	if (*dimBuffer == 8)
		return 0;
	else
		return buffer;
}
/*
 * funzione per scrivere il byte terminatore che ci indica in decompressione quando fermarci nella lettura del byte precedente
 */
void scriviByteFinale(FILE *file, unsigned char buffer, int dimBuffer) {
	//se ho il penultimo byte non completamente usato lo riempio
	if (dimBuffer < 8)
		fwrite(&buffer, sizeof(unsigned char), 1, file);
	//se il penultimo byte era pieno notifico in buffer che nella decompressione vanno letti 8 bit (va letto tutto)
	if (dimBuffer == 8)
		buffer = 8;
	//altrimenti notifico quanti bit vanno letti facendo la dimensione del buffer (8) - lo spazio occupato
	else
		buffer = 8 - dimBuffer;
	//scrivo tutto sul file di output, questa è l'ultima operazione in scrittura
	fwrite(&buffer, sizeof(unsigned char), 1, file);
}

/*
 * funzione di compressione:
 * passo il file da comprimere e il file di output
 */
void compressione(FILE *fp_in, FILE *fp_out) {
	/*
	 * OPERAZIONE DI INIZIALIZZAZIONE E PULIZIA
	 */
	//genero l'albero inizialmente vuoto (coontiene solo la radice, nodo NYT in questo caso)
	Nodo *root = creaAlbero();
	Nodo *puntatoreNyt = root;
	//creo il buffer con dimensione 8 e inizialmente vuoto
	unsigned char buffer = 0;
	int dimBuffer = 8;
	//genero lo spazio per l'insieme di caratteri tutto a 0 (non ho ancora letto nessun carattere)
	Carattere **caratteri = calloc(256, sizeof(Carattere*));
	//OPERAZIONI DI LETTURA
	//letto un byte (un carattere) alla volta
	unsigned char byteLetto;
	while (fread(&byteLetto, sizeof(unsigned char), 1, fp_in) > 0) {
		//controllo se il nodo letto è già presente nell'albero
		Nodo *nodoCarattLetto = nodoDaCarattere(byteLetto, caratteri);
		//se il nodo è presente
		if (nodoCarattLetto) {
			//ottengo la codifica del nodo nell'albero (la sua posizione)
			int dimCodice;
			int *codiceCarattLetto = codiceNodo(nodoCarattLetto, &dimCodice);
			//scrivo la cofifica
			buffer = scriviCodifica(codiceCarattLetto, dimCodice, fp_out,
					buffer, &dimBuffer);
			//aggiorno l'albero (possibili swap)
			aggiorna(nodoCarattLetto, root);
			//dealloco lo spazio utilizzato per la codifica
			free(codiceCarattLetto);
			//non esiste il nodo nell'albero
		} else {
			//ottengo la codifica del nodo NYT
			int dimCodice;
			int *codiceNyt = codiceNodo(puntatoreNyt, &dimCodice);
			//scrivo la codifica del NYT
			buffer = scriviCodifica(codiceNyt, dimCodice, fp_out, buffer,
					&dimBuffer);
			//scrivo l'intero carattere
			buffer = scriviCarattere(byteLetto, fp_out, buffer, &dimBuffer);
			//aggiungo il carattere all'albero quindi alla lista dei caratteri
			Nodo *nuovoCarattere = aggiungiCarattere(byteLetto, &puntatoreNyt,
					caratteri);
			//aggiorno l'albero (possibili swap)
			aggiorna(nuovoCarattere, root);
			//dealloco lo spazio utilizzato per la codifica
			free(codiceNyt);
		}
	}
	//ultima operazione di scrittura, indico fino a dove leggere nel penultimo byte
	scriviByteFinale(fp_out, buffer, dimBuffer);
}
