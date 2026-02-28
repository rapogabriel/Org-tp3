#include "mmu.hpp"
#include "config.hpp"
#include "memoria.hpp"
#include "fs.hpp"
#include <iostream>
#include <cstdlib>

using namespace Config;
using bloco_memoria::BlocoMemoria;

// Função responsável por descer um bloco na hierarquia de memória
void mmu::escreverRetorno(BlocoMemoria vitima, int nivelDestino, 
                          vector<reference_wrapper<Memoria>>& hierarquia)
{
    // Se passar do último nível de cache, escreve na RAM
    if (nivelDestino > 4)
    {
        if (vitima.atualizado)
            FileSystem::setDado(vitima);
        return;
    }

    Memoria &memoriaDestino = hierarquia[nivelDestino - 1].get();

    int indiceConjunto = vitima.endBloco % memoriaDestino.getConjuntos();

    // Escolhe onde o bloco vai entrar na cache inferior
    int idx = memoriaDestino.vitima(indiceConjunto);
    BlocoMemoria anterior = memoriaDestino.acessoDado(idx);

    // Ao descer de nível, o bloco entra sem proteção (bitUso = 0)
    vitima.bitUso = false;
    memoriaDestino.acessoDado(idx) = vitima;

    // Se havia um bloco nesse local, ele continua descendo
    if (anterior.endBloco != -1)
        escreverRetorno(anterior, nivelDestino + 1, hierarquia);
}

// Promove um bloco subindo nível por nível até chegar na L1
void mmu::promoverBloco(BlocoMemoria bloco, int nivelOrigem, 
                        vector<reference_wrapper<Memoria>>& hierarquia)
{
    BlocoMemoria& blocoSubindo = bloco;

    // O loop garante que o bloco suba gradualmente
    for (int nivelAtual = nivelOrigem; nivelAtual > 1; nivelAtual--)
    {
        int nivelAlvo = nivelAtual - 1;
        Memoria &nivel = hierarquia[nivelAlvo - 1].get();

        int indiceConjunto = blocoSubindo.endBloco % nivel.getConjuntos();

        int idx = nivel.vitima(indiceConjunto);
        BlocoMemoria vitimaDoAlvo = nivel.acessoDado(idx);

        // Na L1 aplico BIP; nos outros níveis o bloco entra protegido
        if (nivelAlvo == 1)
        {
            blocoSubindo.bitUso = ((rand() % 100) < 10);
        }
        else
        {
            blocoSubindo.bitUso = true;
        }

        nivel.acessoDado(idx) = blocoSubindo;

        // Se houve substituição, o bloco removido desce na hierarquia
        if (vitimaDoAlvo.endBloco != -1)
        {
            escreverRetorno(vitimaDoAlvo, nivelAtual, hierarquia);
        }
    }
}

// Função principal de busca do bloco na hierarquia
BlocoMemoria mmu::buscarNasMemorias(bloco_memoria::Endereco add, RAM &ram, 
                                    unique_ptr<Processador::cache[]> &caches)
{
    int enderecoBloco = add.endBloco;

    vector<reference_wrapper<Memoria>> hierarquia;
    for(int i = 0; i < 3; ++i)
        hierarquia.push_back(ref(caches[i]));
    hierarquia.push_back(ref(ram));

    int idxNivel = 0;
    for(auto& nivel : hierarquia){
        int sets = nivel.get().getConjuntos();
        int inicio = (enderecoBloco % sets) * VIAS;

        for (int i = inicio; i < inicio + VIAS; ++i){
            BlocoMemoria& bloco = nivel.get().acessoDado(i);
            if (bloco.endBloco == enderecoBloco)
            {
                bloco.bitUso = true;
                ++nivel.get().hit;
                if(idxNivel == 0){
                    bloco.custo = CUSTO_L1;
                    return nivel.get().acessoDado(i);
                } else{
                    switch (idxNivel){
                        case 1:
                            bloco.custo = CUSTO_L2;
                            break;
                        case 2:
                            bloco.custo = CUSTO_L3;
                            break;
                        case 3:
                            bloco.custo = CUSTO_RAM;
                            break;
                    }
                    promoverBloco(bloco, idxNivel+1, hierarquia);
                    return bloco;
                }
            }
        }
        ++nivel.get().miss;
        ++idxNivel;
    }

    BlocoMemoria bloco = FileSystem::buscaDado(enderecoBloco);
    promoverBloco(bloco, 5, hierarquia);
    return bloco;
}
void mmu::drenar(vector<reference_wrapper<Memoria>> &hierarquia)
{
    // Percorre todos os níveis de cache (L1, L2 e L3)
    for (int nivel = 0; nivel < 4; ++nivel)
    {
        // Cache correspondente ao nível atual
        Memoria &atual = hierarquia[nivel].get();

        // Percorre todas as posições da cache usando o tamanho definido dinamicamente
        for (int i = 0; i < atual.getTamanho(); ++i)
        {
            // Bloco armazenado na posição atual da cache
            BlocoMemoria &bloco = atual.acessoDado(i);

            // Se o bloco existe (endBloco != -1) e foi modificado,
            // então ele precisa ser escrito de volta na RAM
            if (bloco.endBloco != -1 && bloco.atualizado)
            {
                // Write-back: grava o conteúdo atualizado do bloco na memória principal
                FileSystem::setDado(bloco);

                // Após salvar na RAM, o bloco deixa de estar "sujo"
                bloco.atualizado = false;
            }
        }
    }
}
