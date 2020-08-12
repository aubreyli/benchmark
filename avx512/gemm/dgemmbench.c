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

uint64_t double_cycles[MAXMATRIX];
uint64_t double_count[MAXMATRIX];
double double_reference[MAXMATRIX];


uint64_t mygemm_cycles[MAXMATRIX];
uint64_t mygemm_count[MAXMATRIX];
double mygemm_reference[MAXMATRIX];

int reference_run;


int series[] = {32,1,32,1,2,4,8,16,32,64,128,256,512,1024,24,48,96,148,192};
//int series[] = {64};
//int series[] = {1,64,512,1024,2000};
//int series[] = {1, 64, 65, 80, 96, 112, 128, 48};


double * allocate_double_matrix(int rows, int cols)
{
	double *M;
	int i, size;

	size = rows * cols;

	M = malloc(size * sizeof(double));

	for (i = 0; i < size; i++)
		M[i] = 10 * (drand48() - 0.5);

	return M;
}

double * allocate_mygemm_matrix(int rows, int cols)
{
	double *M = NULL;
	void *m = NULL;
	int i, size;
	int ret;

	size = rows * cols;

	ret = posix_memalign(&m, 64, size);
	M = m;
	if (!ret)
		M = malloc(size * sizeof(double));

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


void assert_correctness_double(double *A, double *B, double *result, double epsilon, int rows, int cols)
{
	double *reference;
	int i, j, k;

	reference = allocate_double_matrix(rows, cols);

	for(i = 0; i < cols; i++) {
		for(j = 0; j < rows; j++) {
			double R = 0;
			reference[i * rows + j] = 0;
		   	for(k = 0; k < rows; k++) {
				R += (double)A[k * rows + j] * (double)B[k + rows * i];
			}
			reference[i * rows + j] = R;
		}
	}

	for(i = 0; i < cols; i++) {
		for(j = 0; j < rows; j++) {
			double delta;

			delta = reference[i * rows + j] - result[i * rows + j];
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

void assert_correctness_mygemm(double *A, double *B, double *result, double epsilon, int rows, int cols)
{
	double *reference;
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


void save_reference(void)
{
	FILE *file;
	int i;

	memset(double_reference, 0, sizeof(double_reference));
	for (i = 0; i < MAXMATRIX; i++) {
		if (double_count[i] > 0) {
			double_reference[i] = 1.0 * double_cycles[i] / double_count[i];
		}
	}

	memset(mygemm_reference, 0, sizeof(mygemm_reference));
	for (i = 0; i < MAXMATRIX; i++) {
		if (mygemm_count[i] > 0) {
			mygemm_reference[i] = 1.0 * mygemm_cycles[i] / mygemm_count[i];
		}
	}

	file = fopen("reference.dat", "w");
	if (!file)
		return;
	fwrite(double_reference, sizeof(double), MAXMATRIX, file);
	fwrite(mygemm_reference, sizeof(double), MAXMATRIX, file);
	fclose(file);
}

void load_reference(void)
{
	FILE *file;
	int ret = 0;

	file = fopen("reference.dat", "r");
	if (!file)
		return;
	ret += fread(double_reference, sizeof(double), MAXMATRIX, file);
	ret += fread(mygemm_reference, sizeof(double), MAXMATRIX, file);
	if (ret == 0)
		fclose(file);
}

char *ratio(double before, double after)
{
	static char ratio_buffer[256];

	double delta;

	delta = before - after;

	if (fabs(delta) > before / 10) {
		/* we're in multiple space */
		if (before > after)
			sprintf(ratio_buffer,"%4.1fx", before/after);
		else
			sprintf(ratio_buffer,"%4.1fx", -after/before);
	} else {
		/* percentages */
		if (before > after)
			sprintf(ratio_buffer,"%4.1f%%", 100.0 * (delta) / after);
		else
			sprintf(ratio_buffer,"%4.1f%%", 100.0 * (delta) / before);
	}
	



	return ratio_buffer;
}

void print_results(void)
{
	int i;
	double double_offset = 0, mygemm_offset = 0;

	printf("\e[2J\e[H");


	if (double_count[1])
		double_offset = 1.0 * double_cycles[1] / double_count[1] - 4;

	if (mygemm_count[1])
		mygemm_offset = 1.0 * mygemm_cycles[1] / mygemm_count[1] - 4;

	printf("   Matrix\t\t SGEMM cycles\t   MPC\t\t\t\tmyGEMM cycles\t   MPC\n");
	for (i = 0; i < MAXMATRIX; i++) {
		if (double_count[i] > 0) {
			printf("%4i x %-4i\t\t%13.1f\t %5.2f", i, i, 1.0 * double_cycles[i] / double_count[i],  (1.0*i*i*i)/(1.0 * double_cycles[i] / double_count[i] - double_offset));
			if (double_reference[i] != 0.0) 
				printf("\t%s\t", ratio(double_reference[i], double_cycles[i] / double_count[i]));
			else
				printf("\t\t\t");


			if (mygemm_count[i] > 0) {
				printf("\t\t%13.1f\t %5.2f", 1.0 * mygemm_cycles[i] / mygemm_count[i],  (1.0*i*i*i)/(1.0 * mygemm_cycles[i] / mygemm_count[i] - mygemm_offset));
				if (double_reference[i] != 0.0) 
					printf("\t%s\t", ratio(double_reference[i], mygemm_cycles[i] / mygemm_count[i]));
			}
			printf("\n");
		}
	}

}


void bench_double(int rows, int cols, int duration)
{
	double *A, *B, *result;
	uint64_t total = 0, before, after;
	int iter = 0;
	int64_t start;

	A = allocate_double_matrix(rows, cols);
	B = allocate_double_matrix(rows, cols);
	result = allocate_double_matrix(rows, cols);


	cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, rows, cols, cols, 1.0, A, rows, B, cols, 0.0, result, rows);
	assert_correctness_double(A, B, result, rows * 1E-4, rows, cols);

	start = time(NULL);
	while (time(NULL) < start + 1 + duration) {
		before = rdtsc();
		cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, rows, cols, cols, 1.0, A, rows, B, cols, 0.0, result, rows);
		after = rdtsc();
		total += after - before;
		iter++;
	}

	free(A);
	free(B);
	free(result);

	double_cycles[rows] += total;
	double_count[rows] += iter;

	print_results();

	if (reference_run)
		save_reference();
}

extern void mydgemm(int M, int N, int K, double *A, int strideA, double *B, int strideB , double *R, int strideR);


void bench_mygemm(int rows, int cols, int duration)
{
	double *A, *B, *result;
	uint64_t total = 0, before, after;
	int iter = 0;
	int64_t start;

	A = allocate_mygemm_matrix(rows, cols);
	B = allocate_mygemm_matrix(rows, cols);
	result = allocate_mygemm_matrix(rows, cols);


	mydgemm(rows, cols, cols,A, rows, B, cols, result, rows);
	assert_correctness_mygemm(A, B, result, rows * 1E-4, rows, cols);

	start = time(NULL);
	while (time(NULL) < start + 1 + duration) {
		before = rdtsc();
		mydgemm(rows, cols, cols, A, rows, B, cols, result, rows);
		after = rdtsc();
		total += after - before;
		iter++;
	}

	free(A);
	free(B);
	free(result);

	mygemm_cycles[rows] += total;
	mygemm_count[rows] += iter;

	print_results();

	if (reference_run)
		save_reference();
}



int main(int argc, char **argv)
{
	unsigned int i;
	int loop = 0;
//	srand48(time(NULL));
	if (argc > 1) {
		printf("Argument %s passed\n", argv[1]);
		reference_run = 1;
		loop = 4;
	} else {
		load_reference();
		reference_run = 1;
		loop = 0;
	}


	while (1) {
		for (i = 0; i < sizeof(series)/sizeof(int); i++) {
			bench_double(series[i], series[i], loop);
			if (loop)
				sleep(1);
			bench_mygemm(series[i], series[i], loop);
			if (loop)
				sleep(1);
		}
		loop++;
		if (loop > 10)
			break;
	}



	return EXIT_SUCCESS;
}

