#include <vector>
#include <mpi.h>
#include <iostream>
#include <random>
#include <functional>
using namespace std;

int main(int argc, char** argv) {
    int N(5);
    MPI_Init(&argc, &argv);
    mt19937 gen;
    random_device rand_device;
    gen.seed(rand_device());
    int next_player(0);
    int prank;
    int psize;
    int size_(1);
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &prank);

    uniform_int_distribution<> distr(0, psize-1);
    vector<int> vec;
    vec.resize(N, 0);
    // printf("I am processor #%d of total %d processes\n", prank, psize);
    if(prank==0){
        cout<< "Starting"<< endl;
        vec[0] = 0;
        while (next_player==0){
        next_player = distr(gen);
        cout << next_player<< endl;
        }
        cout << "Sending to process "<< next_player<< endl;
       // cout << size_ << endl;
        MPI_Ssend(&size_, 1, MPI_INT, next_player, 0, MPI_COMM_WORLD);
        MPI_Ssend(&vec[0], 1, MPI_INT, next_player, 0, MPI_COMM_WORLD);
        


    }
    while(true){
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&size_, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
        if (size_==-1){
            break;
        }
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&vec[0], size_, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
        cout << "Process " << prank <<" catched the ball "<< size_ << endl;
        size_+=1;
        vec[size_-1] = prank;
        for (int i=0; i<size_; i++){
            cout << vec[i]<< " ";
        }
        cout << endl;


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
      //   cout << next_player<< endl;
        }
        
    //    cout << size_<< endl;
        cout << "Sending to process "<< next_player<< endl;
        MPI_Ssend(&size_, 1, MPI_INT, next_player, 0, MPI_COMM_WORLD);
        MPI_Ssend(&vec[0], size_, MPI_INT, next_player, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;


}
