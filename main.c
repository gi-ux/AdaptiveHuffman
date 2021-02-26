#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Albero.h"
#include "ParteComune.h"
#include "Compressione.h"
#include "Decompressione.h"

/*
 * funzione che mostra come eseguire correttamente l'algoritmo
 */
void regolamento() {
	printf(
			"FUNZIONAMENTO \nl'inserimento dev'essere: \nCompressione -> Progetto_HuffmanAdattivo.exe -c fileDaComprimere fileOutput\nDecompressione -> Progetto_HuffmanAdattivo.exe -d fileDaDecomprimere fileOutput\n");
}
/*
 * funzione di controllo in caso di file vuto
 * il nostro algoritmo scrive sempre l'ultimo byte
 * questa funzione ci consente di risolvere il problema
 */
int fileVuoto(FILE *file) {
	long dim = ftell(file);
	fseek(file, 0, SEEK_END);

	if (ftell(file) == 0) {
		return 1;
	}

	fseek(file, dim, SEEK_SET);
	return 0;
}

int main(int argc, char *argv[]) {
	/*
	 * inizializzo il tempo di esecuzione e i file
	 */
	clock_t start = clock();
	FILE *fp_in;
	FILE *fp_out;
	unsigned char vuoto = 0;
	/*
	 * assegno i file
	 */
	fp_in = fopen(argv[2], "rb");
	fp_out = fopen(argv[3], "wb");

	/*
	 * controllo dei parametro
	 */
	if (argc != 4) {
		printf("errore nei parametri\n");
		regolamento();
		return 0;
	}
	/*
	 * differenzio compressione e decompressione
	 */
	if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "-C") == 0) {
		//controllo file vuoto
		if (fileVuoto(fp_in) == 1) {
			fwrite(&vuoto, sizeof(unsigned char), 0, fp_out);
			printf("Compresso\n");
			return 0;
		}
		compressione(fp_in, fp_out);
		printf("Compresso\n");
	} else if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "-D") == 0) {
		//controllo file vuoto
		if (fileVuoto(fp_in) == 1) {
			fwrite(&vuoto, sizeof(unsigned char), 0, fp_out);
			printf("Decompresso\n");
			return 0;
		}
		decompressione(fp_in, fp_out);
		printf("Decompresso\n");
	} else {
		printf("errore nei parametri\n");
		regolamento();
	}
	/*
	 * chiudo i file utilizzati
	 */
	fclose(fp_in);
	fclose(fp_out);
	clock_t end = clock();
	printf("Tempo di esecuzione =  %f secondi \n",
			((double) (end - start)) / CLOCKS_PER_SEC);

	return 0;
}
