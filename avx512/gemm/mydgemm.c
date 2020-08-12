#include <math.h>
#include <stdint.h> 
#include <immintrin.h> 
#include <assert.h> 
#include <stdio.h>


#define DECLARE_RESULT_512(N,M) __m512d result##N##M = _mm512_setzero_pd()
#define BROADCAST_LOAD_A_512(N,M) __m512d Aval##M = _mm512_broadcastsd_pd(_mm_load_sd(&A[k  + strideA * (i+M)]))
#define LOAD_B_512(N,M)  __m512d Bval##N = _mm512_loadu_pd(&B[strideB * k + j + (N*8)])
#define MATMUL_512(N,M)  result##N##M = _mm512_fmadd_pd(Aval##M, Bval##N , result##N##M)
#define STORE_512(N,M) _mm512_storeu_pd(&R[(i+M) * strideR + j+(N*8)], result##N##M)


#define DECLARE_RESULT_256(N,M) __m256d result##N##M = _mm256_setzero_pd()
#define BROADCAST_LOAD_A_256(N,M) __m256d Aval##M = _mm256_broadcastsd_pd(_mm_load_sd(&A[k  + strideA * (i+M)]))
#define LOAD_B_256(N,M)  __m256d Bval##N = _mm256_loadu_pd(&B[strideB * k + j + (N*4)])
#define MATMUL_256(N,M)  result##N##M = _mm256_fmadd_pd(Aval##M, Bval##N , result##N##M)
#define STORE_256(N,M) _mm256_storeu_pd(&R[(i+M) * strideR + j+(N*4)], result##N##M)

#define DECLARE_RESULT_128(N,M) __m128d result##N##M = _mm_setzero_pd()
#define BROADCAST_LOAD_A_128(N,M) __m128d Aval##M = _mm_movedup_pd(_mm_load_sd(&A[k  + strideA * (i+M)]))
#define LOAD_B_128(N,M)  __m128d Bval##N = _mm_loadu_pd(&B[strideB * k + j + (N*2)])
#define MATMUL_128(N,M)  result##N##M = _mm_fmadd_pd(Aval##M, Bval##N , result##N##M)
#define STORE_128(N,M) _mm_storeu_pd(&R[(i+M) * strideR + j+(N*2)], result##N##M)

#define DECLARE_RESULT_SCALAR(N,M) double result##M = 0;
#define BROADCAST_LOAD_A_SCALAR(N,M) double Aval##M = A[k + strideA * (i + M)];
#define LOAD_B_SCALAR(N,M)  double Bval  = B[k * strideB + j];
#define MATMUL_SCALAR(N,M) result##M +=  Aval##M * Bval;
#define STORE_SCALAR(N,M)  R[(i+M) * strideR + j] = result##M;

#define PREFETCH_DOWN(N,M)  __builtin_prefetch(&B[strideB * (k+M) + (j+8 * N)])


