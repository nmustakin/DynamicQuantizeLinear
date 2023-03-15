/**********************************************
Includes openmp implementation for finding 
min, max values of X array and calculation
of Y array elements

Difference with openmp.cpp : 

This function modifies the Y array which has 
been passed in as a reference as opposed to 
allocating with output array within the 
function and returning it
**********************************************/

#include<iostream> 
#include<algorithm> 
#include<vector> 
#include<cmath> 
#include<chrono>
#include<omp.h>
#include<cfloat>
#include"test.cpp" 

using namespace std; 

///keeping these performance counters global for now
chrono::high_resolution_clock::time_point start1, start2, start3; 
chrono::high_resolution_clock::time_point stop1, stop2, stop3; 
chrono::microseconds duration1, duration2, duration3;

void dynamic_quantize_linear(vector<float> x, vector<uint8_t>& y){

        
    ///start timer for finding min, max
    start1 = chrono::high_resolution_clock::now(); 

    ///get x_min, x_max
    ///OMP 
    float x_min = FLT_MAX;
    float x_max = -FLT_MAX; 

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
    
    ///end timer for finding min, max
    stop1 = chrono::high_resolution_clock::now(); 
    duration1 = chrono::duration_cast<chrono::microseconds>(stop1-start1); 

    ///start timer for calculation 
    start2 = chrono::high_resolution_clock::now(); 
    x_min = min((float)0.0, x_min); 
    x_max = max((float)0.0, x_max);
    
    ///Find scale 
    float y_scale = (x_max-x_min) / (float)(UINT8_MAX-0);
    cout << "y_scale = " << y_scale << endl; 
    
    ///Find zero point
    int y_zeroPoint = clamp((int)round((0-x_min)/y_scale), 0, UINT8_MAX); 
    
    cout << "y_zeroPoint = " << y_zeroPoint << endl; 
  
    ///end timer for calculation (includes io time)
    stop2 = chrono::high_resolution_clock::now(); 

    duration2 = chrono::duration_cast<chrono::microseconds>(stop2-start2);

    ///start time for clamping all elements
    start3 = chrono::high_resolution_clock::now(); 
  
    ///this potentially has false sharing  
    #pragma omp parallel for
    for(int i = 0; i < x.size(); i++){
        y[i] = clamp((int)round(x[i]/y_scale) + y_zeroPoint , 0, UINT8_MAX);
    }
    stop3 = chrono::high_resolution_clock::now(); 

    duration3 = chrono::duration_cast<chrono::microseconds>(stop3-start3); 
  
}


int main(){

    vector<float> x1 {0, 2, -3, -2.5, 1.34, 0.5};  //scale 0.0196078438 & zero point 153
    vector<float> x2 {-1.0, -2.1, -1.3, -2.5, -3.34, -4.0};  //scale 0.0156862754 & zero point 255
    vector<float> x3 {1, 2.1, 1.3, 2.5, 3.34, 4.0, 1.5, 2.6, 3.9, 4.0, 3.0, 2.345};

    vector<uint8_t> y1(x1.size()); 
    vector<uint8_t> y2(x2.size()); 
    vector<uint8_t> y3(x3.size());
    //insert checks
    cout << "--------Test 1--------" << endl;
    dynamic_quantize_linear(x1, y1); 

    cout << endl << "--------Test 2--------" << endl; 
    dynamic_quantize_linear(x2, y2);

    cout << endl << "--------Test 3--------" << endl << endl; 
    dynamic_quantize_linear(x3, y3); 

    /* // uncomment to print out outputs
    for(uint8_t y : y1) cout << (int)y << " "; 
    cout << endl;

    for(uint8_t y : y2) cout << (int)y << " ";
    cout << endl;

    for(uint8_t y : y3) cout << (int)y << " ";
    cout << endl << endl;
    */

    //***test with randomly initialized input***//
    /*
    cout << "For large array (10000000 elements)" << endl; 
    
    vector<float> x_large(10000000);
    srand(unsigned(time(nullptr)));
    //generate(x_large.begin(), x_large.end(), (float)rand()/(float)rand());   
    for(int i = 0; i<x_large.size(); i++){
        x_large[i] = (float)rand()/(float)rand() - (float)rand()/(float)rand(); 
        //cout << x_large[i] << " ";
    }
    */
    
    vector<float> x_test(XTestLength);    
    vector<uint8_t> y_test(XTestLength);

    for(int i =0; i< XTestLength; i++) x_test[i] = XTest[i]; 

    auto start = chrono::high_resolution_clock::now(); 
    dynamic_quantize_linear(x_test, y_test); 
    auto stop = chrono::high_resolution_clock::now(); 

    auto duration = chrono::duration_cast<chrono::microseconds>(stop-start); 

    cout << "-------X_Test from test.cpp-------" 
    cout << "MinMax time: " << duration1.count() << " microseconds" << endl; 
    cout << "Calc time: " << duration2.count() << " microseconds" << endl; 
    cout << "Clamp time: " << duration3.count() << " microseconds" << endl; 
    cout << "Total Execution time: " << duration.count() << " microseconds" << endl; 
    

    return 0; 
}
