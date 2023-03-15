#include<iostream> 
#include<algorithm> 
#include<vector> 
#include<cmath> 
#include<chrono> 
#include<cfloat>

using namespace std; 

chrono::high_resolution_clock::time_point start1, start2, start3; 
chrono::high_resolution_clock::time_point stop1, stop2, stop3; 
chrono::microseconds duration1, duration2, duration3;

void dynamic_quantize_linear(vector<float> x, vector<uint8_t>& y){

    ///steps
    ///get x_min, x_max
    ///start timer for finding min, max
    start1 = chrono::high_resolution_clock::now(); 
    
    float x_min = FLT_MAX;
    float x_max = -FLT_MAX; 

    for(float xs: x){
        if(xs < x_min) x_min = xs; 
        if(xs > x_max) x_max = xs; 
    }

    ///end timer for finding min, max
    stop1 = chrono::high_resolution_clock::now(); 
    duration1 = chrono::duration_cast<chrono::microseconds>(stop1-start1); 
    
    
    ///start timer for calculation 
    start2 = chrono::high_resolution_clock::now(); 
    x_min = min((float)0.0, x_min); 
    x_max = max((float)0.0, x_max);

    ///Find scale 
    ///y_scale = (x_max- x_min) / (255-0) 
    float y_scale = (x_max-x_min) / (float)(UINT8_MAX-0);
    cout << "y_scale = " << y_scale << endl; 
    
    ///Find zero point
    ///y_zeroPoint = clamp(round((0-x_min)/y_scale), 0, UINT8_MAX); 
    int y_zeroPoint = clamp((int)round((0-x_min)/y_scale), 0, UINT8_MAX); 
    
    cout << "y_zeroPoint = " << y_zeroPoint << endl;

    ///end timer for calculation (includes io time)
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

}


int main(int argc, char* argv[]){

    
    vector<float> x1 {0, 2, -3, -2.5, 1.34, 0.5};  //scale 0.0196078438 & zero point 153
    vector<float> x2 {-1.0, -2.1, -1.3, -2.5, -3.34, -4.0};  //scale 0.0156862754 & zero point 255
    vector<float> x3 {1, 2.1, 1.3, 2.5, 3.34, 4.0, 1.5, 2.6, 3.9, 4.0, 3.0, 2.345};
    
    vector<uint8_t> y1(x1.size()); 
    vector<uint8_t> y2(x2.size()); 
    vector<uint8_t> y3(x3.size());
    //insert checks
    dynamic_quantize_linear(x1, y1); 
    dynamic_quantize_linear(x2, y2);
    dynamic_quantize_linear(x3, y3);
    
    /*
    for(uint8_t y : y1) cout << (int)y << " "; 
    cout << endl;

    for(uint8_t y : y2) cout << (int)y << " ";
    cout << endl << endl;
    */

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
    vector<uint8_t> y_large(x_large.size());
    auto start = chrono::high_resolution_clock::now(); 
    
    dynamic_quantize_linear(x_large, y_large); 
    auto stop = chrono::high_resolution_clock::now(); 

    auto duration = chrono::duration_cast<chrono::microseconds>(stop-start); 

    cout << "Execution time: " << duration.count() << " microseconds" << endl; 

    cout << "MinMax time: " << duration1.count() << " microseconds" << endl; 
    cout << "Calc time: " << duration2.count() << " microseconds" << endl; 
    cout << "Clamp time: " << duration3.count() << " microseconds" << endl; 
    


    return 0; 
}
