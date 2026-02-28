#include <vector>
#include <iostream>
#include <functional>
#include <chrono>
#include <thread>
#include <random>

#include "instrucao.hpp"
#include "programas.hpp"
#include "cpu.hpp"
#include "ram.hpp"
#include "enum.hpp"

using namespace std;
using Memoria::RAM;
using Processador::CPU;
using namespace INSTRUCAO;

using enum numInst;

namespace Programas
{
    void programaAleatorioRepeticoes(RAM &ram, CPU &cpu, const vector<int>& TamLs) {
        ram.criarRAM_aleatoria(1000);
        vector<Instrucao> programa = montarInstrucoesProgramaAleatorioRepeticoes("instrucoes.txt");
        vector<Instrucao> interrup = montarInstrucoesProgramaAleatorioRepeticoes("interrup.txt");
        vector<reference_wrapper<Instrucao>> programaAPassar;
        vector<reference_wrapper<Instrucao>> interrupAPassar;

        for (auto &item : programa)
            programaAPassar.push_back(ref(item));
        for (auto &item : interrup)
            interrupAPassar.push_back(ref(item));

        cpu.setPrograma(programaAPassar);
        cpu.setInterrup(interrupAPassar);

        cpu.iniciar(ram, TamLs[0], TamLs[1], TamLs[2]);
    }
    void programaAleatorio(RAM &ram, CPU &cpu, int memoria, int qtdInstrucoes)
    {
        ram.criarRAM_aleatoria(memoria);
        vector<Instrucao> programa = montarInstrucoesProgramaAleatorio(qtdInstrucoes, memoria);
        vector<reference_wrapper<Instrucao>> programaAPassar;

        for (auto &item : programa)
            programaAPassar.push_back(ref(item));

        cpu.setPrograma(programaAPassar);

        int tamL1 = 64;
        int tamL2 = 128;
        int tamL3 = 256;

        cpu.iniciar(ram, tamL1, tamL2, tamL3);
    }
}