#include<iostream> 
#include<algorithm> 
#include<vector> 
#include<cmath> 
#include<chrono>
#include<omp.h>
#include<cfloat>

using namespace std; 

vector<uint8_t> dynamic_quantize_linear(vector<float> x){

    ///steps
    ///get x_min, x_max
    ///OMP 
    
    auto start = chrono::high_resolution_clock::now(); 
    //const auto [min_it, max_it] = std::minmax_element(begin(x), end(x)); 
    
    float x_min = FLT_MAX;
    float x_max = -FLT_MAX; 
 /*   
    for(float xs: x){
        if(xs < x_min) x_min = xs; 
        if(xs > x_max) x_max = xs; 
    }
  */  

    #pragma omp parallel
    {
        float x_min_private = FLT_MAX; 
        float x_max_private = -FLT_MAX; 

        #pragma omp for nowait
        for(float xs: x){
            if(xs < x_min_private) x_min_private = xs; 
            if(xs > x_max_private) x_max_private = xs; 
        }

        #pragma omp critical
        {
            if(x_min_private < x_min) x_min = x_min_private;
            if(x_max_private > x_max) x_max = x_max_private; 
        }

    }
    
    
    auto stop = chrono::high_resolution_clock::now(); 

    auto duration = chrono::duration_cast<chrono::microseconds>(stop-start); 

    cout << "MinMax time: " << duration.count() << " microseconds" << endl; 
    
    //float x_min = min((float)0.0, *min_it); 
    //float x_max = max((float)0.0, *max_it); 
    x_min = min((float)0.0, x_min); 
    x_max = max((float)0.0, x_max);
    
    cout << "min = " << x_min << ", max = " << x_max << "\n"; 
    
    ///Find scale 
    ///y_scale = (x_max- x_min) / (255-0) 
    float y_scale = (x_max-x_min) / (float)(UINT8_MAX-0);
    cout << "y_scale = " << y_scale << endl; 
    
    ///Find zero point
    ///y_zeroPoint = clamp(round((0-x_min)/y_scale), 0, UINT8_MAX); 
    int y_zeroPoint = clamp((int)round((0-x_min)/y_scale), 0, UINT8_MAX); 
    
    cout << "y_zeroPoint = " << y_zeroPoint << endl; 
    
    ///y = clamp(round(x/y_scale) + y_zeroPoint, 0, UINT8_MAX); 
    vector<uint8_t> y(x.size()); 
    
    start = chrono::high_resolution_clock::now(); 
    
    ///this one potentially has false sharing  
    #pragma omp parallel for
    for(int i = 0; i < x.size(); i++){
        y[i] = clamp((int)round(x[i]/y_scale) + y_zeroPoint , 0, UINT8_MAX);
        //cout << omp_get_thread_num() << " working on " << i << endl; 
    }
    stop = chrono::high_resolution_clock::now(); 

    duration = chrono::duration_cast<chrono::microseconds>(stop-start); 

    cout << "omp time " << duration.count() << " microseconds" << endl; 
    return y;  
}


int main(){

    /*
    vector<float> x1 {0, 2, -3, -2.5, 1.34, 0.5};  //scale 0.0196078438 & zero point 153
    vector<float> x2 {-1.0, -2.1, -1.3, -2.5, -3.34, -4.0};  //scale 0.0156862754 & zero point 255

    //insert checks
    vector<uint8_t> y1 = dynamic_quantize_linear(x1); 
    vector<uint8_t> y2 = dynamic_quantize_linear(x2);
    */

    //for(uint8_t y : y1) cout << (int)y << " "; 
    //cout << endl;

    //for(uint8_t y : y2) cout << (int)y << " ";
    //cout << endl;

    cout << "For large array (10000000 elements)" << endl; 
    
    vector<float> x_large(10000000);
    srand(unsigned(time(nullptr)));
    //generate(x_large.begin(), x_large.end(), (float)rand()/(float)rand());   
    for(int i = 0; i<x_large.size(); i++){
        x_large[i] = (float)rand()/(float)rand() - (float)rand()/(float)rand(); 
        //cout << x_large[i] << " ";
    }
    
    //for(int x : x_large) cout << x << " "; 
    //cout << endl; 

    auto start = chrono::high_resolution_clock::now(); 
    vector<uint8_t> y_large = dynamic_quantize_linear(x_large); 
    auto stop = chrono::high_resolution_clock::now(); 

    auto duration = chrono::duration_cast<chrono::microseconds>(stop-start); 

    cout << "Execution time: " << duration.count() << " microseconds" << endl; 
    //for(uint8_t y: y_large) cout << (int)y << " "; 
    //cout << endl; 

    vector<float>().swap(x_large);
    vector<uint8_t>().swap(y_large);

    return 0; 
}
