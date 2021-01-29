// Week 4
// cuSPARSE vs Kernel Testbench.
// acoustikue@yonsei.ac.kr
// written by SukJoon Oh

#include <cuda_runtime.h>
// #include <device_launch_parameters.h>
#include <cusparse.h>	// cusparseSpMV

#include <stdio.h>
#include <stdlib.h>

#include "mmio.h"
#include "errchk.cuh"
#include "spmv_kernel.cuh"

// Option settings

// #define COO
#define CSR

// #define CUSPARSE
#define SCALAR_KERNEL
// #define VECTOR_KERNEL

// ---- main() ----
// Entry
int main(int argc, char* argv[])
{
	int test_iterations 	= 0;
	
	int N 					= 0;
	int M 					= 0;
	int NZ 					= 0;

	int* host_JR			= NULL;
	int* host_JC			= NULL;
    float* host_AA			= NULL;
	int* host_P				= NULL;

	int* device_JR			= NULL;
	int* device_JC			= NULL;
	float* device_AA		= NULL;
	float* device_AA_sorted	= NULL;
	int* device_P			= NULL;

	void* buffer			= NULL;
	size_t buffer_size		= 0;

	cusparseHandle_t handle = NULL;
	cudaStream_t stream		= NULL;

    if (argc == 1 || argc == 2) { printf("Too few arguments.\nProgram exit.\n"); exit(0); }
    if (argc >= 4) 				{ printf("Too many argmuments.\nProgram exit.\n"); exit(0); }

    test_iterations = atoi(argv[1]);
    printf("(arg1) Target iterations: %d\n", test_iterations);
	printf("(arg2) File name: %s\n", argv[2]);

	//
	// Reading file
	{
		FILE* MTX;
		MTX = fopen(argv[2], "r");
		
		MM_typecode matrix_code;
		
		printf("Reading %s... \n", argv[2]);

		// Read banner, type, etc essential infos
		// Verification steps are ignored.
		if (mm_read_banner(MTX, &matrix_code) != 0) exit(1);
		mm_read_mtx_crd_size(MTX, &M, &N, &NZ); // Over max 1025

		host_JR	    = (int*)malloc(NZ * sizeof(int));
		host_JC	    = (int*)malloc(NZ * sizeof(int));
		host_AA	    = (float*)malloc(NZ * sizeof(float));
		host_P	    = (int*)malloc(NZ * sizeof(int));

		for (register int i = 0; i < NZ; i++)
			fscanf(MTX, "%d %d %f\n", &host_JR[i], &host_JC[i], &host_AA[i]);

		fclose(MTX);
	}


	// ---- Step 1. Load info ----	
	printf("(File info)\tm : %d, n : %d, nz : %d\n", M, N, NZ);
	printf("Printing samples...\n");
	printf("JR: "); for (register int i = 0; i < 10; i++) printf("%6.0d", host_JR[i]); printf("\n");
	printf("JC: "); for (register int i = 0; i < 10; i++) printf("%6.0d", host_JC[i]); printf("\n");
	printf("AA: "); for (register int i = 0; i < 10; i++) printf("%6.0lf", host_AA[i]); printf("\n");
	printf("File successfully loaded.\n");

	// ---- Step 2. Handle create, bind a stream ---- 
	printf("Preparing for cusparseXcoosort...\n");
	CUDA_ERR(cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking));
	CUSPARSE_ERR(cusparseCreate(&handle));
	CUSPARSE_ERR(cusparseSetStream(handle, stream));

	// ---- Step 3. Allocate Buffer ---- 
	CUSPARSE_ERR(cusparseXcoosort_bufferSizeExt(handle, M, N, NZ, device_JR, device_JC, &buffer_size));
	printf("Buffer allocation for cusparseXcoosortByRow : %ld Byte\n", buffer_size);

	CUDA_ERR(cudaMalloc((void**)&device_JR, sizeof(int) * NZ));
	CUDA_ERR(cudaMalloc((void**)&device_JC, sizeof(int) * NZ));
	CUDA_ERR(cudaMalloc((void**)&device_P, sizeof(int) * NZ));
	CUDA_ERR(cudaMalloc((void**)&device_AA, sizeof(float) * NZ));
	CUDA_ERR(cudaMalloc((void**)&device_AA_sorted, sizeof(float) * NZ));
	CUDA_ERR(cudaMalloc((void**)&buffer, sizeof(char) * buffer_size));

	CUDA_ERR(cudaMemcpy(device_JR, host_JR, sizeof(int) * NZ, cudaMemcpyHostToDevice));
	CUDA_ERR(cudaMemcpy(device_JC, host_JC, sizeof(int) * NZ, cudaMemcpyHostToDevice));
	CUDA_ERR(cudaMemcpy(device_AA, host_AA, sizeof(float) * NZ, cudaMemcpyHostToDevice));
	CUDA_ERR(cudaDeviceSynchronize());

	printf("Allocation/Memcopy to GPU done.\n");

	// ---- Step 4. Setup permutation vector P to Identity ---- 
	CUSPARSE_ERR(cusparseCreateIdentityPermutation(handle, NZ, device_P));

	// ---- Step 5. Sort ---- 
	CUSPARSE_ERR(cusparseXcoosortByRow(handle, M, N, NZ, device_JR, device_JC, device_P, buffer));
	printf("cusparseXcoosortByRow done.\n");

	// Gather
	CUSPARSE_ERR(cusparseSgthr(handle, NZ, device_AA, device_AA_sorted, device_P, CUSPARSE_INDEX_BASE_ZERO));
	CUDA_ERR(cudaDeviceSynchronize());

	// Fetch back
	CUDA_ERR(cudaMemcpy(host_JR, device_JR, sizeof(int) * NZ, cudaMemcpyDeviceToHost));
	CUDA_ERR(cudaMemcpy(host_JC, device_JC, sizeof(int) * NZ, cudaMemcpyDeviceToHost));
	CUDA_ERR(cudaMemcpy(host_P, device_P, sizeof(int) * NZ, cudaMemcpyDeviceToHost));
	CUDA_ERR(cudaMemcpy(host_AA, device_AA_sorted, sizeof(float) * NZ, cudaMemcpyDeviceToHost));
    CUDA_ERR(cudaDeviceSynchronize());
    
    // Free memories
    if (device_P) 	cudaFree(device_P);
	if (device_AA) 	cudaFree(device_AA);
	if (buffer) 	cudaFree(buffer);
	if (handle) 	cusparseDestroy(handle);
	if (stream) 	cudaStreamDestroy(stream);

	free(host_P); // Unnecessary

	printf("Printing sorted values...\n");
	printf("JR: "); for (register int i = 0; i < 10; i++) printf("%6.0d", host_JR[i]); printf("\n");
	printf("JC: "); for (register int i = 0; i < 10; i++) printf("%6.0d", host_JC[i]); printf("\n");
	printf("AA: "); for (register int i = 0; i < 10; i++) printf("%6.0lf", host_AA[i]); printf("\n");

