#include "pch.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <chrono>

#define CAST std::chrono::duration_cast<std::chrono::nanoseconds>

using namespace std;

struct MatriceCSR {
	long long nr_linii, nr_coloane;
	vector<long long> V, COL_INDEX, ROW_INDEX;
} Matrice1, Matrice2;

void citireMatrice(ifstream &MatriceFile, MatriceCSR &Matrice) {
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
}

void scriereMatrice(ofstream &MatriceFile, MatriceCSR &Matrice) {
	MatriceFile << Matrice.nr_linii << " " << Matrice.nr_coloane << endl;

	for (long long idx = 0; idx < Matrice.ROW_INDEX.size() - 1; idx++) {

		long long idxColMatrice = 0;

		for (long long idxColCsr = Matrice.ROW_INDEX.at(idx); idxColCsr < Matrice.ROW_INDEX.at(idx + 1); idxColCsr++) {
			while (idxColMatrice != Matrice.COL_INDEX.at(idxColCsr)) {
				MatriceFile << 0 << " ";
				idxColMatrice++;
			}
			MatriceFile << Matrice.V.at(idxColCsr) << " ";
			idxColMatrice++;
		}
		
		while (idxColMatrice < Matrice.nr_coloane) {
			MatriceFile << 0 << " ";
			idxColMatrice++;
		}

		MatriceFile << endl;
	}
}

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

	return Rezultat;
}

map<long long, long long> extragereMap(MatriceCSR &Matrice, long long start, long long end) {
	map<long long, long long> rezultat;
	for (long long i = start; i < end; i++)
		rezultat.insert(pair<long long, long long>(Matrice.COL_INDEX.at(i), Matrice.V.at(i)));
	return rezultat;
}

int calculareProdus(map<long long, long long> &map1, map<long long, long long> &map2) {
	long long rezultat = 0;
	for (map<long long, long long>::iterator it = map1.begin(); it != map1.end(); ++it) {
		try {
			long long valoareMap2 = map2.at(it->first);
			rezultat += it->second * valoareMap2;
		}
		catch(out_of_range err){}
	}
	return rezultat;
}

MatriceCSR produsCSR(MatriceCSR &Matrice1, MatriceCSR &Matrice2) {
	MatriceCSR Rezultat;

	Rezultat.nr_linii = Matrice1.nr_linii;
	Rezultat.nr_coloane = Matrice2.nr_linii;
	Rezultat.ROW_INDEX.push_back(0);

	for (long long idx1 = 0; idx1 < Matrice1.ROW_INDEX.size() - 1; idx1++) {
		map<long long, long long> linieCurentaMatrice1 = extragereMap(Matrice1, Matrice1.ROW_INDEX.at(idx1), Matrice1.ROW_INDEX.at(idx1+1));

		long long idxLinieMatrice2 = 0;
		for (long long idx2 = 0; idx2 < Matrice2.ROW_INDEX.size() - 1; idx2++) {
			map<long long, long long> linieCurentaMatrice2 = extragereMap(Matrice2, Matrice2.ROW_INDEX.at(idx2), Matrice2.ROW_INDEX.at(idx2 + 1));

			long long valoareCurenta = calculareProdus(linieCurentaMatrice1, linieCurentaMatrice2);
			if (valoareCurenta != 0) {
				Rezultat.V.push_back(valoareCurenta);
				Rezultat.COL_INDEX.push_back(idxLinieMatrice2);
			}

			idxLinieMatrice2++;
		}
		
		Rezultat.ROW_INDEX.push_back(Rezultat.V.size());
	}

	return Rezultat;
}

int main()
{
	string prefix_fisier = "C:\\Users\\pati\\Desktop\\UNIV\\SEM6\\PP\\proiect\\data\\generat1";

	ifstream Matrice1File(prefix_fisier + "A.txt");
	ifstream Matrice2File(prefix_fisier + "B.txt");
	ofstream RezultatFile(prefix_fisier + "C.txt");

	citireMatrice(Matrice1File, Matrice1);
	citireMatrice(Matrice2File, Matrice2);

	Matrice2 = transpusaCSR(Matrice2);

	//------------------------------------------------------------------
	auto start = std::chrono::high_resolution_clock::now();

	MatriceCSR Rezultat = produsCSR(Matrice1, Matrice2);

	auto end = std::chrono::high_resolution_clock::now();
	RezultatFile << "Timp: " << CAST(end - start).count() << endl;
	//--------------------------------------------------------------------

	scriereMatrice(RezultatFile, Rezultat);

	Matrice1File.close();
	Matrice2File.close();
	RezultatFile.close();
}