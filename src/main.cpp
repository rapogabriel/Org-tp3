#include <iostream>
#include <exception>
#include "ram.hpp"
#include "cpu.hpp"
#include "programas.hpp"
#include "geradorInstrucoes.hpp"

using Memoria::RAM;
using Processador::CPU;
using namespace Programas;

#define TAMC1 64
#define TAMC2 128
#define TAMC3 256
#define REPS 90
#define TAMFOR 10
#define NUMINST 10000
#define TAMMEM 1000

int main()
{
    try
    {
        RAM ram;
        CPU cpu;
        std::vector<int> TamLs;
        TamLs.push_back(TAMC1);
        TamLs.push_back(TAMC2);
        TamLs.push_back(TAMC3);
        gerarInstrucao(REPS, TAMFOR, NUMINST, TAMMEM);
        programaAleatorioRepeticoes(ram, cpu, TamLs);
    }
    catch (const std::exception &e)
    {
        cout << "Erro encontrado: " << e.what() << "\n";
    }
    return 0;
}
