#include "pch.h"
#include "mpi.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <stddef.h>

#include "utils.h"

void mpi_bcast_matrice_csr(MatriceCSR &Matrice, int rank) {
	MPI_Bcast(&(Matrice.nr_linii), 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
	MPI_Bcast(&(Matrice.nr_coloane), 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

	MPI_Bcast(&(Matrice.V_size), 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
	MPI_Bcast(&(Matrice.COL_INDEX_size), 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
	MPI_Bcast(&(Matrice.ROW_INDEX_size), 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

	if (rank != 0) {
		Matrice.V.resize(Matrice.V_size);
		Matrice.COL_INDEX.resize(Matrice.COL_INDEX_size);
		Matrice.ROW_INDEX.resize(Matrice.ROW_INDEX_size);
	}

	MPI_Bcast(&(Matrice.V[0]), Matrice.V_size, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
	MPI_Bcast(&(Matrice.COL_INDEX[0]), Matrice.COL_INDEX_size, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
	MPI_Bcast(&(Matrice.ROW_INDEX[0]), Matrice.ROW_INDEX_size, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

	if (rank != 0) {
		std::cout << "RANK** " << rank << std::endl;

		for (int i = 0; i < Matrice.V_size; i++)
			std::cout << Matrice.V[i] << " ";
		std::cout << std::endl;

		for (int i = 0; i < Matrice.COL_INDEX_size; i++)
			std::cout << Matrice.COL_INDEX[i] << " ";
		std::cout << std::endl;

		for (int i = 0; i < Matrice.ROW_INDEX_size; i++)
			std::cout << Matrice.ROW_INDEX[i] << " ";
		std::cout << std::endl;
	}
}