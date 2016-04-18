#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include "reduce.h"

// #define debug printf // odkomentuj, aby zobaczyc komunikaty diagnostyczne
#define debug // odkomentuj, aby ukryc komunikaty diagnostyczne

extern int world_rank; /* identyfikator w komunikatorze globalnym */
extern int world_size; /* liczba procesow */
extern int master;

void reduce(int words_amount, char* words, int* indexes, int* list) {
	int nbytes;
	int i, j;
	int* count = NULL;
	int* skip = NULL;
	int* count_bytes = NULL;
	int* skip_bytes = NULL;

	if (world_rank == master) {
		count = calloc(world_size, sizeof(int));
		skip = calloc(world_size, sizeof(int));
		count_bytes = calloc(world_size, sizeof(int));
		skip_bytes = calloc(world_size, sizeof(int));

		divide(world_size, words_amount, words, count, skip);
		bytes(world_size, indexes, count, skip, count_bytes, skip_bytes);

		for (i=words_amount-1, j=world_size-1; i>=0; i--) {
			if (i<skip[j])
				j--;
			indexes[i] -= indexes[skip[j]-1];
		}
		for (i=0; i<world_size; i++)
			debug("R count[%d] = %d, count_bytes[%d] = %d, skip[%d] = %d, skip_bytes[%d] = %d\n", i, count[i], i, count_bytes[i], i, skip[i], i, skip_bytes[i]);
		for (i=0; i<words_amount; i++)
			debug("R indexes[%d] = %d\n", i, indexes[i]);
	}

	/* rozsylanie ilosci slow przydzielonych procesom */
	/* zapisane w tablicy count, w procesie zapisywane do zmiennej words_amount */
	MPI_Scatter(count, 1, MPI_INT, &words_amount, 1, MPI_INT, master, MPI_COMM_WORLD);
	debug("[%d] Got %d words to reduce\n", world_rank, words_amount);
	if (world_rank != master) /* jesli proces nie jest master, to trzeba mu przygotowac indexes */
		indexes = calloc(words_amount, sizeof(int));

	MPI_Scatter(count_bytes, 1, MPI_INT, &nbytes, 1, MPI_INT, master, MPI_COMM_WORLD);
	if (world_rank != master)
		list = calloc(nbytes, sizeof(int));
	debug("[%d] Got %d bytes of list to reduce\n", world_rank, nbytes);

	MPI_Scatterv(list, count_bytes, skip_bytes, MPI_INT, list, nbytes, MPI_INT, master, MPI_COMM_WORLD);
	MPI_Scatterv(indexes, count, skip, MPI_INT, indexes, words_amount, MPI_INT, master, MPI_COMM_WORLD);
	for (i=0; i<words_amount; i++)
		debug("[%d] Index %d\n", world_rank, indexes[i]);

	for (i=0, j=0; i<words_amount; i++) {
		int sum = 0;
		for (; j<indexes[i]; j++)
			sum += list[j];
		indexes[i] = sum;
		debug("[%d] SUM %d = %d\n", world_rank, i, indexes[i]);
	}
	MPI_Gatherv(indexes, words_amount, MPI_INT, indexes, count, skip, MPI_INT, master, MPI_COMM_WORLD);
}
