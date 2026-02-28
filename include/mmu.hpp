#ifndef MMU_HPP
#define MMU_HPP

#include "ram.hpp"
#include "cpu.hpp"
#include "BlocoMemoria.hpp"
#include "memoria.hpp"
#include <memory>

class mmu
{
public:
    static bloco_memoria::BlocoMemoria buscarNasMemorias(
        bloco_memoria::Endereco add, 
        RAM &ram, 
        std::unique_ptr<Processador::cache[]> &caches
    );
    static void drenar(
        vector<reference_wrapper<Memoria>> &hierarquia
        );

private:
   
    static void promoverBloco(
        bloco_memoria::BlocoMemoria bloco, 
        int nivelOrigem, 
        vector<reference_wrapper<Memoria>>& hierarquia
    );

    static void escreverRetorno(
        bloco_memoria::BlocoMemoria vitima, 
        int nivelDestino, 
        vector<reference_wrapper<Memoria>>& hierarquia
    );
};

#endif
