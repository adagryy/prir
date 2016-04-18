#include <stdio.h>
#include "divide.h"

// #define debug printf // odkomentuj, aby zobaczyc komunikaty diagnostyczne
#define debug // odkomentuj, aby ukryc komunikaty diagnostyczne

/* generowanie tablic count i skip dla MPI_Scatter/Gather na podstawie ilosci */
/* slow words_amount i liczby procesow size */
void divide(int size, int words_amount, char* words, int* out_count, int* out_skip) {
	int i, j;
	int part = words_amount / size; /* calkowita czesc podzialu */
	int reminder = words_amount % size; /* reszta podzialu */

	debug("part %d, reminder %d\n", part, reminder);
	for (i=0; i<size; i++) {
		out_count[i] = part;

		if (reminder-- > 0) /* jesli zostala reszta to dodaj ja do tego procesu */
			out_count[i]++;

		out_skip[i] = 0;
		/* dla count = [0, 2, 4, 5] => skip = [0, 0+2, 0+2+4, 0+2+4+5] */
		for (j=0; j<i; j++)
			out_skip[i] += out_count[j];
	}
}