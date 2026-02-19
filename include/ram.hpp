#ifndef RAM_HPP
#define RAM_HPP
#include <memory>
#include "BlocoMemoria.hpp"
using namespace std;
namespace Memoria{
    class RAM{
        private:
            unique_ptr<bloco_memoria::BlocoMemoria[]> memoria;
            int tamanhoVetor;
        public:
            RAM();
            void criarRAM(int tamanho);
            bool ehVazia();
            int getTamanho();
            void criarRAM_vazia(int tamanho);
            void criarRAM_aleatoria(int tamanho);
            void setDado(int endereco, bloco_memoria::BlocoMemoria conteudo);
            bloco_memoria::BlocoMemoria getDado(int endereco) const;
            void imprimir();
    };
}
#endif