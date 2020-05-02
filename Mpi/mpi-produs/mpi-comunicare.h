#ifndef MPI_COMUNICARE_H    
#define MPI_COMUNICARE_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <stddef.h>

#include "utils.h"

void mpi_bcast_matrice_csr(MatriceCSR &Matrice, int rank);

#endif