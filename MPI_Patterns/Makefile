CC=mpicc #gcc
CFLAGS=-lmpi
RUN=mpirun -np 2
fscount: zad1.c
	${CC} zad1.c -g -o wcount ${CFLAGS}
run: wcount
	${RUN} ./wcount
