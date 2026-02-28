#include <iostream>
#include <exception>
#include "ram.hpp"
#include "cpu.hpp"
#include "programas.hpp"
#include "geradorInstrucoes.hpp"
#include "config.hpp"

using Processador::CPU;
using namespace Config;

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
        gerarInstrucao(REPS, TAMFOR, NUMINST, TAMMEM, "instrucoes.txt");
        gerarInstrucao(REPS, TAMFOR, 100, TAMMEM, "interrup.txt");
        Programas::programaAleatorioRepeticoes(ram, cpu, TamLs, TAMRAM, TAMMEM);
    }
    catch (const std::exception &e)
    {
        cout << "Erro encontrado: " << e.what() << "\n";
    }
    return 0;
}
