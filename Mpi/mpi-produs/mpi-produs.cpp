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

	MatriceCSR Matrice2;
	std::vector<long long> test;

	//PROCESUL 0
	if (rank == 0) {
		std::cout << "RANK " << rank << std::endl;

		MatriceCSR Matrice1;

		//procesul 0 citeste cele 2 matrici din fisier direct in format CSR
		citireMatrice(Matrice1File, Matrice1);
		citireMatrice(Matrice2File, Matrice2);

		//procesul 0 face transpusa maricei B tot in format CSR
		Matrice2 = transpusaCSR(Matrice2);
	}

	//procesul 0 trimite matricea B la toate celelalte procese
	mpi_bcast_matrice_csr(Matrice2, rank);

	std::cout << std::endl << "Finalize " << rank <<"...";
	MPI_Finalize();
}