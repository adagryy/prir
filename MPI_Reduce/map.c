#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include "map.h"

// #define debug printf // odkomentuj, aby zobaczyc komunikaty diagnostyczne
#define debug // odkomentuj, aby ukryc komunikaty diagnostyczne

extern int world_rank; /* identyfikator w komunikatorze globalnym */
extern int world_size; /* liczba procesow */
extern int master;

/* redukuje mapuje slowa [k1, k2, k1, k3...] z words na [{k1: v1}, {k2: v2}, {k1: v3}, {k3: v4} ...] */
void map(int words_amount, char* words, int* indexes, int* out_nwords, char* out_words, int* out_indexes, int* out_occurs) {
	int  all_words = 0;
	int* count = NULL;
	int* count_bytes = NULL;
	int* skip = NULL;
	int* skip_bytes = NULL;
	char* w = NULL;
	int i, j, len;
	int nbytes;

	indexes++;
	if (world_rank == master) {
		all_words = words_amount;

		count = calloc(world_size, sizeof(int));
		count_bytes = calloc(world_size, sizeof(int));
		skip = calloc(world_size, sizeof(int));
		skip_bytes = calloc(world_size, sizeof(int));

		divide(world_size, words_amount, words, count, skip);
		bytes(world_size, indexes, count, skip, count_bytes, skip_bytes);

		for (i=0; i<world_size; i++)
			debug("M count[%d] = %d, count_bytes[%d] = %d, skip[%d] = %d, skip_bytes[%d] = %d\n", i, count[i], i, count_bytes[i], i, skip[i], i, skip_bytes[i]);
		for (i=0; i<words_amount; i++)
			debug("M indexes[%d] = %d\n", i, indexes[i]);
	}

	/* words_amount */
	MPI_Scatter(count, 1, MPI_INT, &words_amount, 1, MPI_INT, master, MPI_COMM_WORLD);
	debug("[%d] Got %d words to map\n", world_rank, words_amount);

	/* indexes */
	   MPI_Scatter(           count_bytes,  1,         MPI_INT,                     &nbytes,     1,         MPI_INT,                   master, MPI_COMM_WORLD);

// int MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,   MPI_Comm comm)


	if (world_rank != master)
		words = calloc(nbytes, sizeof(char));

	/* words */ 
	MPI_Scatterv(words, count_bytes, skip_bytes, MPI_CHAR, words, nbytes, MPI_CHAR, master, MPI_COMM_WORLD);

	{
		int i, j; int len = 0;
		int* occurs = NULL;

		/* words */
		MPI_Gatherv(words, nbytes, MPI_CHAR, words, count_bytes, skip_bytes, MPI_CHAR, master, MPI_COMM_WORLD);
		occurs = calloc((world_rank == master)? all_words : words_amount, sizeof(int));
		for (i=0; i<words_amount; i++)
			occurs[i] = 1;

		/* occurs */
		MPI_Gatherv(occurs, words_amount, MPI_INT, occurs, count, skip, MPI_INT, master, MPI_COMM_WORLD);

		if (world_rank == master) {
			int p, q;
			char* w = words;
			char* nw = out_words;

			indexes--;
			for (i=0, p=0, q=0; i<all_words; i++, p++) {
				char* a = words + indexes[i];
				if (*a == '\0') {
					p--;
					continue;
				}
				out_indexes[p] = p > 0? out_indexes[p-1] : 0;

				for (j=i; j<all_words; j++) {
					char* b = words+indexes[j];
					if (strcmp(a, b) == 0) {
						out_indexes[p]++;
						out_occurs[q++] = occurs[j];
						if (j==i) {
							strcpy(nw, a);
							nw += strlen(a) + 1;
						} else {
							*b = '\0';
						}
					}
				}
			}
			*out_nwords = p;
		}
	}
}