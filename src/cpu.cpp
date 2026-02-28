#include <iostream>
#include <memory>
#include <iomanip>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "ram.hpp"
#include "instrucao.hpp"
#include "cpu.hpp"
#include "mmu.hpp"

using namespace std;
using INSTRUCAO::Instrucao;

#define Programa vector<reference_wrapper<Instrucao>>

#define MAXINTER 10000 // Quantidade maxima de interrupcoes que pode acontecer

// Porcentagem de acontecer uma interrupcao apos uma instrucao
#define PORCENTOBASE 5 // Durante o programa principal
#define PORCENTOSIMULTANEO 1 // Durante outra interrupcao

#define MAXSIMULTANEOS 5 // Quantidade maxima de programas na pilha de execucao

namespace Processador
{
    using enum numInst;

    void cache::iniciarCache(int tam)
    {
        memoria = make_unique<bloco_memoria::BlocoMemoria[]>(tam);
        tamanho = tam;

        // Inicializa linhas
        for (int i = 0; i < tam; ++i)
        {
            memoria[i].endBloco = -1;
            memoria[i].bitUso = false;
            memoria[i].atualizado = false;
            memoria[i].cacheHit = 0;
            memoria[i].custo = 0;
        }

        // CLOCK por conjunto (Second Chance)
        int sets = tam / 4; // VIAS = 4
        ponteiroRelogio = make_unique<int[]>(sets);
        for (int i = 0; i < sets; i++)
            ponteiroRelogio[i] = 0;
    }

    CPU::CPU() : custo(0), tempoExecucao(0.0), cache(make_unique<Processador::cache[]>(3))
    {
        hit[0] = hit[1] = hit[2] = 0;
        miss[0] = miss[1] = miss[2] = 0;
        pilhaPC.push_back(0);
    }

    void CPU::setPrograma(vector<reference_wrapper<Instrucao>> programaAux)
    {
        programa = programaAux;
    }

    void CPU::setInterrup(vector<reference_wrapper<Instrucao>> programaAux)
    {
        interrup = programaAux;
    }

    // RELATÓRIO FINAL
    void CPU::imprimirResumo()
    {
        int totalL1 = hit[0] + miss[0];
        int totalL2 = hit[1] + miss[1];
        int totalL3 = hit[2] + miss[2];

        float taxL1_H = (totalL1 > 0) ? (float)hit[0] / totalL1 * 100 : 0.0f;
        float taxL1_M = (totalL1 > 0) ? (float)miss[0] / totalL1 * 100 : 0.0f;

        float taxL2_H = (totalL2 > 0) ? (float)hit[1] / totalL2 * 100 : 0.0f;
        float taxL2_M = (totalL2 > 0) ? (float)miss[1] / totalL2 * 100 : 0.0f;

        float taxL3_H = (totalL3 > 0) ? (float)hit[2] / totalL3 * 100 : 0.0f;
        float taxL3_M = (totalL3 > 0) ? (float)miss[2] / totalL3 * 100 : 0.0f;

        const int W_NOME = 10;
        const int W_NUM = 12;
        const int W_TAXA = 14;

        cout << "\n==========================================================================================\n";
        cout << "                               RELATORIO FINAL DE EXECUCAO                                \n";
        cout << "==========================================================================================\n";

        cout << left << setw(W_NOME) << "Memoria"
             << right << setw(W_NUM) << "Tamanho"
             << right << setw(W_NUM) << "Hits"
             << right << setw(W_NUM) << "Misses"
             << right << setw(W_NUM) << "Total"
             << right << setw(W_TAXA) << "Taxa Hit(%)"
             << right << setw(W_TAXA) << "Taxa Miss(%)"
             << "\n";

        cout << "------------------------------------------------------------------------------------------\n";

        // L1
        cout << left << setw(W_NOME) << "Cache L1"
             << right << setw(W_NUM) << cache[0].tamanho
             << right << setw(W_NUM) << hit[0]
             << right << setw(W_NUM) << miss[0]
             << right << setw(W_NUM) << totalL1
             << fixed << setprecision(2)
             << right << setw(W_TAXA - 2) << taxL1_H << "% "
             << right << setw(W_TAXA - 2) << taxL1_M << "% "
             << "\n";

        // L2
        cout << left << setw(W_NOME) << "Cache L2"
             << right << setw(W_NUM) << cache[1].tamanho
             << right << setw(W_NUM) << hit[1]
             << right << setw(W_NUM) << miss[1]
             << right << setw(W_NUM) << totalL2
             << right << setw(W_TAXA - 2) << taxL2_H << "% "
             << right << setw(W_TAXA - 2) << taxL2_M << "% "
             << "\n";

        // L3
        cout << left << setw(W_NOME) << "Cache L3"
             << right << setw(W_NUM) << cache[2].tamanho
             << right << setw(W_NUM) << hit[2]
             << right << setw(W_NUM) << miss[2]
             << right << setw(W_NUM) << totalL3
             << right << setw(W_TAXA - 2) << taxL3_H << "% "
             << right << setw(W_TAXA - 2) << taxL3_M << "% "
             << "\n";

        cout << "------------------------------------------------------------------------------------------\n";
        cout << "\n[RESUMO DE CUSTO E TEMPO]\n";
        cout << "  -> Custo Total (Ciclos):   " << custo << "\n";
        cout << "  -> Tempo Real de Execucao: " << fixed << setprecision(6) << tempoExecucao << " segundos\n";
        cout << "  -> Quantidade de interrupcoes: " << inter << "\n";
        cout << "==========================================================================================\n";
    }

