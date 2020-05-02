#include "pch.h"
#include "mpi.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <stddef.h>

#include "utils.h"
#include "mpi-comunicare.h"

#define CAST std::chrono::duration_cast<std::chrono::nanoseconds>
#define TIP_TIMP std::chrono::steady_clock::time_point

int main()
{
	//INITIALIZARE MEDIU MPI
	int nrprocese, rank;
	TIP_TIMP timp_start, timp_end;
	MPI_Status status;

	int init_ok = MPI_Init(NULL, NULL);
	if (init_ok != MPI_SUCCESS) {
		std::cout << "Err MPI Init";
		MPI_Abort(MPI_COMM_WORLD, init_ok);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &nrprocese);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//VARIABILE NEPARTAJATE
	std::string prefix_fisier = "C:\\Users\\pati\\Desktop\\UNIV\\SEM6\\PP\\proiect\\data\\matrice";

	std::ifstream Matrice1File(prefix_fisier + "A.txt");
	std::ifstream Matrice2File(prefix_fisier + "B.txt");
	std::ofstream RezultatFile(prefix_fisier + "C.txt");

	MatriceCSR Matrice1, Matrice2;
	long long nr_linii_matrice_A;
	std::vector<long long> test;

	//PROCESUL 0
	if (rank == 0) {
		std::cout << "RANK " << rank << std::endl;

		//procesul 0 citeste cele 2 matrici din fisier direct in format CSR
		citireMatrice(Matrice1File, Matrice1);
		nr_linii_matrice_A = Matrice1.nr_linii;
		citireMatrice(Matrice2File, Matrice2);

		//procesul 0 face transpusa maricei B tot in format CSR
		Matrice2 = transpusaCSR(Matrice2);
	}

	//procesul 0 trimite matricea B la toate celelalte procese
	mpi_bcast_matrice_csr(Matrice2, rank);

	//procesul 0 trimite liniile din matricea A corespunzatoare fiecarui proces
	MPI_Bcast(&nr_linii_matrice_A, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

	int sendcounts[2];
	sendcounts[0]=2;
	sendcounts[1]=3;

	int displs[2];
	displs[0] = 3;
	displs[1] = 0;

	test.resize(3);

	MPI_Scatterv(Matrice1.V.data(), sendcounts, displs, MPI_LONG_LONG, &(test[0]), sendcounts[rank], MPI_LONG_LONG, 0, MPI_COMM_WORLD);

	
		std::cout << nr_linii_matrice_A << std::endl;

		for(int i=0; i< 3;i++)
			std::cout << test[i] << " ";
		std::cout << std::endl;
	


	std::cout << "Finalize " << rank <<"...";
	MPI_Finalize();
}