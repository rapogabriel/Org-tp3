#ifndef MMU_HPP
#define MMU_HPP

#include "ram.hpp"
#include "cpu.hpp" 
#include "BlocoMemoria.hpp"
#include <memory>

class mmu
{
public:
    static bloco_memoria::BlocoMemoria buscarNasMemorias(
        bloco_memoria::Endereco add, 
        Memoria::RAM &ram, 
        std::unique_ptr<Processador::cache[]> &caches, 
        Processador::CPU &cpu
    );
    static void drenarCache(
        std::unique_ptr<Processador::cache[]> &caches,
        Memoria::RAM &ram);

private:
   
    static void promoverBloco(
        bloco_memoria::BlocoMemoria bloco, 
        int nivelOrigem, 
        std::unique_ptr<Processador::cache[]> &caches, 
        Memoria::RAM &ram
    );

   
    static void escreverRetorno(
        bloco_memoria::BlocoMemoria vitima, 
        int nivelDestino, 
        std::unique_ptr<Processador::cache[]> &caches, 
        Memoria::RAM &ram
    );

    static int encontrarVitima(
        int indiceConjunto, 
        Processador::cache &cacheNivel
    );
};

#endif
