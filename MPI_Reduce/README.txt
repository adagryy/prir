To run program correctly you should execute commands like:
make
mpirun -n 2 ./wcount --addr book.log

For convenience purposes we recommend:
clear && make && make clean && mpirun -n 2 ./wcount --addr book.log