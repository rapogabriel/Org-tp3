#include <chrono>
#include <fstream>
#include <iosfwd>
#include <random>
#include "geradorInstrucoes.hpp"

using namespace std;
void gerarInstrucao(int repeticoes, int tamFor, int numIns, int tamMem, std::string arquivo) {
    ofstream instrucoes;
	instrucoes.open(arquivo);

	vector<vector<int> > vetor; vetor.clear();
	vector<int> aux; aux.clear();

	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	mt19937 motor(seed);

	int qtFor = numIns / tamFor;
	int qtRepeticoes = (repeticoes / 100.0 + 0.0001)*(qtFor);

	uniform_int_distribution<> dist_for(0, qtFor-1);
	uniform_int_distribution<> dist_opcode(0, 1);
	uniform_int_distribution<> dist_mem(0, tamMem-1);
	uniform_int_distribution<> dist_endPalavra(0, 3);

	vector<int> sequence; sequence.clear();
	for (int i = 0; i < qtFor; i++)
		sequence.push_back(i);

	for (int i = 0; i < qtFor - qtRepeticoes; i++)
	{
		size_t tmp = dist_for(motor);
		while (tmp >= sequence.size())
			tmp -= sequence.size();

		sequence.erase(sequence.begin() + tmp);
	}

	//Sequencia aleatoria completa
	for (int i = 0; i<tamFor; i++)
	{
		aux.push_back(dist_opcode(motor));
		for (int add = 0; add < 3; add++)
		{
			aux.push_back(dist_mem(motor));
			aux.push_back(dist_endPalavra(motor));
		}
		vetor.push_back(aux);
		aux.clear();
	}
	//FOR a ser repetido completo
	bool end = false, same = false;
	for (int a = 0; a<qtFor; a++) {
		if (!end)
		{
			if (a == sequence[0])
			{
				if (a == qtFor - 1)
					vetor[tamFor - 1][0] = -1;
				for (int i = 0; i < tamFor; i++)
					for (int j = 0; j < 7; j++)
						instrucoes << vetor[i][j] << (j == 6 ? "\n" : " ");
				if (sequence.size() > 0)
				{
					sequence.erase(sequence.begin());
					if (sequence.size() == 0)
						end = true;
				}
				same = true;
			}
		}


		if (!same)
		{
			for (int j = 0; j<tamFor; j++)
			{
				if (j == tamFor - 1 && a == qtFor - 1)
					instrucoes << -1;
				else
					instrucoes << dist_opcode(motor);
				for (int add = 0; add < 3; add++)
				{
					instrucoes << " " << dist_mem(motor);
					instrucoes << " " << dist_endPalavra(motor);
				}
				instrucoes << "\n";
			}
		}
		same = false;
	}
	instrucoes.close();
}
