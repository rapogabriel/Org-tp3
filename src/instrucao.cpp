#include "instrucao.hpp"
#include "cpu.hpp"
#include <vector>
#include <random>
#include <chrono>
#include <functional>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace Processador;

namespace INSTRUCAO
{
    vector<Instrucao> montarInstrucoesProgramaAleatorioRepeticoes(std::string arq)
    {
        // Crio o vetor programa reservando 10000 instruções
        vector<Instrucao> programa;
        programa.reserve(10000);
        // abrir arquivo
        ifstream instrucoes;
        instrucoes.open(arq);
        if (!instrucoes.is_open()) {
            throw std::runtime_error("Erro ao abrir o arquivo!\n");
        }
        // linha a ser lida
        string linha;
        while (getline(instrucoes, linha)) {
            cout << linha << endl;
            // depois lê parte a parte da linha, separada por ' '
            stringstream ss(linha);
            string valor;
            // vetor para armazenar os valores de cada parte, como se fosse um split
            vector<int> valores;
            while (getline(ss, valor, ' '))
                valores.push_back(stoi(valor));
            Instrucao tmp;
            tmp.opcode = static_cast<numInst> (valores[0]);
            tmp.add1.endBloco=valores[1];
            tmp.add1.endPalavra=valores[2];

            tmp.add2.endBloco=valores[3];
            tmp.add2.endPalavra=valores[4];

            tmp.add3.endBloco=valores[5];
            tmp.add3.endPalavra=valores[6];

            programa.push_back(tmp);
        }
        instrucoes.close();
        //std::filesystem::remove(ARQ);
        return programa;
    }
    vector<Instrucao> montarInstrucoesProgramaAleatorio(int quantInstrucoes, int tamanhoMemoria)
    {

        // Crio o vetor programa
        vector<Instrucao> programa;
        // reservo 1 espaço de 99 com um halt
        programa.reserve(quantInstrucoes);
        // usa o tempo como semente para criacão de sequencias diferentes
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        mt19937 motor(seed);
        // define as distribuições 0 ou 1
        uniform_int_distribution<> dist_opcode(0, 1);
        // Endereço entre 0 a tamanho da memoria - 1
        uniform_int_distribution<> dist_endBloco(0, tamanhoMemoria - 1);
        uniform_int_distribution<> dist_endPalavra(0, 3);
        for (int i = 0; i < quantInstrucoes - 1; i++)
        {
            Instrucao temporaria;
            // geração do opcode e dos enderecos de memoria
            temporaria.opcode = static_cast<numInst> (dist_opcode(motor));

            temporaria.add1.endBloco = dist_endBloco(motor);
            temporaria.add1.endPalavra = dist_endPalavra(motor);
            temporaria.add2.endBloco = dist_endBloco(motor);
            temporaria.add2.endPalavra = dist_endPalavra(motor);
            temporaria.add3.endBloco = dist_endBloco(motor);
            temporaria.add3.endPalavra = dist_endPalavra(motor);

            programa.push_back(temporaria);
        }
        // Defino a ultima instrução como parada do programa
        Instrucao ultimaInstrucao;
        ultimaInstrucao.opcode = numInst::HALT;
        programa.push_back(ultimaInstrucao);
        return programa;
    }
}

