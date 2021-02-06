// 
// CSR scalar kernel function
__global__ void ker_csr_spmv_scalar(
	const int* argJR, const int* argJC, const float* argAA,
	const float* arg_x, float* arg_y) {

	int idx		= blockDim.x * blockIdx.x + threadIdx.x;
	float sum	= 0;

	for (int i = argJR[idx] - 1; i < argJR[idx + 1] - 1; i++)
		sum		+= (argAA[i] * arg_x[argJC[i] - 1]);

	arg_y[idx]	+= sum;
};


//
// CSR vector kernel function
__global__ void ker_csr_spmv_vector(
	const int* argJR, const int* argJC, const float* argAA,
	const float* arg_x, float* arg_y) {

	// Thread : 32 * M
	int tid		= blockDim.x * blockIdx.x + threadIdx.x;
	int wid		= tid / 32;
	int lidx	= tid & 31;
	float sum	= 0;

	for (int i = argJR[wid] - 1 + lidx; i < argJR[wid + 1] - 1; i += 32)
		sum += argAA[i] * arg_x[argJC[i] - 1];

	for (int i = 16; i > 0; i /= 2)
		sum += __shfl_down_sync(0xFFFFFFFF, sum, i);

	if (lidx == 0) arg_y[wid] = sum;
};

//
// CSR vector kernel function (shared)
__global__ void ker_csr_spmv_vector_shared(
	const int* argJR, const int* argJC, const float* argAA,
	const float* arg_x, float* arg_y) {

	__shared__ float shared_y[1024]; // fixed

	int tid		= blockDim.x * blockIdx.x + threadIdx.x;
	int wid		= tid / 32;
	int lidx	= tid & 31;
	float sum	= 0;

	for (int i = argJR[wid] - 1 + lidx; i < argJR[wid + 1] - 1; i += 32)
		shared_y[threadIdx.x] += argAA[i] * arg_x[argJC[i] - 1];

	// Parallel reduction sum
	if (lidx < 16) shared_y[threadIdx.x] += shared_y[threadIdx.x + 16];
	if (lidx < 8) shared_y[threadIdx.x] += shared_y[threadIdx.x + 8];
	if (lidx < 4) shared_y[threadIdx.x] += shared_y[threadIdx.x + 4];
	if (lidx < 2) shared_y[threadIdx.x] += shared_y[threadIdx.x + 2];
	if (lidx < 1) shared_y[threadIdx.x] += shared_y[threadIdx.x + 1];

	// First thread writes the result
	if (lidx == 0) arg_y[wid] += shared_y[threadIdx.x];
};
