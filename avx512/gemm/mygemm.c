#include <math.h>
#include <stdint.h>
#include <immintrin.h>
#include <assert.h>
#include <stdio.h>


#define DECLARE_RESULT_512(N,M) __m512 result##N##M = _mm512_setzero_ps()
#define BROADCAST_LOAD_A_512(N,M) __m512 Aval##M = _mm512_broadcastss_ps(_mm_load_ss(&A[k  + strideA * (i+M)]))
#define LOAD_B_512(N,M)  __m512 Bval##N = _mm512_loadu_ps(&B[strideB * k + j + (N*16)])
#define MATMUL_512(N,M)  result##N##M = _mm512_fmadd_ps(Aval##M, Bval##N , result##N##M)
#define STORE_512(N,M) _mm512_storeu_ps(&R[(i+M) * strideR + j+(N*16)], result##N##M)


#define DECLARE_RESULT_256(N,M) __m256 result##N##M = _mm256_setzero_ps()
#define BROADCAST_LOAD_A_256(N,M) __m256 Aval##M = _mm256_broadcastss_ps(_mm_load_ss(&A[k  + strideA * (i+M)]))
#define LOAD_B_256(N,M)  __m256 Bval##N = _mm256_loadu_ps(&B[strideB * k + j + (N*8)])
#define MATMUL_256(N,M)  result##N##M = _mm256_fmadd_ps(Aval##M, Bval##N , result##N##M)
#define STORE_256(N,M) _mm256_storeu_ps(&R[(i+M) * strideR + j+(N*8)], result##N##M)

#define DECLARE_RESULT_128(N,M) __m128 result##N##M = _mm_setzero_ps()
#define BROADCAST_LOAD_A_128(N,M) __m128 Aval##M = _mm_broadcastss_ps(_mm_load_ss(&A[k  + strideA * (i+M)]))
#define LOAD_B_128(N,M)  __m128 Bval##N = _mm_loadu_ps(&B[strideB * k + j + (N*4)])
#define MATMUL_128(N,M)  result##N##M = _mm_fmadd_ps(Aval##M, Bval##N , result##N##M)
#define STORE_128(N,M) _mm_storeu_ps(&R[(i+M) * strideR + j+(N*4)], result##N##M)

#define DECLARE_RESULT_SCALAR(N,M) float result##M = 0;
#define BROADCAST_LOAD_A_SCALAR(N,M) float Aval##M = A[k + strideA * (i + M)];
#define LOAD_B_SCALAR(N,M)  float Bval  = B[k * strideB + j];
#define MATMUL_SCALAR(N,M) result##M +=  Aval##M * Bval;
#define STORE_SCALAR(N,M)  R[(i+M) * strideR + j] = result##M;

#define PREFETCH_DOWN(N,M)  __builtin_prefetch(&B[strideB * (k+M) + (j+16 * N)])


void mygemm64x64(int K, float *A, int strideA, float *B, int strideB , float *R, int strideR)
{
	int i, j, k;

	for(i = 0; i < 64; i+=4) {
			j = 0;
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
//				PREFETCH_DOWN(0, 32);		PREFETCH_DOWN(1, 32);			PREFETCH_DOWN(2, 32);		PREFETCH_DOWN(3, 32);

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
}

void mygemm(int M, int N, int K, float *A, int strideA, float *B, int strideB , float *R, int strideR)
{
	int i, j, k;

	int m128 = M & ~63;
	int m64 = M & ~31;
	int m32 = M & ~15;
	int m16 = M & ~7;
        int m8 = M & ~3;

	int n4 = N & ~3;
	int n2 = N & ~1;

	if (M == 64 && N == 64)
		return mygemm64x64(K,A,strideA,B,strideB,R,strideR);


//	m64 = 0;
//	m32 = 0;
//	m16 = 0;

//	n2 = 0;
//	n4 = 0;

	for(i = 0; i < n4; i+=4) {

		for(j = 0; j < m128; j+= 64) {
			k = 0;
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

		for(j = m128; j < m64; j+= 32) {
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

		for(j = m64; j < m32; j+= 16) {
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

		for(j = m32; j < m16; j+= 8) {
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

		for(j = m16; j < m8; j+= 4) {
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

	for(i = n4; i < n2; i+=2) {
		for(j = 0; j < m128; j+= 64) {
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
		for(j = m128; j < m64; j+= 32) {
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

		for(j = m64; j < m32; j+= 16) {
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

		for(j = m32; j < m16; j+= 8) {
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

		for(j = m16; j < m8; j+= 4) {
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
		for(j = 0; j < m128; j+= 64) {
			DECLARE_RESULT_512(0, 0);    DECLARE_RESULT_512(1, 0);    			DECLARE_RESULT_512(2, 0);    DECLARE_RESULT_512(3, 0);    


		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);

				LOAD_B_512(0, x);		LOAD_B_512(1, x);			LOAD_B_512(2, x);		LOAD_B_512(3, x);

				MATMUL_512(0, 0);		MATMUL_512(1, 0);			MATMUL_512(2, 0);		MATMUL_512(3, 0);	
			}
			STORE_512(0, 0);		STORE_512(1, 0);			STORE_512(2, 0);		STORE_512(3, 0);
		}
		for(j = m128; j < m64; j+= 32) {
			DECLARE_RESULT_512(0, 0);    DECLARE_RESULT_512(1, 0);    

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);

				LOAD_B_512(0, x);		LOAD_B_512(1, x);

				MATMUL_512(0, 0);		MATMUL_512(1, 0);		
			}
			STORE_512(0, 0);		STORE_512(1, 0);
		}

		for(j = m64; j < m32; j+= 16) {
			DECLARE_RESULT_512(0, 0);

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_512(x, 0);

				LOAD_B_512(0, x);

				MATMUL_512(0, 0);
			}
			STORE_512(0, 0);
		}

		for(j = m32; j < m16; j+= 8) {
			DECLARE_RESULT_256(0, 0);

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_256(x, 0);

				LOAD_B_256(0, x);

				MATMUL_256(0, 0);
			}
			STORE_256(0, 0);
		}

		for(j = m16; j < m8; j+= 4) {
			DECLARE_RESULT_128(0, 0);

		   	for(k = 0; k < K; k++) {
				BROADCAST_LOAD_A_128(x, 0);

				LOAD_B_128(0, x);

				MATMUL_128(0, 0);
			}
			STORE_128(0, 0);
		}


		for(j = m8; j < M; j++) {
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