void mydgemm32(int N, int K, double *A, int strideA, double *B, int strideB , double *R, int strideR)
{
	int i, j;

	int n4 = N & ~3;
	int n2 = N & ~1;


	for(i = 0; i < (N & ~3); i+=8) {

		j = 0; {
			int k = 0;
			DECLARE_RESULT_512(0, 0);    DECLARE_RESULT_512(1, 0);   
			DECLARE_RESULT_512(0, 1);    DECLARE_RESULT_512(1, 1);  
			DECLARE_RESULT_512(0, 2);    DECLARE_RESULT_512(1, 2);  
			DECLARE_RESULT_512(0, 3);    DECLARE_RESULT_512(1, 3);  
			DECLARE_RESULT_512(0, 4);    DECLARE_RESULT_512(1, 4);   
			DECLARE_RESULT_512(0, 5);    DECLARE_RESULT_512(1, 5);  
			DECLARE_RESULT_512(0, 6);    DECLARE_RESULT_512(1, 6);  
			DECLARE_RESULT_512(0, 7);    DECLARE_RESULT_512(1, 7);  


		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);
				BROADCAST_LOAD_A_512(x, 1);
				BROADCAST_LOAD_A_512(x, 2);
				BROADCAST_LOAD_A_512(x, 3);

				BROADCAST_LOAD_A_512(x, 4);
				BROADCAST_LOAD_A_512(x, 5);
				BROADCAST_LOAD_A_512(x, 6);
				BROADCAST_LOAD_A_512(x, 7);

				LOAD_B_512(0, x);		LOAD_B_512(1, x);

				MATMUL_512(0, 0);		MATMUL_512(1, 0);
				MATMUL_512(0, 1);		MATMUL_512(1, 1);
				MATMUL_512(0, 2);		MATMUL_512(1, 2);
				MATMUL_512(0, 3);		MATMUL_512(1, 3);
				MATMUL_512(0, 4);		MATMUL_512(1, 4);
				MATMUL_512(0, 5);		MATMUL_512(1, 5);
				MATMUL_512(0, 6);		MATMUL_512(1, 6);
				MATMUL_512(0, 7);		MATMUL_512(1, 7);
			}
			STORE_512(0, 0);		STORE_512(1, 0);
			STORE_512(0, 1);		STORE_512(1, 1);
			STORE_512(0, 2);		STORE_512(1, 2);
			STORE_512(0, 3);		STORE_512(1, 3);
			STORE_512(0, 4);		STORE_512(1, 4);
			STORE_512(0, 5);		STORE_512(1, 5);
			STORE_512(0, 6);		STORE_512(1, 6);
			STORE_512(0, 7);		STORE_512(1, 7);
		}

		j = 16; {
			int k = 0;
			DECLARE_RESULT_512(0, 0);    DECLARE_RESULT_512(1, 0);   
			DECLARE_RESULT_512(0, 1);    DECLARE_RESULT_512(1, 1);  
			DECLARE_RESULT_512(0, 2);    DECLARE_RESULT_512(1, 2);  
			DECLARE_RESULT_512(0, 3);    DECLARE_RESULT_512(1, 3);  
			DECLARE_RESULT_512(0, 4);    DECLARE_RESULT_512(1, 4);   
			DECLARE_RESULT_512(0, 5);    DECLARE_RESULT_512(1, 5);  
			DECLARE_RESULT_512(0, 6);    DECLARE_RESULT_512(1, 6);  
			DECLARE_RESULT_512(0, 7);    DECLARE_RESULT_512(1, 7);  


		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);
				BROADCAST_LOAD_A_512(x, 1);
				BROADCAST_LOAD_A_512(x, 2);
				BROADCAST_LOAD_A_512(x, 3);

				BROADCAST_LOAD_A_512(x, 4);
				BROADCAST_LOAD_A_512(x, 5);
				BROADCAST_LOAD_A_512(x, 6);
				BROADCAST_LOAD_A_512(x, 7);

				LOAD_B_512(0, x);		LOAD_B_512(1, x);

				MATMUL_512(0, 0);		MATMUL_512(1, 0);
				MATMUL_512(0, 1);		MATMUL_512(1, 1);
				MATMUL_512(0, 2);		MATMUL_512(1, 2);
				MATMUL_512(0, 3);		MATMUL_512(1, 3);
				MATMUL_512(0, 4);		MATMUL_512(1, 4);
				MATMUL_512(0, 5);		MATMUL_512(1, 5);
				MATMUL_512(0, 6);		MATMUL_512(1, 6);
				MATMUL_512(0, 7);		MATMUL_512(1, 7);
			}
			STORE_512(0, 0);		STORE_512(1, 0);
			STORE_512(0, 1);		STORE_512(1, 1);
			STORE_512(0, 2);		STORE_512(1, 2);
			STORE_512(0, 3);		STORE_512(1, 3);
			STORE_512(0, 4);		STORE_512(1, 4);
			STORE_512(0, 5);		STORE_512(1, 5);
			STORE_512(0, 6);		STORE_512(1, 6);
			STORE_512(0, 7);		STORE_512(1, 7);
		}

	}

	if (i == N)
		return;

	for(i = n4; i < n2; i+=2) {
		j = 0; {
			int k;
			DECLARE_RESULT_512(0, 0);    DECLARE_RESULT_512(1, 0);    			DECLARE_RESULT_512(2, 0);    DECLARE_RESULT_512(3, 0);    
			DECLARE_RESULT_512(0, 1);    DECLARE_RESULT_512(1, 1);    			DECLARE_RESULT_512(2, 1);    DECLARE_RESULT_512(3, 1);    


		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);
				BROADCAST_LOAD_A_512(x, 1);

				LOAD_B_512(0, x);		LOAD_B_512(1, x);			LOAD_B_512(2, x);		LOAD_B_512(3, x);

				MATMUL_512(0, 0);		MATMUL_512(1, 0);			MATMUL_512(2, 0);		MATMUL_512(3, 0);	
				MATMUL_512(0, 1);		MATMUL_512(1, 1);			MATMUL_512(2, 1);		MATMUL_512(3, 1);			
			}
			STORE_512(0, 0);		STORE_512(1, 0);			STORE_512(2, 0);		STORE_512(3, 0);
			STORE_512(0, 1);		STORE_512(1, 1);			STORE_512(2, 1);		STORE_512(3, 1);
		}
	}

	for(i = n2; i < N; i+=1) {
		j = 0; {
			int k;
			DECLARE_RESULT_512(0, 0);    DECLARE_RESULT_512(1, 0);    			DECLARE_RESULT_512(2, 0);    DECLARE_RESULT_512(3, 0);    


		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);

				LOAD_B_512(0, x);		LOAD_B_512(1, x);			LOAD_B_512(2, x);		LOAD_B_512(3, x);

				MATMUL_512(0, 0);		MATMUL_512(1, 0);			MATMUL_512(2, 0);		MATMUL_512(3, 0);	
			}
			STORE_512(0, 0);		STORE_512(1, 0);			STORE_512(2, 0);		STORE_512(3, 0);
		}
	}
}

