#include "pch.h"
#include "mpi.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <stddef.h>

#include "utils.h"

#define CAST std::chrono::duration_cast<std::chrono::nanoseconds>
#define TIP_TIMP std::chrono::steady_clock::time_point

//void build_mpi_matrice_csr_type(unsigned int* i, unsigned int* j, MPI_Datatype* mytype)
//{
//	int array_of_blocklengths[2] = { 1,1 };
//	MPI_Datatype array_of_types[2] = { MPI_INT, MPI_INT };
//	MPI_Aint i_addr, j_addr;
//	MPI_Aint array_of_displacements[2] = { 0 };
//	MPI_Get_address(i, &i_addr);
//	MPI_Get_address(j, &j_addr);
//	array_of_displacements[1] = j_addr - i_addr;
//	MPI_Type_create_struct(2, array_of_blocklengths, array_of_displacements, array_of_types, mytype);
//	MPI_Type_commit(mytype);
//}

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

		//procesul 0 trimite matricea B la toate celelalte procese
		test.push_back(15);
		test.push_back(9);
		MPI_Send(&test[0], test.size(), MPI_LONG_LONG, 1, 0, MPI_COMM_WORLD);
	}
	else {
		if (rank == 1) {
			std::cout << "RANK** " << rank << std::endl;

			test.resize(2);
			MPI_Recv(&test[0], 2, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD, &status);

			std::cout << "RANK** " << rank << " " << test[0] << " " << test[1] << std::endl;
		}
	}

	std::cout << std::endl << "Finalize " << rank <<"...";
	MPI_Finalize();
}