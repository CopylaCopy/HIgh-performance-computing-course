#include <vector>
#include <mpi.h>
#include <iostream>
#include <random>
#include <functional>
using namespace std;

int main(int argc, char** argv) {
    int N(100000);
    MPI_Init(&argc, &argv);
    mt19937 gen;
    random_device rand_device;
    gen.seed(rand_device());
    int next_player(0);
    int prank;
    int psize;
    int size_(1);
    int fixed_message(1024);
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &prank);

    uniform_int_distribution<> distr(0, psize-1);
    vector<int> vec;
    vec.resize(N, 0);
    if(prank==0){
        cout<< "Starting"<< endl;
        vec[0] = 0;
        while (next_player==0){
        next_player = distr(gen);
        }
     //   cout << "Sending to process "<< next_player<< endl;
       // cout << size_ << endl;
        MPI_Ssend(&size_, 1, MPI_INT, next_player, 0, MPI_COMM_WORLD);
        MPI_Ssend(&vec[0], 1, MPI_INT, next_player, 0, MPI_COMM_WORLD);
        


    }
    double elapsed_time = MPI_Wtime();
    while(true){
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&size_, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
        if (size_==-1){
            break;
        }
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&vec[0], fixed_message, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
    //    cout << "Process " << prank <<" catched the ball "<< size_ << endl;
        size_+=1;
        int pos;
        if (size_>fixed_message){
            pos = fixed_message;
        }
        else{
        pos = size_-1;}
        vec[pos] = prank;
        // for (int i=0; i<fixed_message+1; i++){
        //     cout << vec[i]<< " ";
        // }
        // cout << endl;


        if(size_==N){
            cout<< "Finish!"<<endl;
            size_ = -1;
            for (int i=0;i<psize;i++){
                if(i!=prank){
                    MPI_Send(&size_, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                }
            }
            break;
        }
        next_player = prank;
        while (next_player==prank){
                   
        next_player = distr(gen);
        }
        
     //  cout << "Sending to process "<< next_player<< endl;
        MPI_Ssend(&size_, 1, MPI_INT, next_player, 0, MPI_COMM_WORLD);
        int idx = 0;
        if (size_<=fixed_message){
            idx = 0;
        }
        else{
            idx = 1;
        }
        MPI_Ssend(&vec[idx], fixed_message, MPI_INT, next_player, 0, MPI_COMM_WORLD);
    }
        elapsed_time = MPI_Wtime() - elapsed_time;
        if (prank ==0){
        cout << "Elapsed time " << elapsed_time << endl;
    }

    MPI_Finalize();
    return 0;


}