void mydgemm(int M, int N, int K, double *A, int strideA, double *B, int strideB , double *R, int strideR)
{
	int i, j;

	int m128 = M & ~31;
	int m64 = M & ~15;
	int m32 = M & ~7;
	int m16 = M & ~3;
        int m8 = M & ~1;

	int n4 = N & ~3;
	int n2 = N & ~1;

	if (M == 32) {
		mydgemm32(N,K,A,strideA,B,strideB,R,strideR);
		return;
	}


	for(i = 0; i < (N & ~3); i+=4) {

		for(j = 0; j < m128; j+= 32) {
			int k = 0;
			DECLARE_RESULT_512(0, 0);    DECLARE_RESULT_512(1, 0);    			DECLARE_RESULT_512(2, 0);    DECLARE_RESULT_512(3, 0);    
			DECLARE_RESULT_512(0, 1);    DECLARE_RESULT_512(1, 1);    			DECLARE_RESULT_512(2, 1);    DECLARE_RESULT_512(3, 1);    
			DECLARE_RESULT_512(0, 2);    DECLARE_RESULT_512(1, 2);    			DECLARE_RESULT_512(2, 2);    DECLARE_RESULT_512(3, 2);    
			DECLARE_RESULT_512(0, 3);    DECLARE_RESULT_512(1, 3);    			DECLARE_RESULT_512(2, 3);    DECLARE_RESULT_512(3, 3);    


		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);
				BROADCAST_LOAD_A_512(x, 1);
				BROADCAST_LOAD_A_512(x, 2);
				BROADCAST_LOAD_A_512(x, 3);

				LOAD_B_512(0, x);		LOAD_B_512(1, x);			LOAD_B_512(2, x);		LOAD_B_512(3, x);
//				PREFETCH_DOWN(4, 0);		PREFETCH_DOWN(4, 0);			PREFETCH_DOWN(4, 0);		PREFETCH_DOWN(4, 0);

				MATMUL_512(0, 0);		MATMUL_512(1, 0);			MATMUL_512(2, 0);		MATMUL_512(3, 0);	
				MATMUL_512(0, 1);		MATMUL_512(1, 1);			MATMUL_512(2, 1);		MATMUL_512(3, 1);			
				MATMUL_512(0, 2);		MATMUL_512(1, 2);			MATMUL_512(2, 2);		MATMUL_512(3, 2);			
				MATMUL_512(0, 3);		MATMUL_512(1, 3);			MATMUL_512(2, 3);		MATMUL_512(3, 3);			
			}
			STORE_512(0, 0);		STORE_512(1, 0);			STORE_512(2, 0);		STORE_512(3, 0);
			STORE_512(0, 1);		STORE_512(1, 1);			STORE_512(2, 1);		STORE_512(3, 1);
			STORE_512(0, 2);		STORE_512(1, 2);			STORE_512(2, 2);		STORE_512(3, 2);
			STORE_512(0, 3);		STORE_512(1, 3);			STORE_512(2, 3);		STORE_512(3, 3);
		}
		if (j == M)
			continue;

		for(j = m128; j < m64; j+= 16) {
			int k;
			DECLARE_RESULT_512(0, 0);    DECLARE_RESULT_512(1, 0);    
			DECLARE_RESULT_512(0, 1);    DECLARE_RESULT_512(1, 1);    
			DECLARE_RESULT_512(0, 2);    DECLARE_RESULT_512(1, 2);    
			DECLARE_RESULT_512(0, 3);    DECLARE_RESULT_512(1, 3);    

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);
				BROADCAST_LOAD_A_512(x, 1);
				BROADCAST_LOAD_A_512(x, 2);
				BROADCAST_LOAD_A_512(x, 3);

				LOAD_B_512(0, x);		LOAD_B_512(1, x);

				MATMUL_512(0, 0);		MATMUL_512(1, 0);		
				MATMUL_512(0, 1);		MATMUL_512(1, 1);		
				MATMUL_512(0, 2);		MATMUL_512(1, 2);		
				MATMUL_512(0, 3);		MATMUL_512(1, 3);		
			}
			STORE_512(0, 0);		STORE_512(1, 0);
			STORE_512(0, 1);		STORE_512(1, 1);
			STORE_512(0, 2);		STORE_512(1, 2);
			STORE_512(0, 3);		STORE_512(1, 3);
		}

		for(j = m64; j < m32; j+= 8) {
			int k;
			DECLARE_RESULT_512(0, 0);
			DECLARE_RESULT_512(0, 1);
			DECLARE_RESULT_512(0, 2);
			DECLARE_RESULT_512(0, 3);

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);
				BROADCAST_LOAD_A_512(x, 1);
				BROADCAST_LOAD_A_512(x, 2);
				BROADCAST_LOAD_A_512(x, 3);

				LOAD_B_512(0, x);

				MATMUL_512(0, 0);
				MATMUL_512(0, 1);
				MATMUL_512(0, 2);
				MATMUL_512(0, 3);
			}
			STORE_512(0, 0);
			STORE_512(0, 1);
			STORE_512(0, 2);
			STORE_512(0, 3);
		}

		for(j = m32; j < m16; j+= 4) {
			int k;
			DECLARE_RESULT_256(0, 0);
			DECLARE_RESULT_256(0, 1);
			DECLARE_RESULT_256(0, 2);
			DECLARE_RESULT_256(0, 3);

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_256(x, 0);
				BROADCAST_LOAD_A_256(x, 1);
				BROADCAST_LOAD_A_256(x, 2);
				BROADCAST_LOAD_A_256(x, 3);

				LOAD_B_256(0, x);

				MATMUL_256(0, 0);
				MATMUL_256(0, 1);
				MATMUL_256(0, 2);
				MATMUL_256(0, 3);
			}
			STORE_256(0, 0);
			STORE_256(0, 1);
			STORE_256(0, 2);
			STORE_256(0, 3);
		}

		for(j = m16; j < m8; j+= 2) {
			int k;
			DECLARE_RESULT_128(0, 0);
			DECLARE_RESULT_128(0, 1);
			DECLARE_RESULT_128(0, 2);
			DECLARE_RESULT_128(0, 3);

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_128(x, 0);
				BROADCAST_LOAD_A_128(x, 1);
				BROADCAST_LOAD_A_128(x, 2);
				BROADCAST_LOAD_A_128(x, 3);

				LOAD_B_128(0, x);

				MATMUL_128(0, 0);
				MATMUL_128(0, 1);
				MATMUL_128(0, 2);
				MATMUL_128(0, 3);
			}
			STORE_128(0, 0);
			STORE_128(0, 1);
			STORE_128(0, 2);
			STORE_128(0, 3);
		}

		for(j = m8; j < M; j++) {
			int k;
			DECLARE_RESULT_SCALAR(x, 0)
			DECLARE_RESULT_SCALAR(x, 1)
			DECLARE_RESULT_SCALAR(x, 2)
			DECLARE_RESULT_SCALAR(x, 3)

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_SCALAR(x, 0);
				BROADCAST_LOAD_A_SCALAR(x, 1);
				BROADCAST_LOAD_A_SCALAR(x, 2);
				BROADCAST_LOAD_A_SCALAR(x, 3);

				LOAD_B_SCALAR(x, x);

				MATMUL_SCALAR(x, 0);
				MATMUL_SCALAR(x, 1);
				MATMUL_SCALAR(x, 2);
				MATMUL_SCALAR(x, 3);
			}
			STORE_SCALAR(x, 0);
			STORE_SCALAR(x, 1);
			STORE_SCALAR(x, 2);
			STORE_SCALAR(x, 3);
		}
	}

	if (i == N)
		return;

	for(i = n4; i < n2; i+=2) {
		for(j = 0; j < m128; j+= 32) {
			int k;
			DECLARE_RESULT_512(0, 0);    DECLARE_RESULT_512(1, 0);    			DECLARE_RESULT_512(2, 0);    DECLARE_RESULT_512(3, 0);    
			DECLARE_RESULT_512(0, 1);    DECLARE_RESULT_512(1, 1);    			DECLARE_RESULT_512(2, 1);    DECLARE_RESULT_512(3, 1);    


		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);
				BROADCAST_LOAD_A_512(x, 1);

				LOAD_B_512(0, x);		LOAD_B_512(1, x);			LOAD_B_512(2, x);		LOAD_B_512(3, x);

				MATMUL_512(0, 0);		MATMUL_512(1, 0);			MATMUL_512(2, 0);		MATMUL_512(3, 0);	
				MATMUL_512(0, 1);		MATMUL_512(1, 1);			MATMUL_512(2, 1);		MATMUL_512(3, 1);			
			}
			STORE_512(0, 0);		STORE_512(1, 0);			STORE_512(2, 0);		STORE_512(3, 0);
			STORE_512(0, 1);		STORE_512(1, 1);			STORE_512(2, 1);		STORE_512(3, 1);
		}
		for(j = m128; j < m64; j+= 16) {
			int k;
			DECLARE_RESULT_512(0, 0);    DECLARE_RESULT_512(1, 0);    
			DECLARE_RESULT_512(0, 1);    DECLARE_RESULT_512(1, 1);    

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);
				BROADCAST_LOAD_A_512(x, 1);

				LOAD_B_512(0, x);		LOAD_B_512(1, x);

				MATMUL_512(0, 0);		MATMUL_512(1, 0);		
				MATMUL_512(0, 1);		MATMUL_512(1, 1);		
			}
			STORE_512(0, 0);		STORE_512(1, 0);
			STORE_512(0, 1);		STORE_512(1, 1);
		}

		for(j = m64; j < m32; j+= 8) {
			int k;
			DECLARE_RESULT_512(0, 0);
			DECLARE_RESULT_512(0, 1);

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);
				BROADCAST_LOAD_A_512(x, 1);

				LOAD_B_512(0, x);

				MATMUL_512(0, 0);
				MATMUL_512(0, 1);
			}
			STORE_512(0, 0);
			STORE_512(0, 1);
		}

		for(j = m32; j < m16; j+= 4) {
			int k;
			DECLARE_RESULT_256(0, 0);
			DECLARE_RESULT_256(0, 1);

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_256(x, 0);
				BROADCAST_LOAD_A_256(x, 1);

				LOAD_B_256(0, x);

				MATMUL_256(0, 0);
				MATMUL_256(0, 1);
			}
			STORE_256(0, 0);
			STORE_256(0, 1);
		}

		for(j = m16; j < m8; j+= 2) {
			int k;
			DECLARE_RESULT_128(0, 0);
			DECLARE_RESULT_128(0, 1);

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_128(x, 0);
				BROADCAST_LOAD_A_128(x, 1);

				LOAD_B_128(0, x);

				MATMUL_128(0, 0);
				MATMUL_128(0, 1);
			}
			STORE_128(0, 0);
			STORE_128(0, 1);
		}

		for(j = m8; j < M; j++) {
			int k;
			DECLARE_RESULT_SCALAR(x, 0)
			DECLARE_RESULT_SCALAR(x, 1)

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_SCALAR(x, 0);
				BROADCAST_LOAD_A_SCALAR(x, 1);

				LOAD_B_SCALAR(x, x);

				MATMUL_SCALAR(x, 0);
				MATMUL_SCALAR(x, 1);
			}
			STORE_SCALAR(x, 0);
			STORE_SCALAR(x, 1);
		}
	}

	for(i = n2; i < N; i+=1) {
		for(j = 0; j < m128; j+= 32) {
			int k;
			DECLARE_RESULT_512(0, 0);    DECLARE_RESULT_512(1, 0);    			DECLARE_RESULT_512(2, 0);    DECLARE_RESULT_512(3, 0);    


		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);

				LOAD_B_512(0, x);		LOAD_B_512(1, x);			LOAD_B_512(2, x);		LOAD_B_512(3, x);

				MATMUL_512(0, 0);		MATMUL_512(1, 0);			MATMUL_512(2, 0);		MATMUL_512(3, 0);	
			}
			STORE_512(0, 0);		STORE_512(1, 0);			STORE_512(2, 0);		STORE_512(3, 0);
		}
		for(j = m128; j < m64; j+= 16) {
			int k;
			DECLARE_RESULT_512(0, 0);    DECLARE_RESULT_512(1, 0);    

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);

				LOAD_B_512(0, x);		LOAD_B_512(1, x);

				MATMUL_512(0, 0);		MATMUL_512(1, 0);		
			}
			STORE_512(0, 0);		STORE_512(1, 0);
		}

		for(j = m64; j < m32; j+= 8) {
			int k;
			DECLARE_RESULT_512(0, 0);

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);

				LOAD_B_512(0, x);

				MATMUL_512(0, 0);
			}
			STORE_512(0, 0);
		}

		for(j = m32; j < m16; j+= 4) {
			int k;
			DECLARE_RESULT_256(0, 0);

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_256(x, 0);

				LOAD_B_256(0, x);

				MATMUL_256(0, 0);
			}
			STORE_256(0, 0);
		}

		for(j = m16; j < m8; j+= 2) {
			int k;
			DECLARE_RESULT_128(0, 0);

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_128(x, 0);

				LOAD_B_128(0, x);

				MATMUL_128(0, 0);
			}
			STORE_128(0, 0);
		}


		for(j = m8; j < M; j++) {
			int k;
			DECLARE_RESULT_SCALAR(x, 0)

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_SCALAR(x, 0);

				LOAD_B_SCALAR(x, x);

				MATMUL_SCALAR(x, 0);
			}
			STORE_SCALAR(x, 0);
		}
	}
}

