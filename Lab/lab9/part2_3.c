#include <emmintrin.h> /* header file for the SSE intrinsics */
#include <time.h>
#include <stdio.h>

float **a;
float **b;
float **c;
float **c2;

int n = 40000;
int p = 20000;

void init(void) {
    a = malloc(n * sizeof(float *));
    b = malloc(4 * sizeof(float *));
    c = malloc(n * sizeof(float *));
    c2 = malloc(n * sizeof(float *));
    for (int i = 0; i < n; ++i) {
        a[i] = malloc(4 * sizeof(float));
        c[i] = malloc(p * sizeof(float));
        c2[i] = malloc(p * sizeof(float));
    }
    for (int i = 0; i < 4; ++i) {
        b[i] = malloc(p * sizeof(float));
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < 4; ++j) {
            a[i][j] = (float) rand() / (float) RAND_MAX;
        }
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < p; ++j) {
            b[i][j] = (float) rand() / (float) RAND_MAX;
        }
    }
}

void check_correctness(char *msg) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < p; ++j) {
            if (c[i][j] != c2[i][j]) {
                printf("%s incorrect!\n", msg);
                return;
            }
        }
    }
}

void naive_matmul(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // c = a * b
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < p; ++j) {
            c[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("naive: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
}

void loop_unroll_matmul(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i=0; i<n; ++i){
        for(int j=0; j<p; j+=4){
            c[i][j]=0;
            c[i][j+1]=0;
            c[i][j+2]=0;
            c[i][j+3]=0;
            for (int k = 0; k < 4; ++k) {
                c[i][j] += a[i][k] * b[k][j];
                c[i][j+1] += a[i][k] * b[k][j+1];
                c[i][j+2] += a[i][k] * b[k][j+2];
                c[i][j+3] += a[i][k] * b[k][j+3];
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("unroll: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
    check_correctness("loop_unroll_matmul");
}

void simd_matmul(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    // c2 = a * b
    // TODO: implement me!
        __m128 x,y,z,sum;
    // c2 = a * b
    // TODO: implement me!
    for(int i=0;i<n;++i){
        for(int j=0;j<p;j+=4){/*because every time we take four elements to conduct the work*/
            sum=_mm_setzero_ps();
            /*for(int k=0;k<4;++k){
                x=_mm_set1_ps((*(*(a+i)+k)));because we take four elements as a group
                y=_mm_loadu_ps((*(b+k)+j));
                z=_mm_mul_ps(x,y);
                sum=_mm_add_ps(z,sum);
            }
            _mm_storeu_ps((*(c2+i)+j),sum);*/
            for(int k=0;k<4;++k){
                x=_mm_set1_ps(a[i][k]);/*because we take four elements as a group*/
                y=_mm_loadu_ps(&b[k][j]);
                z=_mm_mul_ps(x,y);
                sum=_mm_add_ps(z,sum);
            }
            _mm_storeu_ps(&c2[i][j],sum);/*so this is because of data parallelism*/
            
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("simd: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
    check_correctness("simd_matmul");
}

void loop_unroll_simd_matmul(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    // c2 = a * b
    // TODO: implement me!
    __m128 sum_1,sum_2,sum_3,sum_4,x1,y1,y2,y3,y4,z1,z2,z3,z4;
    for(int i=0; i<n; ++i){
        for(int j=0; j<p; j+=16){
            sum_1=_mm_setzero_ps();
            sum_2=_mm_setzero_ps();
            sum_3=_mm_setzero_ps();
            sum_4=_mm_setzero_ps();
            for (int k = 0; k < 4; ++k) {
                x1=_mm_set1_ps(a[i][k]);/*because we take four elements as a group*/
                y1=_mm_loadu_ps(&b[k][j]);
                y2=_mm_loadu_ps(&b[k][j+4]);
                y3=_mm_loadu_ps(&b[k][j+8]);
                y4=_mm_loadu_ps(&b[k][j+12]);
                z1=_mm_mul_ps(x1,y1);
                z2=_mm_mul_ps(x1,y2);
                z3=_mm_mul_ps(x1,y3);
                z4=_mm_mul_ps(x1,y4);

                sum_1=_mm_add_ps(z1,sum_1);
                sum_2=_mm_add_ps(z2,sum_2);
                sum_3=_mm_add_ps(z3,sum_3);
                sum_4=_mm_add_ps(z4,sum_4);
            }
            _mm_storeu_ps(&c2[i][j],sum_1);
            _mm_storeu_ps(&c2[i][j+4],sum_2);
            _mm_storeu_ps(&c2[i][j+8],sum_3);
            _mm_storeu_ps(&c2[i][j+12],sum_4);


        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("unroll+simd: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
    check_correctness("loop_unroll_simd_matmul");
}


int main(void) {
    init();

    naive_matmul();
    simd_matmul();
    loop_unroll_matmul();
    loop_unroll_simd_matmul();

    return 0;
}

