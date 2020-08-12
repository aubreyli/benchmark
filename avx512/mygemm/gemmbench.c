/*

        gcc -O3 -Wall -W -march=native gemmbench.c -o gemmbench   -lopenblas -lpthread -lgfortran -fopenmp


usage:

./gemmbench --baseline

to create a baseline measurement; no arguments otherwise


Licensed under the Apache-2.0 License

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <cblas.h>


#define MAXMATRIX 2048

uint64_t mygemm_cycles[MAXMATRIX];
uint64_t mygemm_count[MAXMATRIX];
double mygemm_reference[MAXMATRIX];

int series[] = {64,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,64,128,256,512,1024,24,48,96,80,72,88,148,192, 1024, 512, 256, 128, 64,12};
//int series[] = {1,64,512,1024,2000};
//int series[] = {1, 64, 65, 80, 96, 112, 128, 48};
#int series[] = {128};


float * allocate_mygemm_matrix(int rows, int cols)
{
	float *M = NULL;
	int i, size;
	int ret;

	size = rows * cols;

	ret = posix_memalign(&M, 64, size);
	if (!ret)
		M = malloc(size * sizeof(float));

	for (i = 0; i < size; i++)
		M[i] = 100 * (drand48() - 0.5);

	return M;
}

static __attribute__((noinline)) unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

void assert_correctness_mygemm(float *A, float *B, float *result, double epsilon, int rows, int cols)
{
	float *reference;
	int i, j, k;

	reference = allocate_mygemm_matrix(rows, cols);

	for(i = 0; i < cols; i++) {
		for(j = 0; j < rows; j++) {
			double R = 0;
			reference[i * rows + j] = 0;
		   	for(k = 0; k < rows; k++) {
				R += (double)A[k + rows * i] * (double)B[k * rows + j];
			}
			reference[i * rows + j] = R;
		}
	}

	for(i = 0; i < cols; i++) {
		for(j = 0; j < rows; j++) {
			double delta;
			double value;

			delta = reference[i * rows + j] - result[i * rows + j];
			value = reference[i * rows + j];

			if (value > 1 || value < -1 )
				delta = delta / value;

			if (fabs(delta) > epsilon) {
				printf("(%i, %i)\n", i, j);
				printf("reference %1.10f\n",  reference[i * rows + j]);
				printf("result %1.10f\n", result[i * rows + j]);
				printf("Comparison failure %1.10f\n", delta);
				assert(0);
			}
		}
	}

	free(reference);
}

void print_results(int duration)
{
	int i;
	double mygemm_offset = 0;

	if (mygemm_count[1])
		mygemm_offset = 1.0 * mygemm_cycles[1] / mygemm_count[1] - 4;

	printf("(%d)Matrix\t\tmyGEMM cycles\tMPC\n", duration);
	for (i = 0; i < MAXMATRIX; i++) {
		if (mygemm_count[i] > 0) {
			printf("%4i x %-4i\t\t%13.1f\t %5.2f", i, i, 1.0 * mygemm_cycles[i] / mygemm_count[i],  (1.0*i*i*i)/(1.0 * mygemm_cycles[i] / mygemm_count[i] - mygemm_offset));
			printf("\n");
		}
	}
}

extern void mygemm(int M, int N, int K, float *A, int strideA, float *B, int strideB , float *R, int strideR);


void bench_mygemm(int rows, int cols, int duration)
{
	float *A, *B, *result;
	uint64_t total = 0, before, after;
	int iter = 0;
	int64_t start;

	A = allocate_mygemm_matrix(rows, cols);
	B = allocate_mygemm_matrix(rows, cols);
	result = allocate_mygemm_matrix(rows, cols);


	mygemm(rows, cols, cols,A, rows, B, cols, result, rows);
	assert_correctness_mygemm(A, B, result, rows * 1E-4, rows, cols);

	start = time(NULL);
	while (time(NULL) < start + 1 + duration) {
		before = rdtsc();
		mygemm(rows, cols, cols, A, rows, B, cols, result, rows);
		after = rdtsc();
		total += after - before;
		iter++;
	}

	free(A);
	free(B);
	free(result);

	mygemm_cycles[rows] += total;
	mygemm_count[rows] += iter;

	print_results(duration);

}



int main(int argc, char **argv)
{
	unsigned int i;
	int loop = 0;

	while (1) {
		for (i = 0; i < sizeof(series)/sizeof(int); i++) {
			bench_mygemm(series[i], series[i], loop);
			sleep(1);
		}
		loop++;
		if (loop > 100000)
			break;
	}

	return EXIT_SUCCESS;
}
