#include "mmu.hpp"
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace Processador;
using namespace bloco_memoria; 

static const int VIAS = 4;

// Implementação da política Clock (Second Chance)
int mmu::encontrarVitima(int indiceConjunto, Processador::cache &cacheNivel)
{
    // Ponteiro do relógio associado ao conjunto
    int &ponteiro = cacheNivel.ponteiroRelogio[indiceConjunto];
    int inicio = indiceConjunto * VIAS;

    while (true)
    {
        int index = inicio + (ponteiro % VIAS);
        auto &linha = cacheNivel.memoria[index];

        // Se a linha estiver vazia, ela é usada imediatamente
        if (linha.endBloco == -1)
        {
            ponteiro++;
            return index;
        }

        // Se o bit de uso estiver ativo, dou uma segunda chance
        if (linha.bitUso)
        {
            linha.bitUso = false;
            ponteiro++;
        }
        else
        {
            // Caso contrário, essa linha é escolhida como vítima
            ponteiro++;
            return index;
        }
    }
}

// Função responsável por descer um bloco na hierarquia de memória
void mmu::escreverRetorno(BlocoMemoria vitima, int nivelDestino, 
                          unique_ptr<Processador::cache[]> &caches, Memoria::RAM &ram)
{
    // Se passar do último nível de cache, escreve na RAM
    if (nivelDestino > 3)
    {
        if (vitima.atualizado)
        {
            ram.setDado(vitima.endBloco, vitima);
        }
        return;
    }

    Processador::cache &cacheDestino = caches[nivelDestino - 1];
    
    int sets = cacheDestino.tamanho / VIAS;
    int indiceConjunto = vitima.endBloco % sets;

    // Escolhe onde o bloco vai entrar na cache inferior
    int idx = encontrarVitima(indiceConjunto, cacheDestino);
    BlocoMemoria anterior = cacheDestino.memoria[idx];

    // Ao descer de nível, o bloco entra sem proteção (bitUso = 0)
    vitima.bitUso = false;
    cacheDestino.memoria[idx] = vitima;

    // Se havia um bloco nesse local, ele continua descendo
    if (anterior.endBloco != -1)
    {
        escreverRetorno(anterior, nivelDestino + 1, caches, ram);
    }
}

// Promove um bloco subindo nível por nível até chegar na L1
void mmu::promoverBloco(BlocoMemoria bloco, int nivelOrigem, 
                        unique_ptr<Processador::cache[]> &caches, Memoria::RAM &ram)
{
    BlocoMemoria blocoSubindo = bloco;

    // O loop garante que o bloco suba gradualmente
    for (int nivelAtual = nivelOrigem; nivelAtual > 1; nivelAtual--)
    {
        int nivelAlvo = nivelAtual - 1;
        Processador::cache &cacheAlvo = caches[nivelAlvo - 1];

        int sets = cacheAlvo.tamanho / VIAS;
        int indiceConjunto = blocoSubindo.endBloco % sets;

        int idx = encontrarVitima(indiceConjunto, cacheAlvo);
        BlocoMemoria vitimaDoAlvo = cacheAlvo.memoria[idx];

        // Na L1 aplico BIP; nos outros níveis o bloco entra protegido
        if (nivelAlvo == 1)
        {
            blocoSubindo.bitUso = ((rand() % 100) < 10);
        }
        else
        {
            blocoSubindo.bitUso = true;
        }

        cacheAlvo.memoria[idx] = blocoSubindo;

        // Se houve substituição, o bloco removido desce na hierarquia
        if (vitimaDoAlvo.endBloco != -1)
        {
            escreverRetorno(vitimaDoAlvo, nivelAtual, caches, ram);
        }
    }
}

// Função principal de busca do bloco na hierarquia
BlocoMemoria mmu::buscarNasMemorias(Endereco add, Memoria::RAM &ram, 
                                    unique_ptr<Processador::cache[]> &caches, Processador::CPU &cpu)
{
    int enderecoBloco = add.endBloco;

    const int CUSTO_L1  = 10;
    const int CUSTO_L2  = 110;
    const int CUSTO_L3  = 1110;
    const int CUSTO_RAM = 1001110;

    // Busca na L1
    Processador::cache &l1 = caches[0];
    int setsL1 = l1.tamanho / VIAS;
    int inicioL1 = (enderecoBloco % setsL1) * VIAS;

    for (int i = inicioL1; i < inicioL1 + VIAS; i++)
    {
        if (l1.memoria[i].endBloco == enderecoBloco)
        {
            l1.memoria[i].bitUso = true;
            l1.memoria[i].custo = CUSTO_L1;
            cpu.hit[0]++;
            return l1.memoria[i];
        }
    }
    cpu.miss[0]++;

    // Busca na L2
    Processador::cache &l2 = caches[1];
    int setsL2 = l2.tamanho / VIAS;
    int inicioL2 = (enderecoBloco % setsL2) * VIAS;

    for (int i = inicioL2; i < inicioL2 + VIAS; i++)
    {
        if (l2.memoria[i].endBloco == enderecoBloco)
        {
            BlocoMemoria bloco = l2.memoria[i];

            // Remove da L2 para manter exclusividade
            l2.memoria[i].endBloco = -1;
            l2.memoria[i].bitUso = false;

            bloco.custo = CUSTO_L2;
            cpu.hit[1]++;

            promoverBloco(bloco, 2, caches, ram);
            return bloco;
        }
    }
    cpu.miss[1]++;

    // Busca na L3
    Processador::cache &l3 = caches[2];
    int setsL3 = l3.tamanho / VIAS;
    int inicioL3 = (enderecoBloco % setsL3) * VIAS;

    for (int i = inicioL3; i < inicioL3 + VIAS; i++)
    {
        if (l3.memoria[i].endBloco == enderecoBloco)
        {
            BlocoMemoria bloco = l3.memoria[i];

            l3.memoria[i].endBloco = -1;
            l3.memoria[i].bitUso = false;

            bloco.custo = CUSTO_L3;
            cpu.hit[2]++;

            promoverBloco(bloco, 3, caches, ram);
            return bloco;
        }
    }
    cpu.miss[2]++;

    // Miss total: acesso à RAM
    BlocoMemoria blocoRam = ram.getDado(enderecoBloco);
    blocoRam.custo = CUSTO_RAM;
    blocoRam.bitUso = true;
    blocoRam.atualizado = false;

    promoverBloco(blocoRam, 4, caches, ram);
    return blocoRam;
}
void mmu::drenarCache(unique_ptr<Processador::cache[]> &caches, Memoria::RAM &ram)
{
    // Percorre todos os níveis de cache (L1, L2 e L3)
    for (int nivel = 0; nivel < 3; nivel++)
    {
        // Cache correspondente ao nível atual
        Processador::cache &cacheAtual = caches[nivel];

        // Percorre todas as posições da cache usando o tamanho definido dinamicamente
        for (int i = 0; i < cacheAtual.tamanho; i++)
        {
            // Bloco armazenado na posição atual da cache
            BlocoMemoria &bloco = cacheAtual.memoria[i];

            // Se o bloco existe (endBloco != -1) e foi modificado,
            // então ele precisa ser escrito de volta na RAM
            if (bloco.endBloco != -1 && bloco.atualizado)
            {
                // Write-back: grava o conteúdo atualizado do bloco na memória principal
                ram.setDado(bloco.endBloco, bloco);

                // Após salvar na RAM, o bloco deixa de estar "sujo"
                bloco.atualizado = false;
            }
        }
    }
}
