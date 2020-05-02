#include "pch.h"
#include "mpi.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#define CAST std::chrono::duration_cast<std::chrono::nanoseconds>
#define TIP_TIMP std::chrono::steady_clock::time_point

using namespace std;

int main()
{
	//variabile care nu sunt partajate
	int nrprocese, rank;
	TIP_TIMP timp_start, timp_end;

	//Initialize the MPI environment
	int init_ok = MPI_Init(NULL, NULL);
	if (init_ok != MPI_SUCCESS) {
		cout << "Err MPI Init";
		//MPI_COMM_WORLD comunicator predefinit
		MPI_Abort(MPI_COMM_WORLD, init_ok);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &nrprocese);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		//-----------------------------------------------------------------------------------START
		timp_start = std::chrono::high_resolution_clock::now();

		cout << "RANK " << rank << endl;

	}
	else{
		cout << "RANK** " << rank << endl;
	}

	cout << endl << "Finalize " << rank <<"...";
	MPI_Finalize();
}