#include <memory>
#include "BlocoMemoria.hpp"

namespace bloco_memoria
{
    BlocoMemoria::BlocoMemoria()
        : palavras(std::make_unique<int[]>(4)),
          endBloco(-1),
          atualizado(false),
          custo(0),
          cacheHit(0),
          bitUso(false) {}

    BlocoMemoria::BlocoMemoria(const BlocoMemoria &original)
    {
        palavras = std::make_unique<int[]>(4);
        for (int i = 0; i < 4; ++i)
            palavras[i] = original.palavras[i];

        atualizado = original.atualizado;
        cacheHit = original.cacheHit;
        custo = original.custo;
        endBloco = original.endBloco;
        bitUso = original.bitUso;
    }

    void BlocoMemoria::swap(BlocoMemoria &b1, BlocoMemoria &b2) noexcept
    {
        using std::swap;
        swap(b1.palavras, b2.palavras);
        swap(b1.atualizado, b2.atualizado);
        swap(b1.cacheHit, b2.cacheHit);
        swap(b1.custo, b2.custo);
        swap(b1.endBloco, b2.endBloco);
        swap(b1.bitUso, b2.bitUso);
    }

    BlocoMemoria BlocoMemoria::operator=(BlocoMemoria original)
    {
        swap(*this, original);
        return *this;
    }

    std::ostream &operator<<(std::ostream &os, const BlocoMemoria &imprimir)
    {
        for(int i = 0; i < 4; ++i)
            os << imprimir.palavras[i] << " ";
        return os;
    }
}
