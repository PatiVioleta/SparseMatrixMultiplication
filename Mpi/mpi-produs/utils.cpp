#include "pch.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <chrono>

#include "utils.h"

void citireMatrice(std::ifstream &MatriceFile, MatriceCSR &Matrice) {
	MatriceFile >> Matrice.nr_linii;
	MatriceFile >> Matrice.nr_coloane;

	Matrice.ROW_INDEX.push_back(0);

	for (long long i = 0; i < Matrice.nr_linii; i++) {
		for (long long j = 0; j < Matrice.nr_coloane; j++) {
			long long x;
			MatriceFile >> x;
			if (x != 0) {
				Matrice.V.push_back(x);
				Matrice.COL_INDEX.push_back(j);
			}
		}
		Matrice.ROW_INDEX.push_back(Matrice.V.size());
	}

	Matrice.V_COL_INDEX_size = Matrice.V.size();
	Matrice.ROW_INDEX_size = Matrice.ROW_INDEX.size();
}

void afisareMatrice(std::vector< std::vector<long long> > &Matrice, int nr_linii, int nr_coloane) {
	std::cout << nr_linii << " " << nr_coloane << std::endl;
	for (long long i = 0; i < nr_linii; i++) {
		for (long long j = 0; j < nr_coloane; j++) {
			std::cout << Matrice[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void scriereMatrice(FILE *MatriceFile, std::vector< std::vector<long long> > &Matrice, int nr_linii, int nr_coloane, int rank) {
	//fseek(MatriceFile, 9, SEEK_SET);
	for (long long i = 0; i < nr_linii; i++) {
		for (long long j = 0; j < nr_coloane; j++) {
			fprintf(MatriceFile, "%d ", Matrice[i][j]);
		}
		fprintf(MatriceFile, "\n");
	}
}

void scriereMatriceCSR(std::ofstream &MatriceFile, MatriceCSR &Matrice) {
	MatriceFile << Matrice.nr_linii << " " << Matrice.nr_coloane << std::endl;
	for (int i = 0; i < Matrice.V.size(); i++)
		MatriceFile << Matrice.V[i] << " ";
	MatriceFile << std::endl;
	for (int i = 0; i < Matrice.COL_INDEX.size(); i++)
		MatriceFile << Matrice.COL_INDEX[i] << " ";
	MatriceFile << std::endl;
	for (int i = 0; i < Matrice.ROW_INDEX.size(); i++)
		MatriceFile << Matrice.ROW_INDEX[i] << " ";
}

//calculeaza transpusa in format CSR a unei matrice in format CSR
MatriceCSR transpusaCSR(MatriceCSR &Matrice) {
	MatriceCSR Rezultat;

	Rezultat.nr_linii = Matrice.nr_coloane;
	Rezultat.nr_coloane = Matrice.nr_linii;
	Rezultat.ROW_INDEX.push_back(0);

	for (long long j = 0; j < Matrice.nr_coloane; j++) {
		for (long long i = 0; i < Matrice.nr_coloane; i++) {
			if (Matrice.COL_INDEX.at(i) == j) {
				Rezultat.V.push_back(Matrice.V.at(i));

				long long k = 0;
				while (Matrice.ROW_INDEX.at(k) <= i)
					k++;
				Rezultat.COL_INDEX.push_back(k - 1);
			}
		}
		Rezultat.ROW_INDEX.push_back(Rezultat.V.size());
	}

	Rezultat.V_COL_INDEX_size = Rezultat.V.size();
	Rezultat.ROW_INDEX_size = Rezultat.ROW_INDEX.size();

	return Rezultat;
}

//returneaza subsecventa din vector incepand de la indexul x si terminand la indexul y-1
std::vector<long long> subsecventa(std::vector<long long> vector, long long x, long long y) {
	std::vector<long long>::const_iterator first = vector.begin() + x;
	std::vector<long long>::const_iterator last = vector.begin() + y;
	std::vector<long long> rezultat(first, last);
	return rezultat;
}

//returneaza elementele comune a doi vectori (ca valoare) si indecsii corespunzatori valorilor comune pentru fiecare dintre cei doi vectori (idx1 si idx2)
std::vector<long long> intersectie(std::vector<long long> vector1, std::vector<long long> vector2, std::vector<long long> &idx1, std::vector<long long> &idx2) {
	std::vector<long long> rezultat;
	long long curent1 = 0, curent2 = 0;

	while (curent1 < vector1.size() && curent2 < vector2.size()) {
		long long left = vector1[curent1];
		long long right = vector2[curent2];

		if (left == right) {
			rezultat.push_back(right);
			idx1.push_back(curent1);
			idx2.push_back(curent2);

			while (curent1 < vector1.size() && vector1[curent1] == left)
				curent1++;
			while (curent2 < vector2.size() && vector2[curent2] == right)
				curent2++;
			continue;
		}

		if (left < right) {
			while (curent1 < vector1.size() && vector1[curent1] == left)
				curent1++;
		}
		else
			while (curent2 < vector2.size() && vector2[curent2] == right)
				curent2++;
	}
	return rezultat;
}

std::vector< std::vector<long long> > produsCSR(MatriceCSR &Matrice1, MatriceCSR &Matrice2) {
	//Pregatire matrice rezultat
	std::vector< std::vector<long long> > A;
	A.resize(Matrice1.nr_linii);
	for (long long i = 0; i < Matrice1.nr_linii; ++i)
	{
		A[i].resize(Matrice2.nr_linii);
	}

	//Calculare produs
	long long linii_matrice1 = Matrice1.nr_linii;
	long long linii_matrice2 = Matrice2.nr_linii;
	for (long long linie_curenta_matrice1 = 1; linie_curenta_matrice1 <= linii_matrice1; linie_curenta_matrice1++) {

		std::vector<long long> COL_INDEX_1 = subsecventa(Matrice1.COL_INDEX, Matrice1.ROW_INDEX[linie_curenta_matrice1 - 1], Matrice1.ROW_INDEX[linie_curenta_matrice1]);
		std::vector<long long> V_1 = subsecventa(Matrice1.V, Matrice1.ROW_INDEX[linie_curenta_matrice1 - 1], Matrice1.ROW_INDEX[linie_curenta_matrice1]);

		for (long long linie_curenta_matrice2 = 1; linie_curenta_matrice2 <= linii_matrice2; linie_curenta_matrice2++) {
			A[linie_curenta_matrice1 - 1][linie_curenta_matrice2 - 1] = 0;

			std::vector<long long> COL_INDEX_2 = subsecventa(Matrice2.COL_INDEX, Matrice2.ROW_INDEX[linie_curenta_matrice2 - 1], Matrice2.ROW_INDEX[linie_curenta_matrice2]);
			std::vector<long long> V_2 = subsecventa(Matrice2.V, Matrice2.ROW_INDEX[linie_curenta_matrice2 - 1], Matrice2.ROW_INDEX[linie_curenta_matrice2]);

			std::vector<long long> idx1;
			std::vector<long long> idx2;

			std::vector<long long> COL_INDEX_BOTH = intersectie(COL_INDEX_1, COL_INDEX_2, idx1, idx2);
			for (long long k = 0; k < COL_INDEX_BOTH.size(); k++) {
				A[linie_curenta_matrice1 - 1][linie_curenta_matrice2 - 1] += V_1[idx1[k]] * V_2[idx2[k]];
			}
		}
	}
	return A;
}