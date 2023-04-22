#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
using namespace std;
#include <omp.h>

void CreateMatrix(double * A, double* x_true, double* b, double* x, int N){
    mt19937 gen;
    random_device rand_device;
    gen.seed(rand_device());
    uniform_real_distribution<double> dist(-10.0, 10.0);
    for (int i = 0; i < N; ++i) {
        x_true[i] = i; 
    }
    for (int i = 0; i < N; ++i) {
        double sum = 0;
        for (int j = 0; j < N; ++j) {
        A[i * N + j] = dist(gen);
        sum += 2 * abs(A[i * N + j]);  // making matrix diagonally dominant
        }
        A[i * N + i] += sum;
    }


    for(int i =0; i< N ; i++){
        for(int k=0; k < N; k++){
            b[i] += A[i * N + k] * x_true[k];
        }
    }


    for (int i =0; i<N; i++){
        x[i] = dist(gen);
    }
    
}

void Solver(double *A, double* b, double* x, int iter, int N){
    double sum(0);
    for (int k=0; k<iter; k++){
        #pragma omp parallel for private(sum) shared(x, A) num_threads(7)
        for (int i = 0; i<N; i++){
            sum=0;
            for (int j=0;j<N; j++){
                if (j!=i){
                    sum+= A[N*i+j]*x[j];
                }
                
            }
            x[i] = (b[i]-sum)/A[N*i+i];

        }
    }


}

double CheckMSE(double *x, double *x_true, int N){
    double mse = 0;
    for (int i = 0; i < N; ++i) {
        mse += (x[i] - x_true[i]) * (x[i] - x_true[i]);
    }
    mse /= N;
    return mse;
}

int main(){
    cout << "matrix size"<< endl;
    size_t N(1000);
  //  cin >> N;
    double * A, *x_true, *b, *x;
    A = (double*)malloc(N*N*sizeof(double));
    x_true = (double*)malloc(N*sizeof(double));
    b = (double*)malloc(N*sizeof(double));
    x = (double*)malloc(N*sizeof(double));
    int iter = 10;
    CreateMatrix(A, x_true, b, x, N);
    double start, end, duration;
    start = omp_get_wtime();
    Solver(A, b, x, iter, N);
    end = omp_get_wtime();

    duration = (end - start);
    double mse = CheckMSE(x, x_true, N);
    cout << "Time elapsed: "<< (float)duration << " seconds with MSE: " << mse << endl;
    free(A);
    free(x_true);
    free(b);
    free(x);
    return 0;
}