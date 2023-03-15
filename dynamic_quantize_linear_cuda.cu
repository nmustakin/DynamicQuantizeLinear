#include<iostream> 
#include<helper_cuda.h> 
#include<thrust/pair.h>
#include<thrust/device_vector.h>
#include<thrust/extrema.h>
#include<chrono> 
#include<cfloat> 

using namespace std; 

int clamp(int x, int a, int b){
    return max(a, min(b,x)); 
}
__device__ int dev_clamp(int x, int a, int b){
    return max(a, min(b,x)); 
}

__global__ void clamp_all(int N, float* d_x, uint8_t* d_y, float y_scale, int y_zeroPoint){
    int i = threadIdx.x + blockIdx.x * blockDim.x; 
    
    if(i < N) d_y[i] = dev_clamp(llrintf(d_x[i]/y_scale)+y_zeroPoint, 0, 255);  
        
}

void dynamic_quantize_linear(int N, float* d_x, uint8_t* d_y){

    float x_min, x_max;
    
    auto start = chrono::high_resolution_clock::now(); 
    thrust::pair<thrust::device_ptr<float>, thrust::device_ptr<float>> tuple;
    tuple = thrust::minmax_element(thrust::device_pointer_cast(d_x), thrust::device_pointer_cast(d_x) + N);
    
    auto stop = chrono::high_resolution_clock::now(); 
    auto duration = chrono::duration_cast<chrono::microseconds>(stop-start); 

    cout << "MinMax time: " << duration.count() << " microseconds" << endl; 
    
    x_min = tuple.first[0];
    x_max = tuple.second[0];

    cout << "minelement " << x_min <<" - maxelement " << x_max << endl;
    
    float y_scale = (x_max-x_min) / (float)(UINT8_MAX-0);
    cout << "y_scale = " << y_scale << endl; 
            
    ///Find zero point
    ///y_zeroPoint = clamp(round((0-x_min)/y_scale), 0, UINT8_MAX); 
    int y_zeroPoint = clamp((int)round((0-x_min)/y_scale), 0, UINT8_MAX); 
                
    cout << "y_zeroPoint = " << y_zeroPoint << endl; 

    unsigned int TB_size = 1024; 
    start = chrono::high_resolution_clock::now(); 

    clamp_all<<<ceil((double)N/(double)TB_size), TB_size>>>(N, d_x, d_y, y_scale, y_zeroPoint);
    stop = chrono::high_resolution_clock::now(); 

    duration = chrono::duration_cast<chrono::microseconds>(stop-start); 

    cout << "Clamp time: " << duration.count() << " microseconds" << endl; 
}



int main(){

    const int N = 10000000;
    
    cout << "For large array (10000000 elements)" << endl; 
         
    float *h_x = (float*) malloc(N*sizeof(float)); 
   
    srand(unsigned(time(nullptr)));
    //generate(x_large.begin(), x_large.end(), (float)rand()/(float)rand());   
    for(int i = 0; i<N; i++){
        h_x[i] = (float)rand()/(float)rand() - (float)rand()/(float)rand(); 
        //cout << x_large[i] << " ";
    }

    float *d_x;

    uint8_t *h_y, *d_y;  
    auto start = chrono::high_resolution_clock::now(); 
     
    cudaMalloc(&d_x, N*sizeof(float));
    cudaMemcpy(d_x, h_x, N*sizeof(float), cudaMemcpyHostToDevice); 
    auto stop = chrono::high_resolution_clock::now(); 

    auto duration = chrono::duration_cast<chrono::microseconds>(stop-start); 

    cout << "Memcpy host to device time: " << duration.count() << " microseconds" << endl; 
    
    h_y = (uint8_t*) malloc(N*sizeof(uint8_t)); 
    cudaMalloc(&d_y, N*sizeof(uint8_t));

    dynamic_quantize_linear(N, d_x, d_y);

    start = chrono::high_resolution_clock::now();

    cudaMemcpy(h_y, d_y, N*sizeof(uint8_t), cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    stop = chrono::high_resolution_clock::now();

    duration = chrono::duration_cast<chrono::microseconds>(stop-start);    

    cout << "Memcpy device to host time: " << duration.count() << " microseconds" << endl;        


    return 0;
}
