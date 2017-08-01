/**
 * C++ implementation for Kernel Density Map
 */
#ifndef DYNAMIC_KERNEL_DENSITY_MAP
#define DYNAMIC_KERNEL_DENSITY_MAP

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <pthread.h>

#define NUM_THREADS 4

using namespace std;

typedef unsigned char UINT8;

class MyThread
{
public:
    MyThread() {/*empty*/}
    virtual ~MyThread(){/*empty*/}

    /** returns true if thread was started successfully*/
    bool start()
    {
        return (pthread_create(&_thread, NULL, startEntry, this) ==0);
    }

    /** will not return until thread has exited */
    bool join()
    {
        (void) pthread_join(_thread, NULL);
    }

private:
    pthread_t _thread;
    
    static void * startEntry(void *This)
    {
        ((MyThread*)This)->run();
        return NULL;
    }

protected:
    /** real run function that should be overwrite */
    virtual void run() = 0;
};

static double triangular(double z){
    return 1 - abs(z);
}

static double uniform(double z) {
    return abs(z);
}

static double quadratic(double z) {
    return (15 * 1.0 / 16) * (1 - z*z) * (1 - z*z); 
}

static double gaussian(double z) {
    return sqrt(2*M_PI)*exp(-0.5*z*z);
}

/*
 * Map of Kernel Density Estimation 
 *
 */
class KDE: public MyThread
{
public:
    // class constructor/de-constructor
    // KDE(int _n, double* _x_list, double* _y_list, double* _extent, /*double (*_kernel_func)(double), */double _bandwidth, double _cellsize, double _opaque);
    KDE(int _n, 
        vector<double> &_x_list, 
        vector<double> &_y_list, 
        vector<int> &_pts_ids, 
        vector<double> &_extent, 
        double _bandwidth, 
        double _cellsize, 
        int _kernel_func_type,
        int _gradient_type,
        double _opaque);
    ~KDE();
    
    int cols, rows;
    double gradient_min, gradient_max;
    vector<UINT8> r_buffer;
    vector<UINT8> g_buffer;
    vector<UINT8> b_buffer;
    vector<UINT8> a_buffer;
    
private:
    // class members
    double  n;
    int gradient_type;
    vector<double> x_list;
    vector<double> y_list;
    vector<int> pts_ids;
    vector<double> extent;
    double  bandwidth, cellsize, opaque;
    double  (*kernel_func)(double z);

    double left,lower,right,upper;
    double extent_width, extent_height;
    double grid_lower,grid_upper,grid_left,grid_right;
   
    double** grid; 
        
protected:
    void run();
    
public:
    // class functions
    void* update_grid();
    void get_minmax_gradient();
    void create_rgba_buffer(double gradientMin, double gradientMax);
};

class DKDE
{
public:
    //DKDE(double* _x_list, double* _y_list, int _intervals, int* _itv_n_pts, double* _extent, double _bandwidth, double _cellsize, double _opaque);
    DKDE(vector<double> _x_list,
         vector<double> _y_list,
         int _intervals,
         vector<vector<int> > _itv_pts_ids,
         vector<double> _extent,
         double _bandwidth,
         double _cellsize,
         int kernel_func_type,
         int gradient_type,
         double _opaque);
             
    ~DKDE();
    
    int cols, rows;
    double gradient_min, gradient_max;
    vector<vector<UINT8> > r_buffer_array;
    vector<vector<UINT8> > g_buffer_array;
    vector<vector<UINT8> > b_buffer_array;
    vector<vector<UINT8> > a_buffer_array;
};

