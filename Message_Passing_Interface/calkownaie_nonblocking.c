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

	MPI_Request request, request2;

	float obl_calke(float lokal_a, float lokal_b, /* funkcja lokalnego */
			int lokal_n, float h); /* obliczania calki */

	MPI_Init(&argc, &argv); /* inicjacja MPI */
	MPI_Comm_size(MPI_COMM_WORLD, &p); /* zwraca liczbe procesow */
	MPI_Comm_rank(MPI_COMM_WORLD, &id); /* zwraca identyfikator procesu */
	
	h = (b - a) / (float)n; /* h jest to samo dla wszystkich procesow */
	lokal_n = n / p; /* ta sama liczba przedzialow w kazdym procesie */
	lokal_a = a + (float)id *
		(float)lokal_n * h; /* wyznaczenie lewego i prawego */
	lokal_b = lokal_a + (float)lokal_n * h; /* konca przedzialu */
	calka = obl_calke(lokal_a, lokal_b, lokal_n, h);
	if (id == 0) { /* dla procesu 0 */
		suma = calka;
		for (zrodlo = 1; zrodlo < p; zrodlo++) {
			MPI_Irecv(&calka, 1, MPI_FLOAT, zrodlo, typ, MPI_COMM_WORLD, &request);
			suma = suma + calka;
		}
	}
	else { /* dla procesow o numerach != 0 */
		/* wyslanie lokalnej wartosci calki */

	// int  MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
	// int MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)

		MPI_Isend(&calka, 1, MPI_FLOAT, docel, typ, MPI_COMM_WORLD, &request2);
	}
	
	if(docel != 0 && zrodlo != 0){
		MPI_Wait(&request, &status);
	    MPI_Wait(&request2, &status);
	}

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
