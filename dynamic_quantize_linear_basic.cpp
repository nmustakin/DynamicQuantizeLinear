/**********************************************
Base cpp implementation 

Difference with mod.cpp : 

1. This function returns the Y array as 
opposed to getting an already allocated Y 
array passed in by reference

2. std::minmax_element() used to get min and 
max of X array
**********************************************/
#include<iostream> 
#include<algorithm> 
#include<vector> 
#include<cmath> 
#include<chrono> 
#include"test.cpp" 

using namespace std; 


///keeping these performance counters global for now
chrono::high_resolution_clock::time_point start1, start2, start3; 
chrono::high_resolution_clock::time_point stop1, stop2, stop3; 
chrono::microseconds duration1, duration2, duration3;

vector<uint8_t> dynamic_quantize_linear(vector<float> x){

    ///start timer for finding min, max
    start1 = chrono::high_resolution_clock::now(); 
    const auto [min_it, max_it] = std::minmax_element(begin(x), end(x)); 
    //const auto min_it = min_element(x.begin(), x.end());
    //const auto max_it = max_element(x.begin(), x.end());
    
    ///end timer for finding min, max
    stop1 = chrono::high_resolution_clock::now(); 
    duration1 = chrono::duration_cast<chrono::microseconds>(stop1-start1); 

    ///start timer for calculation 
    start2 = chrono::high_resolution_clock::now(); 
    
    ///get x_min, x_max
    float x_min = min((float)0.0, *min_it); 
    float x_max = max((float)0.0, *max_it); 
    cout << "min = " << x_min << ", max = " << x_max << "\n"; 
    
    ///Find scale 
    float y_scale = (x_max-x_min) / (float)(UINT8_MAX-0);
    cout << "y_scale = " << y_scale << endl; 
    
    ///Find zero point
    int y_zeroPoint = clamp((int)round((0-x_min)/y_scale), 0, UINT8_MAX); 
    
    cout << "y_zeroPoint = " << y_zeroPoint << endl; 

    vector<uint8_t> y(x.size()); 

    ///end timer for calculation (includes io and allocation time)
    stop2 = chrono::high_resolution_clock::now(); 

    duration2 = chrono::duration_cast<chrono::microseconds>(stop2-start2);

    ///start time for clamping all elements
    start3 = chrono::high_resolution_clock::now(); 
    for(int i=0; i<x.size(); i++){
        y[i] = clamp((int)round(x[i]/y_scale) + y_zeroPoint, 0, UINT8_MAX); 
    
    }

    ///end timer for clamping     
    stop3 = chrono::high_resolution_clock::now(); 
    duration3 = chrono::duration_cast<chrono::microseconds>(stop3-start3); 

    return y;  
}


int main(){

    vector<float> x1 {0, 2, -3, -2.5, 1.34, 0.5};  //scale 0.0196078438 & zero point 153
    vector<float> x2 {-1.0, -2.1, -1.3, -2.5, -3.34, -4.0};  //scale 0.0156862754 & zero point 255
    vector<float> x3 {1, 2.1, 1.3, 2.5, 3.34, 4.0, 1.5, 2.6, 3.9, 4.0, 3.0, 2.345}; //scale 0.0156862754 & zero point 0
    
    //insert checks
    cout << "--------Test 1--------" << endl;
    vector<uint8_t> y1 = dynamic_quantize_linear(x1); 

    cout << endl << "--------Test 2--------" << endl; 
    vector<uint8_t> y2 = dynamic_quantize_linear(x2);

    cout << endl << "--------Test 3--------" << endl << endl; 
    vector<uint8_t> y3 = dynamic_quantize_linear(x3);

    /* // uncomment to print out outputs
    for(uint8_t y : y1) cout << (int)y << " "; 
    cout << endl;

    for(uint8_t y : y2) cout << (int)y << " ";
    cout << endl;
    
    for(uint8_t y : y3) cout << (int)y << " ";
    cout << endl;
    */

    //***test with randomly initialized input***//
    /* 
    cout << "For large array (100000 elements)" << endl; 
    
    vector<float> x_large(100000);
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
    vector<uint8_t> y_large = dynamic_quantize_linear(x_test); 
    auto stop = chrono::high_resolution_clock::now(); 

    auto duration = chrono::duration_cast<chrono::microseconds>(stop-start); 

    cout << "-------X_Test from test.cpp-------" 
    cout << "MinMax time: " << duration1.count() << " microseconds" << endl; 
    cout << "Calc time: " << duration2.count() << " microseconds" << endl; 
    cout << "Clamp time: " << duration3.count() << " microseconds" << endl; 
    
    cout << "Total execution time: " << duration.count() << " microseconds" << endl; 


    return 0; 
}
