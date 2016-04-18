#include <stdio.h>
#include "bytes.h"

extern int world_size; /* liczba procesow */

void bytes(int size, int* indexes, int* count, int* skip, int* out_count_bytes, int* out_skip_bytes) {
	int i;
	/*            0123456789  */
	/* text    = "oto tekst!" */
	/* indexes = [3, 9]       */
	/* indexes zawiera indeksy koncow podciagow w text */
	for (i=0; i<world_size; i++) {
		if (i>0) out_skip_bytes[i] = indexes[skip[i]-1];
		out_count_bytes[i] = indexes[skip[i]+count[i]-1];
	}

	for (i= --size; i>0; i--)
		out_count_bytes[i] -= out_count_bytes[i-1];
}