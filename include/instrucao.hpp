#ifndef INSTRUCAO_HPP
#define INSTRUCAO_HPP
#include <vector>
#include <functional>
#include "enum.hpp"
#include "BlocoMemoria.hpp"
namespace INSTRUCAO{
    class Instrucao{
        public:
            bloco_memoria::Endereco add1;
            bloco_memoria::Endereco add2;
            bloco_memoria::Endereco add3;
            numInst opcode;
    };
    std::vector<Instrucao> montarInstrucoesProgramaAleatorio(int quantInstrucoes, int tamanhoMemoria);
    std::vector<Instrucao> montarInstrucoesProgramaAleatorioRepeticoes(std::string arq);
    
}
#endif