#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <mpi.h>
#include <stdexcept>
#include <vector>
#include <random>
using namespace std;


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    mt19937 gen;
    random_device rand_device;
    gen.seed(rand_device());
    uniform_int_distribution<> distr(0, 1);
    MPI_Status status;
    int prank;
    int psize;
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &prank);
    //rule
    int rule_from [10] = { 1, 1, 1, 0, 0, 0, 1, 0, 1, 1 };
    int N(1000), right, left;

    int boundary = 1;
    int rule_to[8] = {0};
    if (prank==0){
    cout << "size: " << endl;

    cin >> N;
    cout << "boundary conditions (1 - periodic, 0 - constant): "<< endl;
    cin>> boundary;
    cout << "rule according to mapping: 111, 110, 100, 000, 001, 010, 101, 011 " << endl;
    for (int i =0; i< 8;i++){
        cin >> rule_to[i];
    }

    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&boundary, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rule_to[0], 8, MPI_INT, 0, MPI_COMM_WORLD);
    double elapsed_time = MPI_Wtime();
 //   int N(12);
    int iter = N/10;
    int * a, * b;


    int N_proc = N/psize;

    int res = N%psize;
    if (prank ==psize-1){
        N_proc = N_proc+res;
        
    }
    a = (int * )malloc(sizeof(int)*N);
    b = (int * )malloc(sizeof(int)*(N_proc+2));
    int * next = (int * )malloc(sizeof(int)*(N_proc+2));
	int *size = (int *)calloc(sizeof(int), psize);
	int *displs = (int *)calloc(sizeof(int), psize);
    int *pair = (int *)malloc(sizeof(int)*2);
	
	for (int k = 0; k < psize; k++)
	{
		size[k] = (int) N / (double) psize;
		displs[k] = k * size[k];
	}

	size[psize -1] += res;

    FILE *f;
    f  = fopen("out_auto.txt", "w+");

    for (int i=1;i<=N_proc;i++){
        b[i] = distr(gen);
    }
    int idx(0);
    for (int step=0; step<iter;step++){
    
            if (boundary==1){
                left = b[1];
               right = b[N_proc];
            }
            else{
                if(prank == 0){
                left = 0;
                    right = b[N_proc];
                }
                else if (prank == psize-1){
                     right = 0;
                     left = b[1];
                }
                else{
                     right = b[N_proc];
                     left = b[1];
                }
            }
            if (psize == 1){
                b[0] = right;
                b[N_proc+1] = left;
            }
            else if (psize ==2){
                pair[0] = left;
                pair[1] = right;
                MPI_Send(&pair[0], 2, MPI_INT, (prank-1+psize)%psize, 0, MPI_COMM_WORLD);
                MPI_Recv(&pair[0], 2, MPI_INT, (prank+1+psize)%psize, 0, MPI_COMM_WORLD,  &status);
                b[0] =pair[1]; //left
                b[N_proc+1] = pair[0]; //right

            }
            else{
            MPI_Send(&left, 1, MPI_INT, (prank-1+psize)%psize, 0, MPI_COMM_WORLD);
            MPI_Send(&right, 1, MPI_INT, (prank+1+psize)%psize, 0, MPI_COMM_WORLD);

            MPI_Recv(&b[0], 1, MPI_INT, (prank-1+psize)%psize, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&b[N_proc+1], 1, MPI_INT, (prank+1+psize)%psize, 0, MPI_COMM_WORLD,  &status);
            }

        for (int i=1;i<=N_proc;i++){   
            for (int j =0;j<8;j++){
                if(b[i-1]==rule_from[j]&&b[i]==rule_from[j+1]&&b[i+1]==rule_from[j+2]){
                    idx = j;
                    break;
                }
            } 
            next[i] = rule_to[idx];

        }
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Gatherv(&b[1], N_proc, MPI_INT, a, size, displs, MPI_INT, 0, MPI_COMM_WORLD);
        for (int i =1; i<=N_proc;i++){
            b[i] = next[i];
        }
        if (prank==0){
            for (int i =0; i< N;i++){
                fprintf(f, "%d\t", a[i]);
            }
            fprintf(f, "\n");
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Gatherv(&next[1], N_proc, MPI_INT, a, size, displs, MPI_INT, 0, MPI_COMM_WORLD);
    if (prank==0){
        for (int i =0; i< N;i++){
            fprintf(f, "%d\t",a[i]);
        }
        fprintf(f, "\n");
    }
    if (prank==0){
        fclose(f);
    }
    elapsed_time = MPI_Wtime() - elapsed_time;
    if (prank ==0){
        cout << "Elapsed time " << elapsed_time << endl;
    }

	free(a);
	free(b);
    free(next);
    free(size);
    free(displs);
	MPI_Finalize();

}