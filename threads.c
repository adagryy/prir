#include <stdlib.h>
#include <stdio.h>
#define N 10

double** result_matrix;
int x1, y_1, y2, x2;//"y1" is a built-in function, so I named it "y_1"

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

void multiply_matrices(double**A, int a, int b, double**B, int c, int d, double**C)
{
    int i, j, k;
    double s;
    for(i=0; i<a; i++)
    {
        for(j=0; j<d; j++)
        {
            s = 0;
            for(k=0; k<b; k++)
            {
                s+=A[i][k]*B[k][j];
            }
            C[i][j] = s;
        }

    }
}


int main(){
	
	  FILE *file1, *file2;
	  file1=fopen("first.txt", "r");
	  file2=fopen("second.txt", "r");

	  double** m1 = read_data(file1, &x1, &y_1);
	  double** m2 = read_data(file2, &x2, &y2);
	  result_matrix = create_matrix(5,2);

	  print_matrix(m1, x1, y_1);
	  print_matrix(m2, x2, y2);
	  print_matrix(result_matrix, x1, y2);

	  free_matrix(result_matrix, x1, y2);
	  free_matrix(m1, x1, y_1);
	  free_matrix(m2, x2, y2);
	return 0;
}
