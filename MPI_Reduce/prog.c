#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include "map.h"
#include "divide.h"
#include "bytes.h"
#include "reduce.h"

// #define debug printf // odkomentuj, aby zobaczyc komunikaty diagnostyczne
#define debug // odkomentuj, aby ukryc komunikaty diagnostyczne
#define die(msg, err) do { perror(msg); return err; } while(0)

int world_rank; /* identyfikator w komunikatorze globalnym */
int world_size; /* liczba procesow */
int master = 0;

int main(int argc, char** argv) {
	int i, j;
	int words_amount = 0;
	int out_nwords = 0;
	char* words = NULL;
	char* out_words = NULL;
	int* indexes = NULL;
	int* out_indexes = NULL;
	int* out_occurs = NULL;
	int buffer_size = 1024;
	int marker = 0;
	int sp = 0;

	
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	if (world_rank == master) {
		if(strcmp(argv[1], "--addr") != 0 && strcmp(argv[1], "--stat") != 0 && strcmp(argv[1], "--time") != 0){
			char usage [] = "Usage:\n";
			char usage2 [] = "--(addr|stat|time) path/to/logfile";
			printf("%s %s %s\n", usage, argv[0], usage2);
			return 0;
		}else{
			int bytes;
			char c;
			int len; char* w;
			FILE* f = fopen(argv[2],"r");
			words = calloc(buffer_size, sizeof(char)); /* "The last time I was"... */
			indexes = calloc(buffer_size, sizeof(int)); /* "3 8 13 15... */

			if (!f) die("Nie udalo sie otworzyc pliku", 1);

			/* pozbieraj slowa */
			for (bytes = 0; (c = getc(f)) != EOF; bytes++) {
				if(c == 32){
					sp++;
					bytes--;
					continue;
				}
				if(c == 10){
					sp = 0;
					bytes--;
					continue;
				}

				if (strcmp(argv[1], "--time") == 0 && sp != 3) {
					if (!words[bytes-1]) bytes--;
					continue;
				}

				if (strcmp(argv[1], "--addr") == 0 && sp != 0) {
					if (!words[bytes-1]) bytes--;
					continue;
				}

				if (strcmp(argv[1], "--stat") == 0 && sp != 5) {
					if (!words[bytes-1]) bytes--;
					continue;
				}


				if (bytes > buffer_size) {
					buffer_size *= 2;
					printf("Powiekszam bufor do %d bajtow.\n", buffer_size);
					words = realloc(words, buffer_size * sizeof(char));
					indexes = realloc(indexes, buffer_size * sizeof(int));
				}
				words[bytes] = c;
			}

			/* policz slowa */
			w = words;
			len = strlen(w);
			while(len) {
				len = strlen(w);
				if (len == 0) break;
				indexes[++words_amount] = w + len - words + 1;
				debug("%s on position %d\n", w, indexes[words_amount]);
				w += len + 1;
			}

			debug("Wczytano %d bajtow (%d slow)\n", bytes, words_amount);
			out_words = calloc(bytes,sizeof(char));
			out_indexes = calloc(words_amount, sizeof(int));
			out_occurs = calloc(words_amount, sizeof(int));
		}
	}

	if (world_rank == master) printf("Faza mapowania %d slow na %d procesorach\n", words_amount, world_size);
	map(words_amount, words, indexes, &out_nwords, out_words, out_indexes, out_occurs);
	
	/* wyswietl wynik mapowania */
	if (world_rank == master) {
		char* w = out_words;
		for (i=0; i<out_nwords; i++, w += strlen(w) + 1) {
			int a = i>0? out_indexes[i-1] : 0;
			int b = out_indexes[i];
			printf("Mapped %s => [", w);
			for (j=a; j<b; j++)
				printf("%d ", out_occurs[j]);
			printf("]\n");
		}
	}

	if (world_rank == master) printf("Faza redukcji %d kluczy na %d procesorach\n", out_nwords, world_size);

	reduce(out_nwords, out_words, out_indexes, out_occurs);
	
	if (world_rank == master) {
		char* w = out_words;
		for (i=0; i<out_nwords; i++, w += strlen(w) + 1) {
			printf("Reduced %s => %d\n", w, out_indexes[i]);
		}
	}

	if (world_rank == master) {
		free(words);
		free(indexes);
		free(out_words);
		free(out_indexes);
		free(out_occurs);
	}
	MPI_Finalize();
	return 0;
}
