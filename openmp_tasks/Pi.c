#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int id;

double calculate_pi(const size_t N){
    double pi=0.0;
    #pragma omp parallel reduction (+: pi) private(id) num_threads(6)
    {
    id = omp_get_thread_num();
    #pragma omp for 
    for (int i=0; i<N;i++){
        double x = ((double)rand_r(&id))/RAND_MAX;
        double y = ((double)rand_r(&id))/RAND_MAX;
        if ((x*x + y*y)<=1.){
            pi++;
        }
    }
    }
    return 4*pi/N;

}

int main()
{
    const size_t N = 100000;
    double step;

    double x, pi, sum = 0.;

    double start = omp_get_wtime();
    pi = calculate_pi(N);
    double end = omp_get_wtime();
    printf("Time: %f seconds.\n", end-start);
    printf("pi = %.16f\n", pi);

    return 0;
}
