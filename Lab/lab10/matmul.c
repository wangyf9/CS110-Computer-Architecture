#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MATRIX_SIZE 1024

void matmul_naive (double *a, double *b, double *c)
{
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      for (int k = 0; k < MATRIX_SIZE; k++) {
        c[i * MATRIX_SIZE + j] += a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
      }
    }
  }
}

void matmul_optimized_slices (double *a, double *b, double *c)
{
  int thread_id,num_thread;
  double tmp;
  #pragma omp parallel private(tmp,thread_id,num_thread)//num_threads(32)
  {
    num_thread = omp_get_num_threads();
    thread_id= omp_get_thread_num();
    for (int i = thread_id; i < MATRIX_SIZE; i+=num_thread) {
      for (int j = 0; j < MATRIX_SIZE; j++) {
         tmp=0;
        for (int k = 0; k < MATRIX_SIZE; k++) {
          tmp+= a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];//相当于设置私有变量
        }
        c[i * MATRIX_SIZE + j]=tmp;
      }
    }
  }
}

void matmul_optimized_chunks (double *a, double *b, double *c)
{ 
  double tmp;
  int num_id;
  int size_block;
  #pragma omp parallel num_threads(32) private(tmp,num_id)
  {
    num_id = omp_get_thread_num();
    size_block=MATRIX_SIZE/omp_get_num_threads();
    for (int i = num_id*size_block; i < size_block*(1+num_id); i++) {
      for (int j = 0; j < MATRIX_SIZE; j++) {
        tmp=0;
        for (int k = 0; k < MATRIX_SIZE; k++) {
          tmp+= a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];//相当于设置私有变量
        }
        c[i * MATRIX_SIZE + j]=tmp;
      }
    }
  }
}

int main()
{
  double *a = (double *) malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
  double *b = (double *) malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
  double *c = (double *) calloc(MATRIX_SIZE * MATRIX_SIZE, sizeof(double));

  if (!a || !b || !c) {
    printf("Error: could not allocate memory.\n");
    return 1;
  }

  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      a[i * MATRIX_SIZE + j] = (double) rand() / RAND_MAX;
      b[i * MATRIX_SIZE + j] = (double) rand() / RAND_MAX;
    }
  }

  double start_time = omp_get_wtime();
  matmul_naive(a, b, c);
  double end_time = omp_get_wtime();
  printf("Naive: %.5f seconds\n", end_time - start_time);

  start_time = omp_get_wtime();
  matmul_optimized_slices(a, b, c);
  end_time = omp_get_wtime();
  printf("Optimized (slices): %.5f seconds\n", end_time - start_time);

  start_time = omp_get_wtime();
matmul_optimized_chunks(a, b, c);
end_time = omp_get_wtime();
printf("Optimized (chunks): %.5f seconds\n", end_time - start_time);

free(a);
free(b);
free(c);

return 0;
}
