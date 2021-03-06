#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main(int argc, char**argv){
int n_threads;


int N = 10000000,  S = (int)sqrt(N), M = N/10;


argc > 1 ? (((atoi(argv[1])) == 0) ? (n_threads = 2) : (n_threads = atoi(argv[1]))) : (n_threads = 2);

long int a[S + 1]; /*tablica pomocnicza*/
long int pierwsze[M]; /*liczby pierwsze w przedziale 2..N*/
long i, k, liczba, reszta;
long int lpodz; /* l. podzielnikow*/
long int llpier =0; /*l. liczb pierwszych w tablicy pierwsze*/
double czas; /*zmienna do  mierzenia czasu*/
FILE *fp;
/*wyznaczanie podzielnikow z przedzialow 2..S*/

for(i=2; i<=S; i++)
	a[i] = 1; /*inicjowanie*/
for(i=2; i<=S; i++)
	if(a[i] == 1){
		pierwsze[llpier++] = i; /*zapamietanie podzielnika*/
		/*wykreslanie liczb zlozonych bedacych wielokrotnosciami i*/
	for(k = i+i; k<=S; k+=i) 
		a[k] = 0;
}

lpodz = llpier; /*zapamietanie liczby podzielnikow*/
/*wyznaczanie liczb pierwszych*/
omp_set_dynamic(0);
omp_set_num_threads(n_threads);


#pragma omp parallel for shared(S, N, lpodz, pierwsze) firstprivate(k, reszta)
for(liczba = S+1; liczba <=N; liczba++){
	for(k=0; k<lpodz; k++){
	reszta = (liczba % pierwsze[k]);
	if(reszta == 0) break; /*liczba zlozona*/
	}	
	if(reszta != 0){
		#pragma omp critical
		pierwsze[llpier++] = liczba;
	} /*zapamietanie liczby pierwszej*/
}
if((fp = fopen("primes.txt", "w")) == NULL)
{
	printf("Nie moge otworzyc pliku do zapisu\n");
	exit(1);
}
for(i=0; i< llpier; i++)
	fprintf(fp,"%ld ", pierwsze[i]);
fclose(fp);
return 0;
}
