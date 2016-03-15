#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#define N 2

// Compile program in this way:
// cc -pthread threads.c -lm
// Then run:
// ./a.out


int x1, y_1, y2, x2;//"y1" is a built-in function, so I named it "y_1"

pthread_mutex_t mymutex=PTHREAD_MUTEX_INITIALIZER;

//global variables storing matrices
double** m1;// A
double** m2;// B
double** temp;// Threads IDs
double** result_matrix;//here we store results of multiplication

double general_sum = 0, to_frobenius_norm = 0;//"to_frobenius_norm" - sum squares of matrix elements

double** create_matrix(int rows, int cols) {
    int i,j;
    double** M = malloc(rows*sizeof(double*));
    for(i=0; i<rows; i++) {
        M[i] = malloc(cols*sizeof(double));
        for (j=0; j<cols; j++)
            M[i][j]=0;
    }
    return M;//creates new matrix
}

double** read_data(FILE *file, int *x, int *y){
	int i,j;
	/*matrix*/
	/*Use double , you have floating numbers not int*/
	fscanf(file, "%d%d", x, y);

	double** mat=malloc((*x)*sizeof(double*)); 
	for(i=0;i<*x;++i)
	mat[i]=malloc((*y)*sizeof(double));
	 for(i = 0; i < *x; i++)
	  {
	      for(j = 0; j < *y; j++) 
	      {
	  //Use lf format specifier, %c is for character
	       if (!fscanf(file, "%lf", &mat[i][j])) 
	           break;
	      // mat[i][j] -= '0'; 
	       // printf("%lf\n",mat[i][j]); //Use lf format specifier, \n is for new line
	      }

	  }
	  fclose(file);
	  return mat;//reads matrix from external file
}

void print_matrix(double** M, int rows, int cols) {
    int i, j;
    for (i=0; i<rows; i++) {
        for (j=0; j<cols; j++)
            printf("%f ", M[i][j]);
        printf("\n");
    }
}

void free_matrix(double** M, int rows, int cols) {
    int i;
    for(i=0; i<rows; i++)
        free(M[i]);
    free(M);
}

void multiply_matrices(double**A, int a, int d, int b, double**B, double**C)//calculates partial result A, B - input matrices, b - rows of A matrix (cols of B matrix) C - result matrix, 
//a,d - parameters to calculate multiplication
//function calculates multiplication in cell C[a][d]
{
    int i, j, k;
    double s;        
    s = 0;
    for(k=0; k<b; k++){
        s+=A[a][k]*B[k][d];
    }
    C[a][d] = s; 
}

void assign_matrix_areas_to_threads(double** result_matrix, int n, int x1, int y2, int elems){
	int i, j, iters;
	  if(elems <= N){
	  	iters = 0;
	  	for(i = 0; i < x1; i++)
	  		for(j = 0; j < y2; j++){
	  			result_matrix[i][j] = iters;
	  			iters += 1;
	  		}
	  }else{
	  	iters = 0;
	  	for(i = 0; i < x1; i++)
	  		for(j = 0; j < y2; j++){
	  			result_matrix[i][j] = iters;
	  			iters += 1;
	  			if(iters == N)
	  				iters = 0;
	  		}
	  }
}

void *calculate(void *threadid)
{
	
   long tid;
   tid = (long)threadid;
   int i, j;
   
   for(i = 0; i < x1; i++){ 
   	 for(j = 0; j < y2; j++){
   	    	if((int)tid == (int)temp[i][j]){
   	    		temp[i][j] = -1;
   	    		multiply_matrices(m1, i, j, y_1, m2, result_matrix); //multiplying of i-th row of A(m1) and j-th column of B(m2, B has y_1 rows, and saving 
   	    															 //calculated value to "result_matrix"
   	    		pthread_mutex_lock(&mymutex);
   	    			general_sum += result_matrix[i][j];
   	    			to_frobenius_norm += result_matrix[i][j] * result_matrix[i][j];
   	    		pthread_mutex_unlock(&mymutex);
   	    	}
   	    }
   	} 
   pthread_exit(NULL);
}

int main(){
	  pthread_t th[N];
	  int elems, i;
	  FILE *file1, *file2;
	  file1=fopen("A.txt", "r");
	  file2=fopen("B.txt", "r");

	  m1 = read_data(file1, &x1, &y_1);
	  m2 = read_data(file2, &x2, &y2);
	  result_matrix = create_matrix(x1, y2);
	  temp = create_matrix(x1, y2); //temp array for storing threads IDs in order to decide how to give to threads proper cells in results_matrix array

	  elems = x1 * y2;

	  assign_matrix_areas_to_threads(temp, N, x1, y2, elems);

	  // Here you can see temp matrix with threads IDs by uncomementing line under:
	  // print_matrix(temp, x1, y2);

	  for(i = 0; i < N; i++){	  
	  	    if ( pthread_create( &th[i], NULL, calculate, (void *)i) ) {
	  		    printf("error creating thread.");
	  		    abort();
	  		  }
	  	}

	  	for(i = 0; i < N; i++){
		  if ( pthread_join ( th[i], NULL ) ) {
		    printf("error joining thread.");
		    abort();
		  }
	}
	  printf("A = [\n");
	  print_matrix(m1, x1, y_1);
	  printf("]\n");
	  printf("B = [\n");
	  print_matrix(m2, x2, y2);
	  printf("]\n");
	  printf("C = [\n");
	  print_matrix(result_matrix, x1, y2);
	  printf("]\n");

	  double frobenius_norm = sqrt(to_frobenius_norm);

	  // printf("Suma elementów: %lf\nNorma Frobeniusa: %lf\n", general_sum, frobenius_norm);
	  printf("Norma Frobeniusa: %lf\n", frobenius_norm);



	  free_matrix(temp, x1, y2);
	  free_matrix(result_matrix, x1, y2);
	  free_matrix(m1, x1, y_1);
	  free_matrix(m2, x2, y2);
	return 0;
}

/*
0.834302 0.592958 
1.581957 1.104131 
1.489694 0.974431 
1.764281 1.089746 
1.343201 0.911115 
*/