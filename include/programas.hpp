#ifndef PROGRAMAS_HPP
#define PROGRAMAS_HPP

#include "ram.hpp"

// Declaração antecipada para evitar inclusão circular
namespace Processador
{
    class CPU;
}

using Memoria::RAM;
// Usamos o Processador::CPU apenas como referência aqui
using Processador::CPU;

namespace Programas
{
    void programaAleatorio(RAM &ram, CPU &cpu, int memoria, int qtdInstrucoes);
    void programaAleatorioRepeticoes(RAM &ram, CPU &cpu, const vector<int>& TamLs);
}

#endif