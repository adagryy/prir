/* Rownolegly program calkowania metoda trapezow.
 * * Wejscie: brak.
 * * Wyjscie: Estymowana wartosc calki oznaczonej od a do b funkcji f(x)
 * * obliczonej z uzyciem n trapezow.
 * *
 * * Algorytm:
 * * 1. Kazdy proces wyznacza "swoj" przedzial calkowania.
 * * 2. Kazdy proces wyznacza przyblizenie calki funkcji f(x)
 * * w swoim przedziale stosujac metode trapezow.
 * * 3. Kazdy proces != 0 wysyla swoja wartosc calki do procesu 0.
 * * 4. Proces 0 sumuje wyniki obliczen otrzymane od poszczegolnych
 * * procesow i drukuje wynik
 * * Uwaga: f(x), a, b i n sa ustalone w tekscie programu.
 * */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
int main(int argc, char** argv) {
	int id; /* identyfikator (ranga) procesu */
	int p; /* liczba procesow */


	// ./integrate begin end num_points

	if (argc != 4){
		printf("Niepoprawna ilość argumentów/niepoprawne argumenty\n");
		return 0;
	}	

	// float a = 0.0; /* lewy koniec przedzialu */
	// float b = 0.5; /* prawy koniec przedzialu */
	// int n = 1024; /* liczba przedzialow (trapezow) */
	float a, b;
	int n;

	a = atof(argv[1]);
	b = atof(argv[2]);
	n = atoi(argv[3]);

	// if ((a = atof(argv[1])) == 0){
	// 	printf("Niepoprawne dane wejściowe początku przedziału\n");
	// 	return 0;
	// }

	// if ((b = atof(argv[2])) == 0){
	// 	printf("Niepoprawne dane wejściowe końca przedziału\n");
	// 	return 0;
	// }

	// if ((n = atoi(argv[3])) == 0){
	// 	printf("Niepoprawne dane wejściowe ilości\n");
	// 	return 0;
	// }
	float h; /* dlugosc przedzialu */
	float lokal_a; /* lokalny, lewy koniec przedzialu */
	float lokal_b; /* lokalny, prawy koniec przedzialu */
	int lokal_n; /* lokalna liczba przedzialow */
	float calka; /* lokalna wartosc calki */
	float suma; /* pelna wartosc calki */
	int zrodlo; /* identyfikator procesu zrodlowego */
	int docel = 0; /* identyfikator procesu docelowego, tj. procesu 0 */
	int typ = 0; /* typ (znacznik) wiadomosci */
	MPI_Status status; /* status powrotu dla funkcji receive */
	float obl_calke(float lokal_a, float lokal_b, /* funkcja lokalnego */
			int lokal_n, float h); /* obliczania calki */
		MPI_Init(&argc, &argv); /* inicjacja MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &id); /* zwraca identyfikator procesu */
	MPI_Comm_size(MPI_COMM_WORLD, &p); /* zwraca liczbe procesow */
	h = (b - a) / (float)n; /* h jest to samo dla wszystkich procesow */
	lokal_n = n / p; /* ta sama liczba przedzialow w kazdym procesie */
	lokal_a = a + (float)id *
		(float)lokal_n * h; /* wyznaczenie lewego i prawego */
	lokal_b = lokal_a + (float)lokal_n * h; /* konca przedzialu */
	calka = obl_calke(lokal_a, lokal_b, lokal_n, h);

	//     MPI_Scatter(globaldata, 1, MPI_INT, &localdata, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//     printf("Processor %d has data %d\n", rank, localdata);
	//     localdata *= 2;
	//     printf("Processor %d doubling the data, now has %d\n", rank, localdata);

	//     MPI_Gather(&localdata, 1, MPI_INT, globaldata, 1, MPI_INT, 0, MPI_COMM_WORLD);
	// MPI_Scatter(globaldata, 1, MPI_INT, &localdata, 1, MPI_INT, 0, MPI_COMM_WORLD);


	MPI_Scatter(&calka, 1, MPI_FLOAT, &docel, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
	suma = suma + calka;
	// MPI_Gather(&localdata, 1, MPI_INT, globaldata, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Gather(&docel, 1, MPI_FLOAT, &calka, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
	//===============
	// if (id == 0) { /* dla procesu 0 */
	// 	suma = calka;
	// 	for (zrodlo = 1; zrodlo < p; zrodlo++) {
	// 		// MPI_Scatter(sendbuf,sendcount,MPI_FLOAT,recvbuf,recvcount, MPI_FLOAT, source, MPI_COMM_WORLD);
	// 		   MPI_Recv(&calka, 1, MPI_FLOAT, zrodlo, typ,	MPI_COMM_WORLD, &status);
	// 		suma = suma + calka;
	// 	}
	// }
	// else { /* dla procesow o numerach != 0 */
	// 	/* wyslanie lokalnej wartosci calki */
	// 	MPI_Send(&calka, 1, MPI_FLOAT, docel, typ, MPI_COMM_WORLD);
	// }
	//===============
	/* drukowanie wyniku */
	if (id == 0) {
		printf("Liczba procesow: %d\n", p);
		printf("Przy n = %d trapezach, przyblizenie calki\n", n);
		printf("w granicach od %f do %f wynosi %f\n", a, b, suma);
	}
	/* zamkniecie MPI */
	MPI_Finalize();
	return 0;
}
float obl_calke(float lokal_a, float lokal_b,
		int lokal_n, float h) {
	float calka; /* lokalna wartosc calki */
	float x;
	int i;
	float f(float x); /* funkcja ktora calkujemy */
	calka = (f(lokal_a) + f(lokal_b)) / 2.0;
	x = lokal_a;
	for (i = 1; i <= lokal_n - 1; i++) {
		x = x + h;
		calka = calka + f(x);
	}
	calka = calka * h;
	return calka;
} /* obl_calke */
float f(float x) {
	float wart;
	/* obliczenie f(x) */
	wart = x * x;
	return wart;
}

// int main(int argc, char **argv) {
//     int size, rank;

//     MPI_Init(&argc, &argv);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//     int *globaldata=NULL;
//     int localdata;

//     if (rank == 0) {
//         globaldata = malloc(size * sizeof(int) );
//         for (int i=0; i<size; i++)
//             globaldata[i] = 2*i+1;

//         printf("Processor %d has data: ", rank);
//         for (int i=0; i<size; i++)
//             printf("%d ", globaldata[i]);
//         printf("\n");
//     }

//     MPI_Scatter(globaldata, 1, MPI_INT, &localdata, 1, MPI_INT, 0, MPI_COMM_WORLD);

//     printf("Processor %d has data %d\n", rank, localdata);
//     localdata *= 2;
//     printf("Processor %d doubling the data, now has %d\n", rank, localdata);

//     MPI_Gather(&localdata, 1, MPI_INT, globaldata, 1, MPI_INT, 0, MPI_COMM_WORLD);

//     if (rank == 0) {
//         printf("Processor %d has data: ", rank);
//         for (int i=0; i<size; i++)
//             printf("%d ", globaldata[i]);
//         printf("\n");
//     }

//     if (rank == 0)
//         free(globaldata);

//     MPI_Finalize();
//     return 0;
// }
