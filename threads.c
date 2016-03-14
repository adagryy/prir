#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#define N 13

double** result_matrix;//here we store results of multiplication
int x1, y_1, y2, x2;//"y1" is a built-in function, so I named it "y_1"

pthread_mutex_t mymutex=PTHREAD_MUTEX_INITIALIZER;

double** m1;
double** m2;

double general_sum = 0, to_frobenius_norm = 0;

double** create_matrix(int rows, int cols) {
    int i,j;
    double** M = malloc(rows*sizeof(double*));
    for(i=0; i<rows; i++) {
        M[i] = malloc(cols*sizeof(double));
        for (j=0; j<cols; j++)
            M[i][j]=0;
    }
    return M;
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
	  return mat;
}

void print_matrix(double** M, int rows, int cols) {
    int i, j;
    printf("%d %d\n", rows, cols);
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
    {
         //for(j=0; j<d; j++)
        {
            s = 0;

            for(k=0; k<b; k++)
            {
                 //s+=A[i][k]*B[k][j];
                s+=A[a][k]*B[k][d];
            }
            C[a][d] = s;
        }

    }
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

void *PrintHello(void *threadid)
{
	
   long tid;
   tid = (long)threadid;
   // pthread_mutex_lock(&mymutex);
   int i, j;
   // int cx1 = x1, cy2 = y2;
   
   for(i = 0; i < x1; i++){ 
   	 for(j = 0; j < y2; j++){
   	    	if((int)tid == (int)result_matrix[i][j]){
   	    		multiply_matrices(m1, i, j, y_1, m2, result_matrix);  
   	    		pthread_mutex_lock(&mymutex);
   	    			general_sum += result_matrix[i][j];
   	    			to_frobenius_norm += result_matrix[i][j] * result_matrix[i][j];
   	    		pthread_mutex_unlock(&mymutex);

   	    		// printf("%d %d, \n", i, j);
   	    	}
   	    }
   	}
   	  		// multiply_matrices(m1, i, j, x1, m2, result_matrix);  
   	// 
   pthread_exit(NULL);
}

int main(){

	  pthread_t th[N];
	  int elems, i;
	  FILE *file1, *file2;
	  file1=fopen("first.txt", "r");
	  file2=fopen("second.txt", "r");

	  m1 = read_data(file1, &x1, &y_1);
	  m2 = read_data(file2, &x2, &y2);
	  result_matrix = create_matrix(x1, y2);

	  elems = x1 * y2;

	  assign_matrix_areas_to_threads(result_matrix, N, x1, y2, elems);



	  for(i = 0; i < N; i++){	  
	  	    if ( pthread_create( &th[i], NULL, PrintHello, (void *)i) ) {
	  		    printf("error creating thread.");
	  		    abort();
	  		  }
	  	}

	  // multiply_matrices(m1, 4, 0, y_1, m2, result_matrix);

	  	for(i = 0; i < N; i++){
		  if ( pthread_join ( th[i], NULL ) ) {
		    printf("error joining thread.");
		    abort();
		  }
	}
	  // print_matrix(m1, x1, y_1);
	  // print_matrix(m2, x2, y2);
	  print_matrix(result_matrix, x1, y2);

	  double frobenius_norm = sqrt(to_frobenius_norm);

	  printf("Suma elementów: %lf\nNorma Frobeniusa: %lf\n", general_sum, frobenius_norm);

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