// 
static UINT8 GRADIENT_CLASSIC[3][256] = {{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 250, 248, 244, 241, 237, 233, 229, 225, 220, 216, 211, 206, 201, 197, 191, 185, 180, 174, 169, 164, 158, 151, 146, 140, 134, 128, 122, 116, 110, 105, 99, 93, 88, 82, 76, 71, 66, 60, 55, 50, 45, 40, 36, 31, 27, 23, 19, 15, 12, 9, 6, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 19, 21, 24, 26, 29, 32, 35, 37, 40, 42, 44, 46, 48, 49, 51, 53},{237, 224, 209, 193, 176, 159, 142, 126, 110, 94, 81, 67, 56, 46, 37, 29, 23, 18, 14, 11, 8, 6, 5, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 6, 10, 14, 18, 22, 26, 31, 36, 41, 45, 51, 57, 62, 68, 74, 81, 86, 93, 99, 105, 111, 118, 124, 131, 137, 144, 150, 156, 163, 169, 175, 181, 187, 192, 198, 203, 208, 213, 218, 222, 227, 232, 235, 238, 242, 245, 247, 250, 251, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 251, 249, 247, 246, 244, 242, 240, 238, 236, 233, 231, 228, 226, 223, 221, 218, 216, 214, 211, 209, 207, 204, 202, 200, 198, 196, 194, 191, 190, 188, 187, 185, 184, 183, 182, 181, 181, 180, 180, 180, 180, 180, 181, 182, 182, 183, 184, 184, 186, 187, 188, 190, 191, 193, 194, 196, 198, 200, 201, 203, 205, 207, 209, 211, 213, 215, 216, 219, 222, 224, 227, 229, 231, 233, 234, 234, 234, 234, 234, 234, 234, 234, 234, 234, 234, 234, 232, 228, 224, 219, 214, 208, 202, 195, 188, 180, 173, 164, 156, 147, 139, 130, 122, 117, 112, 107, 99, 93, 86, 78, 71, 65, 58, 52, 46, 40, 35, 30, 26, 22, 18, 15, 12, 9, 8, 6, 5, 4, 4, 5, 5, 6, 7, 8, 10, 12, 14, 16, 19, 21, 24, 26, 29, 32, 35, 37, 40, 42, 44, 46, 48, 50, 51, 52},{237, 224, 209, 193, 176, 159, 142, 126, 110, 94, 81, 67, 56, 46, 37, 29, 23, 18, 14, 11, 8, 6, 5, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 7, 7, 8, 8, 9, 9, 10, 11, 11, 12, 13, 14, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 30, 31, 33, 34, 35, 37, 39, 40, 43, 44, 46, 48, 51, 53, 55, 57, 60, 62, 65, 68, 70, 74, 77, 80, 84, 88, 92, 95, 99, 104, 108, 112, 116, 120, 125, 129, 134, 138, 143, 147, 151, 156, 160, 165, 171, 178, 184, 190, 197, 203, 209, 214, 220, 225, 230, 234, 238, 242, 246, 248, 251, 254, 255, 255, 255, 255, 255, 254, 252, 250, 247, 244, 240, 236, 232, 228, 222, 218, 213, 208, 205, 203, 199, 196, 193, 189, 184, 180, 175, 171, 167, 162, 157, 152, 147, 142, 136, 131, 126, 120, 115, 110, 106, 101, 97, 92, 89, 85, 82, 79, 77, 77, 77, 76, 74, 73, 72, 71, 69, 70, 69, 68, 67, 67, 65, 65, 64, 63, 62, 61, 61}};
static UINT8 GRADIENT_FIRE[3][256] = {{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 253, 251, 250, 248, 247, 246, 244, 242, 240, 239, 238, 235, 234, 232, 229, 228, 226, 224, 222, 219, 218, 216, 213, 211, 209, 207, 205, 203, 200, 199, 196, 194, 192, 190, 188, 186, 183, 181, 179, 177, 175, 173, 170, 169, 166, 165, 162, 160, 158, 156, 154, 153, 150, 149, 147, 146, 144, 142, 142, 141, 140, 139, 138, 136, 135, 135, 134, 133, 133, 132, 131, 132, 131, 130, 130, 130, 130, 130, 129, 130, 130, 130, 130, 131, 131, 130, 131, 132, 132, 133, 134, 135, 136, 138, 139, 140, 141, 142, 143, 144, 145, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 160, 161, 162, 163, 164, 165, 166, 167, 167, 168, 169, 170, 170, 171, 172, 173, 173, 174, 175, 175, 176, 176, 177, 177},{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 251, 250, 247, 245, 242, 239, 236, 232, 229, 226, 222, 218, 215, 210, 207, 203, 199, 194, 190, 186, 182, 176, 172, 168, 163, 159, 154, 150, 145, 141, 136, 132, 128, 124, 119, 115, 111, 107, 103, 99, 95, 92, 89, 85, 81, 79, 76, 72, 70, 67, 65, 63, 60, 59, 57, 55, 55, 53, 54, 54, 55, 56, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 62, 63, 64, 64, 65, 66, 66, 67, 67, 68, 69, 69, 70, 71, 71, 72, 73, 73, 74, 74, 75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 82, 82, 83, 83, 84, 85, 85, 86, 87, 87, 88, 89, 90, 91, 92, 94, 95, 96, 98, 99, 100, 101, 103, 104, 105, 107, 108, 109, 111, 112, 113, 114, 116, 117, 118, 119, 121, 122, 123, 124, 126, 127, 128, 129, 131, 132, 133, 134, 135, 136, 138, 139, 140, 141, 142, 143, 144, 145, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 160, 161, 162, 163, 164, 165, 166, 167, 167, 168, 169, 170, 170, 171, 172, 173, 173, 174, 175, 175, 176, 176, 177, 177},{255, 253, 250, 247, 244, 241, 238, 234, 231, 227, 223, 219, 214, 211, 206, 202, 197, 192, 187, 183, 178, 172, 167, 163, 157, 152, 147, 142, 136, 132, 126, 121, 116, 111, 106, 102, 97, 91, 87, 82, 78, 74, 70, 65, 61, 57, 53, 50, 46, 43, 39, 38, 34, 31, 29, 26, 25, 23, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46, 47, 47, 48, 48, 49, 50, 50, 51, 51, 52, 53, 53, 54, 54, 55, 56, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 62, 63, 64, 64, 65, 66, 66, 67, 67, 68, 69, 69, 70, 71, 71, 72, 73, 73, 74, 74, 75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 82, 82, 83, 83, 84, 85, 85, 86, 87, 87, 88, 89, 90, 91, 92, 94, 95, 96, 98, 99, 100, 101, 103, 104, 105, 107, 108, 109, 111, 112, 113, 114, 116, 117, 118, 119, 121, 122, 123, 124, 126, 127, 128, 129, 131, 132, 133, 134, 135, 136, 138, 139, 140, 141, 142, 143, 144, 145, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 160, 161, 162, 163, 164, 165, 166, 167, 167, 168, 169, 170, 170, 171, 172, 173, 173, 174, 175, 175, 176, 176, 177, 177}};
static UINT8 GRADIENT_OMG[3][256] = {{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 251, 251, 249, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238, 237, 236, 235, 234, 233, 232, 232, 230, 230, 229, 228, 227, 227, 226, 225, 224, 223, 223, 223, 222, 221, 221, 220, 219, 219, 219, 219, 218, 217, 217, 217, 217, 216, 216, 216, 215, 216, 215, 215, 215, 215, 215, 215},{255, 254, 253, 251, 250, 249, 247, 246, 244, 242, 241, 239, 237, 235, 233, 231, 229, 227, 226, 224, 222, 220, 217, 215, 213, 210, 208, 206, 204, 202, 199, 197, 194, 192, 189, 188, 185, 183, 180, 178, 176, 173, 171, 169, 167, 164, 162, 160, 158, 155, 153, 151, 149, 147, 145, 143, 141, 139, 137, 136, 134, 132, 131, 129, 128, 127, 127, 126, 125, 125, 124, 123, 123, 122, 122, 121, 120, 119, 119, 118, 117, 117, 115, 115, 114, 114, 113, 113, 112, 111, 111, 110, 110, 109, 109, 108, 107, 107, 106, 106, 105, 104, 104, 103, 103, 102, 102, 101, 101, 100, 100, 99, 99, 98, 98, 97, 97, 96, 96, 95, 95, 94, 93, 93, 93, 92, 92, 91, 91, 90, 90, 89, 89, 89, 89, 89, 89, 88, 88, 87, 87, 87, 87, 87, 86, 86, 85, 85, 85, 85, 85, 85, 84, 84, 84, 84, 85, 84, 84, 83, 83, 83, 84, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 84, 83, 84, 84, 84, 83, 84, 83, 84, 85, 85, 85, 85, 85, 85, 86, 86, 87, 89, 91, 92, 94, 96, 97, 99, 100, 102, 104, 105, 107, 109, 110, 112, 113, 115, 117, 118, 120, 121, 123, 124, 126, 127, 129, 130, 132, 133, 135, 136, 138, 139, 140, 142, 143, 144, 146, 147, 148, 150, 151, 152, 153, 154, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 166, 167, 168, 169, 170, 170},{255, 254, 253, 251, 250, 249, 247, 246, 244, 242, 241, 239, 237, 235, 233, 231, 229, 227, 226, 224, 222, 220, 217, 215, 213, 210, 208, 206, 204, 202, 199, 197, 194, 192, 189, 188, 185, 183, 180, 178, 176, 173, 171, 169, 167, 164, 162, 160, 158, 155, 153, 151, 149, 147, 145, 143, 141, 139, 137, 136, 134, 132, 131, 129, 128, 127, 127, 126, 125, 125, 124, 122, 122, 121, 121, 120, 119, 118, 118, 116, 116, 115, 114, 114, 113, 112, 111, 111, 110, 108, 108, 107, 107, 105, 105, 104, 104, 102, 102, 101, 101, 99, 99, 98, 98, 97, 96, 96, 96, 94, 94, 93, 92, 91, 91, 90, 89, 89, 89, 88, 88, 86, 86, 85, 85, 85, 84, 83, 83, 82, 82, 81, 82, 80, 80, 79, 79, 79, 79, 78, 78, 78, 77, 77, 77, 77, 76, 76, 75, 76, 75, 76, 75, 75, 75, 75, 75, 75, 75, 74, 75, 75, 75, 75, 75, 75, 75, 76, 76, 76, 76, 76, 76, 76, 76, 77, 78, 78, 79, 78, 79, 79, 80, 80, 81, 82, 82, 83, 83, 84, 84, 85, 85, 87, 89, 92, 93, 96, 98, 100, 103, 105, 108, 111, 113, 116, 119, 122, 125, 127, 130, 134, 136, 140, 142, 145, 148, 151, 154, 157, 159, 162, 165, 168, 170, 173, 176, 178, 181, 183, 185, 188, 190, 192, 195, 197, 199, 201, 202, 205, 207, 208, 210, 211, 213, 214, 216, 217, 218, 219, 220, 222, 223, 223, 224, 225}};
static UINT8 GRADIENT_PBJ[3][256] = {{41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 45, 46, 46, 47, 47, 47, 48, 48, 49, 49, 49, 50, 50, 51, 51, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 59, 59, 60, 60, 61, 61, 62, 63, 63, 64, 64, 65, 66, 67, 68, 69, 69, 70, 71, 71, 72, 73, 73, 75, 76, 77, 77, 78, 79, 80, 80, 80, 81, 83, 83, 84, 85, 86, 87, 88, 88, 89, 91, 92, 93, 94, 94, 95, 96, 96, 97, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 118, 119, 120, 120, 121, 122, 124, 125, 127, 128, 130, 131, 132, 133, 134, 135, 137, 138, 140, 141, 142, 143, 144, 145, 146, 149, 150, 151, 152, 153, 154, 155, 157, 159, 160, 160, 162, 163, 164, 165, 167, 168, 169, 170, 172, 173, 174, 175, 176, 178, 179, 180, 181, 182, 183, 184, 186, 187, 188, 189, 190, 191, 192, 192, 193, 194, 196, 196, 197, 198, 199, 200, 201, 202, 203, 204, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 214, 215, 215, 216, 217, 218, 219, 220, 221, 222, 222, 223, 224, 225, 226, 227, 227, 228, 229, 229, 230, 231, 231, 232, 233, 234, 234, 235, 236, 237, 237, 238, 239, 239, 240, 240, 241, 241, 242, 243, 243, 244, 244, 245, 246, 246, 247, 247, 248, 249, 249, 250, 250, 250, 250, 251, 251, 252, 252, 253, 253, 253, 254, 254, 255},{10, 10, 10, 10, 10, 10, 9, 9, 9, 9, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 7, 7, 7, 6, 6, 6, 5, 5, 5, 5, 4, 4, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 9, 9, 10, 10, 11, 11, 12, 13, 14, 15, 16, 16, 17, 18, 18, 19, 20, 22, 22, 22, 23, 24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 38, 39, 40, 41, 42, 42, 43, 44, 45, 47, 48, 49, 50, 51, 52, 53, 55, 57, 57, 58, 59, 60, 61, 62, 63, 65, 66, 67, 68, 69, 70, 71, 71, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 83, 84, 85, 86, 87, 87, 89, 90, 90, 90, 91, 92, 93, 93, 94, 96, 96, 97, 98, 99, 99, 100, 100, 100, 102, 103, 103, 104, 105, 105, 106, 107, 107, 108, 109, 109, 110, 111, 111, 112, 113, 113, 114, 114, 115, 116, 116, 117, 117, 118, 119, 119, 120, 120, 121, 121, 122, 122, 123, 124, 124, 125, 125, 126, 126, 127, 127, 128, 128, 128, 129, 129, 130, 130, 131, 131, 131, 132, 132, 133, 133, 133, 134, 134, 135, 135, 135, 136, 136, 136, 136, 137},{89, 89, 89, 89, 88, 88, 88, 88, 88, 88, 89, 88, 88, 88, 88, 88, 88, 87, 87, 87, 87, 87, 87, 86, 86, 86, 86, 86, 85, 85, 85, 85, 85, 84, 84, 84, 84, 84, 84, 84, 84, 83, 83, 83, 83, 82, 82, 82, 82, 82, 82, 82, 81, 81, 81, 80, 80, 80, 79, 80, 80, 79, 79, 79, 78, 78, 78, 77, 77, 76, 76, 76, 75, 75, 74, 74, 73, 73, 73, 73, 72, 72, 71, 71, 70, 70, 69, 70, 69, 69, 68, 67, 67, 66, 66, 66, 65, 64, 64, 63, 62, 61, 61, 60, 60, 59, 58, 58, 57, 56, 55, 54, 54, 54, 53, 52, 51, 50, 49, 48, 48, 47, 46, 46, 45, 44, 43, 42, 42, 41, 41, 40, 39, 38, 38, 37, 36, 36, 35, 34, 33, 33, 32, 31, 30, 30, 29, 29, 28, 27, 26, 26, 25, 25, 24, 24, 23, 23, 23, 22, 22, 21, 21, 21, 20, 20, 19, 19, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25}};
static UINT8 GRADIENT_PJAITCH[3][256] = {{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 251, 250, 249, 248, 248, 247, 246, 245, 244, 243, 241, 240, 239, 238, 236, 236, 235, 233, 232, 230, 229, 228, 227, 226, 224, 222, 221, 220, 219, 217, 215, 214, 212, 211, 209, 208, 206, 204, 203, 202, 200, 198, 197, 195, 193, 192, 190, 189, 187, 186, 184, 182, 181, 179, 177, 175, 174, 172, 170, 169, 167, 165, 164, 162, 161, 159, 157, 154, 153, 151, 150, 148, 147, 145, 143, 141, 140, 139, 137, 135, 134, 132, 130, 129, 127, 126, 124, 122, 121, 120, 118, 116, 115, 113, 112, 110, 108, 107, 106, 104, 102, 101, 99, 99, 97, 96, 94, 92, 92, 90, 89, 87, 86, 85, 84, 83, 81, 80, 80, 78, 77, 75, 75, 74, 73, 71, 71, 70, 69, 68, 67, 66, 65, 64, 64, 63, 62, 61, 60, 59, 59, 59, 58, 57, 56, 56, 55, 55, 55, 55, 54, 53, 53, 52, 52, 52, 52, 52, 52, 51, 51, 51, 51, 51, 51, 51, 51},{254, 254, 253, 253, 253, 252, 252, 252, 251, 251, 251, 250, 250, 250, 249, 249, 249, 248, 248, 248, 247, 247, 246, 246, 246, 245, 245, 245, 244, 244, 243, 243, 242, 242, 241, 241, 240, 240, 239, 239, 238, 238, 237, 237, 236, 237, 236, 235, 235, 234, 233, 233, 232, 231, 230, 230, 229, 228, 227, 226, 225, 225, 224, 223, 222, 222, 221, 220, 219, 218, 216, 215, 214, 213, 212, 211, 209, 208, 208, 206, 205, 204, 202, 201, 199, 199, 199, 198, 197, 196, 195, 195, 194, 193, 192, 192, 191, 190, 189, 188, 187, 187, 186, 185, 184, 183, 182, 181, 181, 180, 179, 178, 177, 176, 176, 175, 173, 172, 171, 170, 170, 169, 167, 166, 166, 165, 163, 162, 161, 161, 159, 158, 157, 157, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 144, 144, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 127, 127, 126, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 113, 113, 112, 111, 110, 109, 108, 107, 105, 104, 104, 103, 102, 101, 100, 99, 98, 96, 96, 96, 94, 93, 92, 91, 90, 90, 89, 87, 86, 86, 85, 84, 83, 82, 82, 80, 80, 79, 78, 78, 76, 75, 75, 74, 74, 73, 72, 71, 71, 69, 69, 68, 67, 67, 66, 65, 65, 64, 63, 63, 62, 62, 61, 61, 60, 60, 59, 58, 57, 57, 57, 56, 56, 56, 55, 54, 53, 53, 52, 53, 53, 52, 52},{165, 164, 163, 162, 161, 160, 159, 157, 156, 155, 153, 152, 150, 149, 148, 146, 145, 143, 141, 139, 138, 136, 134, 132, 130, 129, 127, 125, 123, 121, 119, 117, 114, 112, 111, 109, 107, 105, 102, 100, 99, 97, 95, 92, 90, 89, 87, 84, 82, 80, 79, 77, 74, 72, 70, 69, 67, 65, 63, 61, 60, 58, 56, 54, 52, 51, 49, 47, 46, 44, 43, 42, 41, 39, 39, 37, 36, 34, 33, 33, 32, 30, 29, 29, 28, 28, 27, 27, 27, 27, 26, 26, 26, 26, 26, 25, 26, 26, 25, 25, 25, 25, 25, 25, 26, 26, 25, 25, 26, 26, 25, 25, 26, 26, 25, 25, 26, 26, 25, 25, 26, 26, 25, 25, 26, 26, 26, 26, 25, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 50, 51, 51, 51, 51}};
static UINT8 GRADIENT_RDYIBU[3][256] = {{150, 166, 168, 168, 169, 172, 174, 175, 176, 178, 180, 183, 184, 186, 187, 190, 191, 193, 196, 198, 199, 201, 203, 205, 206, 209, 209, 211, 212, 213, 214, 216, 218, 219, 219, 220, 221, 222, 223, 223, 225, 227, 228, 229, 229, 230, 231, 232, 233, 234, 236, 237, 237, 237, 238, 239, 239, 239, 239, 240, 241, 241, 242, 242, 242, 242, 242, 243, 243, 243, 244, 244, 245, 245, 245, 245, 245, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 246, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 247, 246, 245, 245, 244, 243, 242, 241, 238, 237, 237, 236, 235, 234, 232, 231, 230, 229, 228, 227, 226, 224, 223, 222, 222, 220, 217, 215, 214, 212, 210, 209, 206, 205, 204, 202, 199, 197, 195, 193, 191, 188, 187, 185, 183, 182, 180, 178, 176, 174, 172, 169, 168, 165, 164, 162, 160, 158, 155, 153, 152, 150, 148, 147, 144, 143, 141, 139, 137, 133, 131, 129, 129, 127, 124, 121, 121, 120, 118, 114, 113, 111, 110, 108, 105, 105, 103, 101, 100, 98, 97, 94, 92, 91, 90, 89, 87, 84, 82, 81, 80, 77, 77, 77, 77, 74, 74, 73, 73, 73, 71, 70, 70, 68, 68, 67, 67, 67, 66, 65, 63, 63, 62, 62, 61, 60, 59, 62},{62, 16, 16, 16, 19, 21, 23, 24, 25, 27, 29, 30, 33, 34, 36, 37, 40, 42, 43, 44, 47, 48, 50, 51, 53, 55, 58, 59, 60, 62, 65, 68, 71, 73, 73, 76, 79, 82, 83, 84, 87, 90, 92, 95, 96, 98, 101, 103, 105, 108, 110, 113, 113, 115, 119, 121, 123, 124, 128, 129, 132, 136, 138, 142, 143, 144, 147, 150, 152, 153, 155, 158, 160, 163, 165, 168, 170, 172, 175, 176, 179, 181, 182, 183, 185, 188, 191, 191, 192, 194, 196, 199, 201, 203, 205, 206, 208, 210, 212, 214, 214, 216, 219, 220, 222, 223, 223, 224, 225, 227, 228, 230, 230, 231, 232, 234, 236, 236, 237, 237, 238, 241, 242, 242, 244, 245, 246, 246, 247, 248, 247, 246, 246, 245, 245, 245, 244, 244, 244, 243, 243, 242, 242, 242, 242, 242, 240, 239, 239, 238, 238, 238, 237, 237, 235, 234, 234, 233, 233, 231, 230, 230, 229, 229, 228, 226, 224, 223, 223, 222, 221, 220, 220, 218, 218, 216, 215, 214, 214, 211, 210, 209, 208, 206, 204, 203, 201, 199, 199, 196, 194, 193, 191, 191, 188, 186, 184, 183, 182, 180, 179, 178, 175, 174, 172, 171, 169, 166, 164,161, 160, 157, 155, 153, 152, 151, 148, 145, 144, 142, 140, 137, 136, 134, 133, 130, 128, 125, 123, 121, 119, 117, 114, 112, 110, 108, 106, 103, 101, 99, 97, 93, 91, 90, 87, 84, 82, 81, 78, 76, 73, 71, 68, 67, 65, 67},{82, 52, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 51, 51, 51, 52, 54, 57, 58, 58, 59, 59, 61, 62, 62, 63, 65, 67, 67, 67, 68, 70, 71, 72, 73, 74, 75, 77, 77, 78, 80, 81, 81, 82, 83, 84, 85, 87, 88, 89, 90, 91, 91, 92, 94, 96, 97, 98, 98, 99, 100, 102, 103, 104, 105, 109, 110, 111, 113, 114, 117, 119, 120, 121, 122, 124, 126, 129, 131, 132, 134, 135, 136, 139, 140, 142, 144, 144, 147, 150, 152, 152, 154, 156, 158, 160, 161, 162, 164, 166, 168, 168, 171, 172, 174, 175, 177, 180, 182, 183, 184, 186, 189, 191, 192, 194, 194, 197, 199, 203, 205, 207, 210, 211, 213, 214, 217, 220, 222, 223, 224, 228, 230, 231, 232, 234, 237, 239, 239, 241, 241, 239, 239, 239, 238, 237, 237, 237, 237, 236, 234, 234, 234, 234, 233, 233, 232, 231, 231, 230, 230, 230, 229, 229, 228, 226, 225, 224, 223, 222, 221, 221, 220, 220, 219, 218, 217, 216, 215, 214, 214, 213, 212, 210, 209, 209, 209, 208, 206, 205, 205, 204, 203, 202, 200, 199, 198, 196, 196, 195,194, 194, 192, 191, 190, 189, 187, 187, 186, 185, 183, 183, 181, 180, 179, 176, 176, 175, 175, 173, 172, 172, 171, 169, 168, 168, 165, 165, 163, 162, 161, 160, 160, 158, 158, 156, 154, 153, 152, 152, 151}};
#endif