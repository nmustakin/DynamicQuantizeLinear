# DynamicQuantizeLinear
Implementations of https://github.com/onnx/onnx/blob/main/docs/Operators.md#DynamicQuantizeLinear using C++, OpenMP, and CUDA

To compile simply `make` in the director. 

### Files 
- **dynamic\_quantize\_linear\_basic.cpp** - base unoptimized C++ implementation, uses std::minmax\_elements. 
- **dynamic_quantize_linear_mod.cpp** - manual implementation of minmax\_element. Implemented as a void function by passing in output vector as a reference. 
- **dynamic_quantize_linear_openmp.cpp** - Optimized solution with OpenMP. 
- **dynamic_quantize_linear_omp_mod.cpp** - void function implementation with OpenMP.
- **dynamic_quantize_linear_cuda.cu** - CUDA implementation using thrust library for minmax\_element.   
