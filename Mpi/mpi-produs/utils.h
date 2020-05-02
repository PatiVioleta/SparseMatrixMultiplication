#ifndef UTILS_H    
#define UTILS_H

#include "pch.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

struct MatriceCSR {
	long long nr_linii, nr_coloane;
	long long V_COL_INDEX_size, ROW_INDEX_size;
	std::vector<long long> V, COL_INDEX, ROW_INDEX;
};

void citireMatrice(std::ifstream &MatriceFile, MatriceCSR &Matrice);

void scriereMatrice(std::ofstream &MatriceFile, std::vector< std::vector<long long> > &Matrice, int nr_linii, int nr_coloane);

void scriereMatriceCSR(std::ofstream &MatriceFile, MatriceCSR &Matrice);

//calculeaza transpusa in format CSR a unei matrice in format CSR
MatriceCSR transpusaCSR(MatriceCSR &Matrice);

//returneaza subsecventa din vector incepand de la indexul x si terminand la indexul y-1
std::vector<long long> subsecventa(std::vector<long long> vector, long long x, long long y);

//returneaza elementele comune a doi vectori (ca valoare) si indecsii corespunzatori valorilor comune pentru fiecare dintre cei doi vectori (idx1 si idx2)
std::vector<long long> intersectie(std::vector<long long> vector1, std::vector<long long> vector2, std::vector<long long> &idx1, std::vector<long long> &idx2);




#endif