#ifdef CSR
	printf("Converting COO to CSR...\n");

	if (device_JR) 	cudaFree(device_JR);
	if (device_JC) 	cudaFree(device_JC);

    int* t_JR	    = (int*)calloc((M + 1), sizeof(int));
	for (int i = 0; i < M + 1; i++) t_JR[i]++; 
	for (int i = 0; i < NZ; i++) 	t_JR[host_JR[i]]++;
	for (int i = 0; i < M; i++)		t_JR[i + 1] += (t_JR[i] - 1);


    free(host_JR);
	host_JR = t_JR; // switch
	
	printf("Done.\n");
	printf("JR: "); for (register int i = 0; i < 10; i++) printf("%6.0d", host_JR[i]); printf("\n");
	printf("JC: "); for (register int i = 0; i < 10; i++) printf("%6.0d", host_JC[i]); printf("\n");
	printf("AA: "); for (register int i = 0; i < 10; i++) printf("%6.0lf", host_AA[i]); printf("\n");

#endif

    {
        handle		            = NULL;
        buffer		            = NULL;
        buffer_size             = 0;

		float average			= 0;
        float elapsed           = 0;
        cudaEvent_t start, stop;

#ifdef CUSPARSE
		printf("Test: CUSPARSE\n");
#else
		printf("Test: KERNEL\n");
#endif
        // ---- Step 7. Define variables
		const float alpha	    = 1;
		const float beta	    = 0;

		float host_y[N]		    = {0, };
        float host_x[M];
        for (auto& elem : host_x) elem = 1; // Set x to have all ones.

		float* device_x		    = NULL;
        float* device_y		    = NULL;
        
        cusparseSpMatDescr_t sp_mtx; // device
        cusparseDnVecDescr_t dn_x, dn_y; // device
        
        CUSPARSE_ERR(cusparseCreate(&handle));

        CUDA_ERR(cudaMalloc((void**)&device_x, sizeof(float) * M));
		CUDA_ERR(cudaMalloc((void**)&device_y, sizeof(float) * N));

		CUDA_ERR(cudaMemcpy(device_x, host_x, sizeof(float) * M, cudaMemcpyHostToDevice));
        CUDA_ERR(cudaMemcpy(device_y, host_y, sizeof(float) * N, cudaMemcpyHostToDevice));
#ifdef CSR
        CUDA_ERR(cudaMalloc((void**)&device_JR, sizeof(int) * (M + 1)));
		CUDA_ERR(cudaMalloc((void**)&device_JC, sizeof(int) * NZ));

		CUDA_ERR(cudaMemcpy(device_JR, host_JR, sizeof(int) * (M + 1), cudaMemcpyHostToDevice));
		CUDA_ERR(cudaMemcpy(device_JC, host_JC, sizeof(int) * NZ, cudaMemcpyHostToDevice));
#endif

#ifdef CUSPARSE
#ifndef CSR // when COO
        CUSPARSE_ERR(cusparseCreateCoo(&sp_mtx, 
                M, N, NZ, device_JR, device_JC, device_AA_sorted,
                CUSPARSE_INDEX_32I, CUSPARSE_INDEX_BASE_ONE, CUDA_R_32F));
#endif
#ifdef CSR
        CUSPARSE_ERR(cusparseCreateCsr(&sp_mtx,
                M, N, NZ, device_JR, device_JC, device_AA_sorted,
                CUSPARSE_INDEX_32I, CUSPARSE_INDEX_32I, CUSPARSE_INDEX_BASE_ONE, CUDA_R_32F));
#endif

        CUSPARSE_ERR(cusparseCreateDnVec(&dn_x, N, device_x, CUDA_R_32F));
        CUSPARSE_ERR(cusparseCreateDnVec(&dn_y, M, device_y, CUDA_R_32F));

#ifndef CSR // when COO
		CUSPARSE_ERR(cusparseSpMV_bufferSize(
			handle, CUSPARSE_OPERATION_NON_TRANSPOSE,
			&alpha, sp_mtx, dn_x, &beta, dn_y, CUDA_R_32F,
			CUSPARSE_COOMV_ALG, &buffer_size));
#else	// when CSR
		CUSPARSE_ERR(cusparseSpMV_bufferSize(
			handle, CUSPARSE_OPERATION_NON_TRANSPOSE,
			&alpha, sp_mtx, dn_x, &beta, dn_y, CUDA_R_32F,
			CUSPARSE_CSRMV_ALG1, &buffer_size));
#endif
		CUDA_ERR(cudaMalloc(&buffer, buffer_size));
#endif

		printf("Iteration start.\n");
        for (register int i = 0; i < test_iterations; i++) {
            cudaEventCreate(&start);
            cudaEventCreate(&stop);
            cudaEventRecord(start); // Timer start

            
		// ---- Step 9. Do SpMV ----
#ifdef CUSPARSE
#ifndef CSR
			CUSPARSE_ERR(cusparseSpMV(handle, CUSPARSE_OPERATION_NON_TRANSPOSE,
				&alpha, sp_mtx, dn_x, &beta, dn_y, CUDA_R_32F,
				CUSPARSE_COOMV_ALG, buffer));
#else
			CUSPARSE_ERR(cusparseSpMV(handle, CUSPARSE_OPERATION_NON_TRANSPOSE,
				&alpha, sp_mtx, dn_x, &beta, dn_y, CUDA_R_32F,
				CUSPARSE_CSRMV_ALG1, buffer));
#endif
#else // Kernel function implementation

#ifdef SCALAR_KERNEL
			// find the minimum block
			int block_num = 1;
			int thread_num = M;

			if (M > 1024) {
				while (block_num * 1024 < M) block_num++;
				thread_num = 1024;
			}
			
			ker_csr_spmv_scalar<<<block_num, thread_num>>>(device_JR, device_JC, device_AA_sorted, device_x, device_y);
#endif
#ifdef VECTOR_KERNEL

			// find the minimum block
			int block_num = 1;
			int thread_num = M * 32;

			if (M > 1024) {
				while (block_num * 1024 < 32 * M) block_num++;
				thread_num = 1024;
			}
			
			ker_csr_spmv_vector<<<block_num, thread_num>>>(device_JR, device_JC, device_AA_sorted, device_x, device_y);

#endif
#endif
            // Record
            cudaEventRecord(stop); // timer end
            cudaEventSynchronize(stop);
            cudaEventElapsedTime(&elapsed, start, stop);
			
			if (i != 0) printf("   Iter %3d, Cached, Elapsed: %fms\n", i + 1, elapsed);
			else printf("   Iter %3d, Elapsed: %fms\n", i + 1, elapsed);
			
			average += elapsed;
			elapsed = 0;
		}		

		printf("Iteration end.\n");
		printf("   Average elapsed time: %lf\n", average / test_iterations);

		// ---- Step 10. Fetch the result ----
		CUDA_ERR(cudaMemcpy(host_y, device_y, N * sizeof(float), cudaMemcpyDeviceToHost));
		printf("Host memory check...\nhost_y: "); for (int i = 0; i < 10; i++) printf("%9.1f", host_y[i]); printf("\n");

		// ---- Step 11. Destroy ----
		CUSPARSE_ERR(cusparseDestroySpMat(sp_mtx));
		CUSPARSE_ERR(cusparseDestroyDnVec(dn_x));
        CUSPARSE_ERR(cusparseDestroyDnVec(dn_y));
        
        // ---- Step 12. Return resources ----
		if (device_JR) 			cudaFree(device_JR);
		if (device_JC) 			cudaFree(device_JC);
		if (device_AA_sorted) 	cudaFree(device_AA_sorted);
		if (device_x) 			cudaFree(device_x);
		if (device_y) 			cudaFree(device_y);
		if (buffer) 			cudaFree(buffer);
		if (handle) 			cusparseDestroy(handle);

        cudaEventDestroy(start);
		cudaEventDestroy(stop);

		
    }

    free(host_JR);
	free(host_JC);
	free(host_AA);

    if (cudaDeviceReset() != cudaSuccess) {
		fprintf(stderr, "cudaDeviceReset failed!");
		return 1;
	}

    return 0;
}