    void CPU::iniciar(Memoria::RAM &ram, int tamC1, int tamC2, int tamC3)
    {
        // Inicia caches
        cache[0].iniciarCache(tamC1);
        cache[1].iniciarCache(tamC2);
        cache[2].iniciarCache(tamC3);

        // Reset contadores
        hit[0] = hit[1] = hit[2] = 0;
        miss[0] = miss[1] = miss[2] = 0;
        custo = 0;

        inter = 0;

        bool pularIncremento;

        auto inicioRelogio = chrono::high_resolution_clock::now();

        while (!pilhaPC.empty()) {
            reference_wrapper<Programa> programaAtual = pilhaPC.size() > 1 ? std::ref(interrup) : std::ref(programa);
            Instrucao& inst = programaAtual.get()[pilhaPC.back()].get();
            pularIncremento = false;

            if(pilhaPC.back())
            if (inst.opcode != HALT)
            {
                registrador1 = mmu::buscarNasMemorias(inst.add1, ram, cache, *this);
                registrador2 = mmu::buscarNasMemorias(inst.add2, ram, cache, *this);
                registrador3 = mmu::buscarNasMemorias(inst.add3, ram, cache, *this);
            }

            switch (inst.opcode)
            {
            case HALT:
            {
                pilhaPC.pop_back();
                pularIncremento = true;
                if(pilhaPC.size() == 0){
                    cout << "-> \nDRENANDO CACHE\n";
                    mmu::drenarCache(cache, ram);
                    cout << "\n PROCESSAMENTO FINALIZADO \n";

                    auto fimRelogio = chrono::high_resolution_clock::now();
                    chrono::duration<double> duracao = fimRelogio - inicioRelogio;
                    tempoExecucao = duracao.count();
                    imprimirResumo();
                    break;
                }
                else{
                    cout << "FINALIZANDO INTERRUPCAO..." << endl;
                    break;
                }
            }

            case SOMA:
                registrador3.palavras[inst.add3.endPalavra] =
                    registrador1.palavras[inst.add1.endPalavra] +
                    registrador2.palavras[inst.add2.endPalavra];

                registrador3.atualizado = true;
                custo += registrador1.custo + registrador2.custo + registrador3.custo;

                cout << "\nInstrucao " << pilhaPC.back() << ": SOMA\nCusto acumulado: " << custo << endl;
                break;

            case SUB:
                registrador3.palavras[inst.add3.endPalavra] =
                    registrador1.palavras[inst.add1.endPalavra] -
                    registrador2.palavras[inst.add2.endPalavra];

                registrador3.atualizado = true;
                custo += registrador1.custo + registrador2.custo + registrador3.custo;

                cout << "\nInstrucao " << pilhaPC.back() << ": SUB\nCusto acumulado: " << custo << endl;
                break;
            }
            if(!pilhaPC.empty()){
                if(!pularIncremento)
                    ++pilhaPC.back();
                if(inter < MAXINTER && ((rand() % 100) < (pilhaPC.size() == 1 ? PORCENTOBASE : pilhaPC.size() < MAXSIMULTANEOS ? PORCENTOSIMULTANEO : 0))){
                    pilhaPC.push_back(0);
                    cout << "LIDANDO COM INTERRUPCAO " << pilhaPC.size()-1 <<"..." << endl;
                    ++inter;
                    //this_thread::sleep_for(chrono::seconds(1));
                }
            }
        }
    }

}
