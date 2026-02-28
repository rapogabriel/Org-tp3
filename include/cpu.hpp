#ifndef CPU_HPP
#define CPU_HPP

#include <vector>
#include <functional>
#include <memory>
#include <chrono>
#include "ram.hpp"
#include "instrucao.hpp"
#include "BlocoMemoria.hpp"
#include "enum.hpp"

using bloco_memoria::BlocoMemoria;
using INSTRUCAO::Instrucao;

namespace Processador
{
    class cache
    {
    public:
        std::unique_ptr<BlocoMemoria[]> memoria;
        int tamanho;

        // CLOCK
        std::unique_ptr<int[]> ponteiroRelogio;
        long long movimentacoes = 0;

        void iniciarCache(int tam);
    };

    class CPU
    {
    private:
        std::vector<std::reference_wrapper<Instrucao>> programa;
        std::vector<std::reference_wrapper<Instrucao>> interrup;
        vector<int> pilhaPC;
        numInst opcode;
        long long custo;
        double tempoExecucao;
        int inter;

    public:
        int hit[3] = {0, 0, 0};
        int miss[3] = {0, 0, 0};

        BlocoMemoria registrador1;
        BlocoMemoria registrador2;
        BlocoMemoria registrador3;

        std::unique_ptr<Processador::cache[]> cache;

        CPU();
        void setPrograma(std::vector<std::reference_wrapper<Instrucao>> programaAux);
        void setInterrup(vector<reference_wrapper<Instrucao>> programaAux);
        void iniciar(Memoria::RAM &ram, int tamC1, int tamC2, int tamC3);
        void imprimirResumo();
    };
}

#endif
