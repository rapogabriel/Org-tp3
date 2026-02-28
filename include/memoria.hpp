#ifndef MEMORIA_HPP
#define MEMORIA_HPP

#include <memory>
#include <BlocoMemoria.hpp>

using std::unique_ptr;
using bloco_memoria::BlocoMemoria;

class Memoria{
    protected:
        unique_ptr<BlocoMemoria[]> memoria;
        unique_ptr<int[]> ponteiroRelogio;
        int tamanho, conjuntos;
    public:
        int hit, miss;
        Memoria();
        virtual ~Memoria();
        bool ehVazio();
        void inicializar(int tam);
        void imprimir();
        int getTamanho();
        int& acessoPonteiro(int pos);
        virtual BlocoMemoria& acessoDado(int pos);
        int vitima(int indiceConjunto);
        const int& getConjuntos() const;
        float getHitRate();
        float getMissRate();
};
#endif