#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
using namespace std;
#include <omp.h>



void create_data(double  * x, double* y, double  * y_true, int N, double  a, double  b){
    mt19937 gen;
    random_device rand_device;
    gen.seed(rand_device());
    uniform_real_distribution <double> dist_x (0.0, 1000.0);
    uniform_real_distribution<double > dist(-5.0, 5.0);
    for (int i = 0; i<N; i++){
        x[i] = dist_x(gen);
        y_true[i] = a*x[i] + b+ dist(gen);
        y[i] = 0;
    }

}

void Solver(double *x, double *y, int  N, int iter,
                                 double lr, double *y_true, double  * a_found,double  *b_found){
    double   a(0), b(0), grad_a(0), grad_b(0);
    for (int i=0; i<iter; i++){
        grad_a =0;
        grad_b = 0;
        #pragma omp parallel for reduction(+: grad_a, grad_b) num_threads(7)
        for (int j=0; j<N;j++){
            auto d = y_true[j]-a*x[j]-b;
            grad_a += lr*2*d*x[j]/N;
            grad_b += lr*2*d/N;
        }
        a+=grad_a;
        b+=grad_b;

    }
    *a_found = a;
    *b_found = b;
}



double  CheckMSE(double  *x, double  *x_true, int N){
    double  mse = 0;
    for (int i = 0; i < N; ++i) {
        mse += (x[i] - x_true[i]) * (x[i] - x_true[i]);
    }
    mse /= N;
    return mse;
}




int main(){
    int N(100000); //vector size
    double  *x, *y, *y_found, *y_true;
    x = (double *)malloc(N*sizeof(double ));
    y_found = (double *)malloc(N*sizeof(double ));
    y_true = (double *)malloc(N*sizeof(double ));
    y = (double*)malloc(N*sizeof(double));
    double  a(3.67), b(5.1983);
    create_data(x,y, y_true, N, a, b);
    double  start, end, duration;
    double  lr(0.0000001), a_found, b_found;
    int iter(1000);
    start = omp_get_wtime();
   Solver(x,y,  N, iter, lr, y_true, &a_found, &b_found);
    end = omp_get_wtime();

     duration = end - start;
    cout << a_found << " " << b_found << endl;
    for (int i =0; i<N;i++){
        y_found[i] = a_found*x[i]+b_found;
    }
    
    double   mse(0);
   mse = CheckMSE(y_true, y_found, N);
    cout << "Time elapsed: "<< duration << " seconds with mse: " << mse << endl;
    free(x);
    free(y_found);
    free(y_true);
    free(y);

    return 0;
}
