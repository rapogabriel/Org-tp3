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
#include "config.hpp"

using namespace std;
using INSTRUCAO::Instrucao;
using namespace Config;

#define Programa vector<reference_wrapper<Instrucao>>

namespace Processador
{
    using enum numInst;

    CPU::CPU() : custo(0), tempoExecucao(0.0), cache(make_unique<Processador::cache[]>(3))
    {
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
    void CPU::imprimirResumo(RAM &ram)
    {

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

        // Caches
        for(int i = 0; i < 3; ++i){
            cout << left << setw(W_NOME) << "Cache L" << i+1
                 << right << setw(W_NUM) << cache[i].getTamanho()
                 << right << setw(W_NUM) << cache[i].hit
                 << right << setw(W_NUM) << cache[i].miss
                 << right << setw(W_NUM) << cache[i].hit+cache[i].miss
                 << fixed << setprecision(2)
                 << right << setw(W_TAXA - 2) << cache[i].getHitRate() * 100 << "% "
                 << right << setw(W_TAXA - 2) << cache[i].getMissRate() * 100 << "% "
                 << "\n";
        }

        // RAM
        cout << left << setw(W_NOME+1) << "RAM"
             << right << setw(W_NUM) << ram.getTamanho()
             << right << setw(W_NUM) << ram.hit
             << right << setw(W_NUM) << ram.miss
             << right << setw(W_NUM) << ram.hit+ram.miss
             << right << setw(W_TAXA - 2) << ram.getHitRate() * 100 << "% "
             << right << setw(W_TAXA - 2) << ram.getMissRate() * 100 << "% "
             << "\n";

        cout << "------------------------------------------------------------------------------------------\n";
        cout << "\n[RESUMO DE CUSTO E TEMPO]\n";
        cout << "  -> Custo Total (Ciclos):   " << custo << "\n";
        cout << "  -> Tempo Real de Execucao: " << fixed << setprecision(6) << tempoExecucao << " segundos\n";
        cout << "  -> Quantidade de interrupcoes: " << inter << "\n";
        cout << "==========================================================================================\n";
    }

    void CPU::iniciar(RAM &ram, int tamC1, int tamC2, int tamC3)
    {
        // Inicia caches
        cache[0].inicializar(tamC1);
        cache[1].inicializar(tamC2);
        cache[2].inicializar(tamC3);

        // Reset contadores
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
                registrador1 = mmu::buscarNasMemorias(inst.add1, ram, cache);
                registrador2 = mmu::buscarNasMemorias(inst.add2, ram, cache);
                registrador3 = mmu::buscarNasMemorias(inst.add3, ram, cache);
            }

            switch (inst.opcode)
            {
            case HALT:
            {
                pilhaPC.pop_back();
                pularIncremento = true;
                if(pilhaPC.size() == 0){
                    cout << "-> \nDRENANDO CACHE\n";
                    vector<reference_wrapper<Memoria>> hierarquia;
                    for(int i = 0; i < 3; ++i)
                        hierarquia.push_back(ref(cache[i]));
                    hierarquia.push_back(ref(ram));
                    mmu::drenar(hierarquia);
                    cout << "\n PROCESSAMENTO FINALIZADO \n";

                    auto fimRelogio = chrono::high_resolution_clock::now();
                    chrono::duration<double> duracao = fimRelogio - inicioRelogio;
                    tempoExecucao = duracao.count();
                    imprimirResumo(ram);
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
