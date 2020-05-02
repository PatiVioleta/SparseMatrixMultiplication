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

	//VARIABILE PR 0
	MatriceCSR Matrice1;

	//VARIABILE PT TOATE
	std::string prefix_fisier = "C:\\Users\\pati\\Desktop\\UNIV\\SEM6\\PP\\proiect\\data\\generat0";

	std::ifstream Matrice1File(prefix_fisier + "A.txt");
	std::ifstream Matrice2File(prefix_fisier + "B.txt");
	std::ofstream RezultatFile(prefix_fisier + "C.txt");

	MatriceCSR Matrice2;
	long long nr_linii_matrice_A;	//folosit de procese la scrierea in fisier
	MatriceCSR MatriceProcess;	//procesele au setate doar: V, COL_INDEX, ROW_INDEX
	long long nr_linii_de_procesat;	//nr de linii pe care il va procesa fiecare proces

	//PROCESUL 0
	if (rank == 0) {
		//procesul 0 citeste cele 2 matrici din fisier direct in format CSR
		citireMatrice(Matrice1File, Matrice1);
		nr_linii_matrice_A = Matrice1.nr_linii;
		citireMatrice(Matrice2File, Matrice2);

		scriereMatriceCSR(RezultatFile, Matrice1);

		//procesul 0 face transpusa maricei B tot in format CSR
		Matrice2 = transpusaCSR(Matrice2);
	}

	//procesul 0 trimite matricea B la toate celelalte procese
	mpi_bcast_matrice_csr(Matrice2, rank);

	//procesul 0 trimite nr de linii din matricea A fiecarui proces
	MPI_Bcast(&nr_linii_matrice_A, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

	//fiecare proces isi calculeaza nr de linii de procesat
	if (rank == nrprocese - 1)
		nr_linii_de_procesat = nr_linii_matrice_A / nrprocese + nr_linii_matrice_A % nrprocese;
	else
		nr_linii_de_procesat = nr_linii_matrice_A / nrprocese;
	//std::cout << rank << " nr linii " << nr_linii_de_procesat << std::endl;

	//procesul 0 trimite liniile din matricea A corespunzatoare fiecarui proces
	int *send_nr_elem_nenule, *displs_nr_elem_nenule;
	int *send_nr_linii, *displs_nr_linii;

	if (rank == 0) {
		//alocare
		send_nr_elem_nenule = (int *)malloc(nrprocese * sizeof(int));
		displs_nr_elem_nenule = (int *)malloc(nrprocese * sizeof(int));
		send_nr_linii = (int *)malloc(nrprocese * sizeof(int));
		displs_nr_linii = (int *)malloc(nrprocese * sizeof(int));

		//calcul
		int nr_linii_proces_majoritate = nr_linii_matrice_A / nrprocese;

		for (int i = 0; i < nrprocese - 1; i++) {
			int start_poz_row_index = i * nr_linii_proces_majoritate;
			int end_poz_row_index = start_poz_row_index + nr_linii_proces_majoritate;

			send_nr_linii[i] = nr_linii_proces_majoritate;
			displs_nr_linii[i] = start_poz_row_index;

			send_nr_elem_nenule[i] = Matrice1.ROW_INDEX[end_poz_row_index] - Matrice1.ROW_INDEX[start_poz_row_index];
			displs_nr_elem_nenule[i] = Matrice1.ROW_INDEX[start_poz_row_index];
		}
		int start_poz_row_index = (nrprocese - 1) * nr_linii_proces_majoritate;
		int end_poz_row_index = start_poz_row_index + nr_linii_proces_majoritate + nr_linii_matrice_A % nrprocese;

		send_nr_linii[nrprocese - 1] = nr_linii_proces_majoritate + nr_linii_matrice_A % nrprocese;
		displs_nr_linii[nrprocese - 1] = (nrprocese - 1) * nr_linii_proces_majoritate;

		send_nr_elem_nenule[nrprocese - 1] = Matrice1.ROW_INDEX[end_poz_row_index] - Matrice1.ROW_INDEX[start_poz_row_index];
		displs_nr_elem_nenule[nrprocese - 1] = Matrice1.ROW_INDEX[start_poz_row_index];

		std::cout << "send_nr_elem_nenule" << std::endl;
		for (int i = 0; i < nrprocese; i++)
			std::cout << send_nr_elem_nenule[i] << " ";
		std::cout << std::endl;
		std::cout << "displs_nr_elem_nenule" << std::endl;
		for (int i = 0; i < nrprocese; i++)
			std::cout << displs_nr_elem_nenule[i] << " ";
		std::cout << std::endl;
		std::cout << "send_nr_linii" << std::endl;
		for (int i = 0; i < nrprocese; i++)
			std::cout << send_nr_linii[i] << " ";
		std::cout << std::endl;
		std::cout << "displs_nr_linii" << std::endl;
		for (int i = 0; i < nrprocese; i++)
			std::cout << displs_nr_linii[i] << " ";
		std::cout << std::endl;
	}
	else {
		send_nr_elem_nenule = NULL;
		displs_nr_elem_nenule = NULL;
		send_nr_linii = NULL;
		displs_nr_linii = NULL;
	}

	//procesul 0 trimite dimensiunile pentru vectorii V si COL_INDEX
	int size_v_col_index[1];
	MPI_Scatter(&(send_nr_elem_nenule[0]), 1, MPI_INT, &size_v_col_index, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//procesul 0 imparte vectorii V si COL_INDEX la toate procesele
	MatriceProcess.V.resize(size_v_col_index[0]);
	MatriceProcess.COL_INDEX.resize(size_v_col_index[0]);
	MPI_Scatterv(Matrice1.V.data(), send_nr_elem_nenule, displs_nr_elem_nenule, MPI_LONG_LONG, &(MatriceProcess.V[0]), size_v_col_index[0], MPI_LONG_LONG, 0, MPI_COMM_WORLD);
	MPI_Scatterv(Matrice1.COL_INDEX.data(), send_nr_elem_nenule, displs_nr_elem_nenule, MPI_LONG_LONG, &(MatriceProcess.COL_INDEX[0]), size_v_col_index[0], MPI_LONG_LONG, 0, MPI_COMM_WORLD);

	//procesul 0 imparte vectorul ROW_INDEX la toate procesele
	MatriceProcess.ROW_INDEX.resize(nr_linii_de_procesat + 1);
	MPI_Scatterv(Matrice1.ROW_INDEX.data(), send_nr_linii, displs_nr_linii, MPI_LONG_LONG, &(MatriceProcess.ROW_INDEX[0]), nr_linii_de_procesat, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
	MatriceProcess.ROW_INDEX[nr_linii_de_procesat] = MatriceProcess.ROW_INDEX[0] + MatriceProcess.V.size();
	
	for (int i = 0; i < MatriceProcess.V.size(); i++)
		std::cout << MatriceProcess.V[i] << " ";
	std::cout << std::endl;

	for (int i = 0; i < MatriceProcess.COL_INDEX.size(); i++)
		std::cout << MatriceProcess.COL_INDEX[i] << " ";
	std::cout << std::endl;

	for(int i=0; i< MatriceProcess.ROW_INDEX.size();i++)
		std::cout << MatriceProcess.ROW_INDEX[i] << " ";
	std::cout << std::endl;
	


	std::cout << "Finalize " << rank <<"...";
	MPI_Finalize();
}