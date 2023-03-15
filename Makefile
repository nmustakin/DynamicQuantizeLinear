CC = g++ 
NVCC = nvcc
CUDA_HOME = ~/cuda_codes/cuda-samples #set your own cuda home here

dqlbase : dynamic_quantize_linear_basic.cpp
	$(CC) dynamic_quantize_linear_basic.cpp -o dqlbase

dqlmod : dynamic_quantize_linear_mod.cpp
	$(CC) -fopenmp dynamic_quantize_linear_mod.cpp -o dqlmod

dqlomp : dynamic_quantize_linear_openmp.cpp
	$(CC) -fopenmp dynamic_quantize_linear_openmp.cpp -o dqlomp

dqlompmod : dynamic_quantize_linear_omp_mod.cpp
	$(CC) -fopenmp dynamic_quantize_linear_omp_mod.cpp -o dqlompmod

dqlcu : dynamic_quantize_linear_cuda.cu
	$(NVCC) -I $(CUDA_HOME)/Common dynamic_quantize_linear_cuda.cu -o dqlcu 
