CC=mpicc #gcc
CFLAGS=-lmpi
RUN=mpirun -np 2
fscount: prog.c
	${CC} prog.c -g -o wcount ${CFLAGS}
run: wcount
	${RUN} ./wcount
