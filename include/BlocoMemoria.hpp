#ifndef BLOCOMEMORIA_HPP
#define BLOCOMEMORIA_HPP

#include <memory>
#include <iostream>

namespace bloco_memoria
{
    class BlocoMemoria
    {
    private:
        void swap(BlocoMemoria &b1, BlocoMemoria &b2) noexcept;

    public:
        std::unique_ptr<int[]> palavras;
        int endBloco;
        bool atualizado;
        int custo;
        int cacheHit;
        bool bitUso; // CLOCK

        BlocoMemoria();
        BlocoMemoria(const BlocoMemoria &original);
        BlocoMemoria operator=(BlocoMemoria original);
    };

    class Endereco
    {
    public:
        int endBloco;
        int endPalavra;
    };

    std::ostream &operator<<(std::ostream &os, const BlocoMemoria &imprimir);
}

#endif
