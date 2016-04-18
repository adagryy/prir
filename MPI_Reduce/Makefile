CC=mpicc
CFLAGS=-c -g -lmpi
LDFLAGS=
SOURCES=bytes.c divide.c map.c prog.c reduce.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=wcount	
RUN=mpirun -np 2

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
.c.o:
	$(CC) $(CFLAGS) $< -o $@
run: wcount
	${RUN} ./wcount --addr book
clean:
	rm -f *.o