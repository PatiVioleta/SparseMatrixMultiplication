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
	int nrprocese, rank, flag = 0;
	MPI_Status status;
	MPI_Request request;
	char *segment_number1, *segment_number2, *number1, *number2;
	int *sendcounts, *displs;
	TIP_TIMP timp_start, timp_end;

	number1 = (char*)malloc(0);
	number2 = (char*)malloc(0);
	sendcounts = (int*)malloc(0);
	displs = (int*)malloc(0);
	int rem = 0, sum = 0, len_segm = 0;

	//Initialize the MPI environment
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS) {
		cout << "Err MPI Init";
		//MPI_COMM_WORLD comunicator predefinit
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &nrprocese);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		//-----------------------------------------------------------------------------------START
		timp_start = std::chrono::high_resolution_clock::now();

		//CITIRE NUMERE MARI
		FILE *file1, *file2;
		errno_t err1 = fopen_s(&file1, "C:\\Users\\pati\\Desktop\\lab2\\numar1.txt", "r");
		errno_t err2 = fopen_s(&file2, "C:\\Users\\pati\\Desktop\\lab2\\numar2.txt", "r");

		//lungime fisier1
		fseek(file1, 0, SEEK_END);
		int len1 = ftell(file1);
		fseek(file1, 0, SEEK_SET);

		//lungime fisier2
		fseek(file2, 0, SEEK_END);
		int len2 = ftell(file2);
		fseek(file2, 0, SEEK_SET);

		//citire numar 1
		number1 = (char*)malloc(sizeof(char)*len1 + 1);
		for (int i = len1 - 1; i >= 0; i--)
		{
			fscanf_s(file1, "%c", number1 + i);
		}
		number1[len1] = '\0';

		//citire numar 2
		number2 = (char*)malloc(sizeof(char)*len2 + 1);
		for (int i = len2 - 1; i >= 0; i--)
		{
			fscanf_s(file2, "%c", number2 + i);
		}
		number2[len2] = '\0';

		//number1 - numarul mai mic
		if (len1 > len2) {
			char *aux = number1;
			number1 = number2;
			number2 = aux;

			int auxx = len1;
			len1 = len2;
			len2 = auxx;
		}

		rem = len1 % nrprocese;
		sendcounts = (int*)malloc(sizeof(int)*len1);
		displs = (int*)malloc(sizeof(int)*len1);

		// calculate send counts and displacements
		//pentru procesul 0
		len_segm = len1 / nrprocese;
		sendcounts[0] = len_segm;
		if (rem > 0) {
			sendcounts[0]++;
			len_segm++;
			rem--;
		}

		//cout << "RANK " << rank << " : len_segm=" << len_segm << endl;

		displs[0] = sum;
		sum += sendcounts[0];

		//pentru restul proceselor
		int len_segm2 = len1 / nrprocese;
		for (int i = 1; i < nrprocese; i++) {
			sendcounts[i] = len_segm2;

			if (rem > 0) {
				sendcounts[i]++;
				rem--;
			}

			//trimitem fiecarui proces lungimea segmentului corespunzator
			MPI_Send(&sendcounts[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);

			displs[i] = sum;
			sum += sendcounts[i];
		}
		/*for (int i = 0; i < nrprocese; i++) {
			printf("sendcounts[%d] = %d\tdispls[%d] = %d\n", i, sendcounts[i], i, displs[i]);
		}*/
	}
	else {
		MPI_Recv(&len_segm, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		//cout << "RANK " << rank << " : len_segm=" << len_segm << endl;
	}

	//alocam spatiu si pentru \0
	segment_number1 = (char*)malloc(sizeof(char)*(len_segm + 1));
	segment_number2 = (char*)malloc(sizeof(char)*(len_segm + 1));

	MPI_Scatterv(number1, sendcounts, displs, MPI_CHAR, segment_number1, len_segm + 1, MPI_CHAR, 0, MPI_COMM_WORLD);
	MPI_Scatterv(number2, sendcounts, displs, MPI_CHAR, segment_number2, len_segm + 1, MPI_CHAR, 0, MPI_COMM_WORLD);

	segment_number1[len_segm] = '\0';
	segment_number2[len_segm] = '\0';

	//cout << "RANK " << rank << " : " << segment_number1 <<" "<<segment_number2 << endl;

	if (rank == 0) {
		char *summ;
		int len1 = strlen(number1), len2 = strlen(number2);

		summ = (char*)malloc(sizeof(char)*len_segm);

		//Calcul suma partiale pentru procesul 0
		int temporalCarry = 0;
		for (int i = 0; i < len_segm; i++) {
			int temporalSum = segment_number1[i] - '0' + segment_number2[i] - '0' + temporalCarry;
			summ[i] = (temporalSum % 10) + '0';
			//cout << "RANK " << rank << " " << segment_number1[i] - '0' << "+" << segment_number2[i] - '0' << "+" << temporalCarry << " = " << temporalSum << " => "<< summ[i]<< endl;
			temporalCarry = temporalSum / 10;
		}
		if (temporalCarry != 0) {
			summ[len_segm] = (char)malloc(sizeof(char));
			summ[len_segm] = temporalCarry + '0';
			//cout << "RANK " << rank << " carry=" << summ[len_segm] << endl;
			len_segm++;
		}

		//caracterul '\0'
		summ[len_segm] = (char)malloc(sizeof(char));
		summ[len_segm] = '\0';

		//cout << "RANK " << rank << " suma=" << summ << endl;

		//Pregatim suma finala (len2 + 1 -> rez poate avea maxim o cifra in plus fata de cel mai mare numar )
		vector<char> summ_final;
		for (int i = 0; i < len2 + 1; i++)
			summ_final.push_back('0');

		//Punem suma din procesul 0 in suma finala
		for (int i = 0; i < len_segm; i++)
			summ_final[i] = summ[i];

		/*cout << "RANK " << rank << " suma finala=";
		for (int i = 0; i < len2 + 1; i++)
			cout << summ_final[i];
		cout << endl;*/

		//SUMELE PRIMITE
		char **summ_primit = (char**)malloc(sizeof(char)*nrprocese);
		int recv_count_max = len1 / nrprocese + 1;	//lungimea segm maxim trimisa din pr 0
		recv_count_max += 1 + 1 + 1;					//o sa primim in plus: rank, carry, '\0'
		recv_count_max = recv_count_max * 2;

		//Primim sumele intr-o ordine oarecare
		for (int i = 1; i < nrprocese; i++) {
			summ_primit[i] = (char*)malloc(sizeof(char)*recv_count_max);
			MPI_Recv(summ_primit[i], recv_count_max, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			//cout << "FROM RANK " << summ_primit[i][0] << " : " << summ_primit[i] << endl;
		}

		int start;
		//Concatenare sume primite in suma finala
		for (int i = 1; i < nrprocese; i++) {
			int rank_sender = summ_primit[i][0] - '0';

			//Calculam pozitia de start
			if (rank_sender < len1 % nrprocese)
				start = rank_sender * (len1 / nrprocese + 1);
			else
				start = len1 % nrprocese + len1 / nrprocese * rank_sender;

			int j = 1, temporalCarry = 0;
			while (summ_primit[i][j] != '\0') {
				int temporalSum = summ_final[start] - '0' + summ_primit[i][j] - '0' + temporalCarry;
				summ_final[start] = (temporalSum % 10) + '0';
				temporalCarry = temporalSum / 10;
				start++;
				j++;
			}
			if (temporalCarry != 0) {
				int temporalSum = summ_final[start] - '0' + temporalCarry;
				summ_final[start] = temporalSum + '0';
				len_segm++;
			}
		}

		/*cout << "RANK " << rank << " suma finala=";
		for (int i = 0; i < len2 + 1; i++)
			cout << summ_final[i];
		cout << endl;*/

		//Concatenare cu restul din numarul mare
		start = len1 % (nrprocese)+len1 / nrprocese * nrprocese;
		temporalCarry = 0;
		for (int i = start; i < len2; i++) {
			//cout << "pozitia: " << i << " cifra: " << number2[i] << endl;
			int temporalSum = summ_final[i] - '0' + number2[i] - '0' + temporalCarry;
			summ_final[i] = (temporalSum % 10) + '0';
			temporalCarry = temporalSum / 10;
		}
		if (temporalCarry != 0) {
			int temporalSum = summ_final[len2] - '0' + temporalCarry;
			summ_final[len2] = temporalSum + '0';
		}

		//-----------------------------------------------------------------------------------END
		timp_end = std::chrono::high_resolution_clock::now();
		cout << "TIMP: " << CAST(timp_end - timp_start).count() << endl;

		ofstream g("C:\\Users\\pati\\Desktop\\lab2\\rezultat.txt");

		cout << endl;
		if (summ_final[len2] == '0')	len2--;
		for (int i = len2; i >= 0; i--) {
			cout << summ_final[i];
			g << summ_final[i];
		}
	}
	else {
		char *summ;
		summ = (char*)malloc(sizeof(char)*len_segm);

		//Punem rankul pe prima pozitie
		summ[0] = rank + '0';
		summ[len_segm] = (char)malloc(sizeof(char));
		len_segm++;

		//Calcul suma partiala
		int temporalCarry = 0;
		for (int i = 0; i < len_segm - 1; i++) {
			int temporalSum = segment_number1[i] - '0' + segment_number2[i] - '0' + temporalCarry;
			summ[i + 1] = (temporalSum % 10) + '0';
			//cout << "RANK " << rank << " " << segment_number1[i] - '0' << "+" << segment_number2[i] - '0' << "+" << temporalCarry << " = " << temporalSum << " => "<< summ[i]<< endl;
			temporalCarry = temporalSum / 10;
		}
		if (temporalCarry != 0) {
			summ[len_segm] = (char)malloc(sizeof(char));
			summ[len_segm] = temporalCarry + '0';
			//cout << "RANK " << rank << " carry=" << summ[len_segm] << endl;
			len_segm++;
		}

		//caracterul '\0'
		summ[len_segm] = (char)malloc(sizeof(char));
		summ[len_segm] = '\0';

		//cout << "RANK " << rank << " rank+suma=" << summ << endl;

		//Trimitem suma partiala procesului 0 impreuna cu '\0' ca sa stim cand se termina sirul in pr 0
		MPI_Send(summ, len_segm + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}


	//cout << endl << "Finalize " << rank <<"...";
	MPI_Finalize();
}

//MPI_Wait
//MPI_Test
//MPI_Irecv