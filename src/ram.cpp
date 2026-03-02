#include <random>
#include <chrono>
#include <memory>
#include <limits>
#include <iostream>
#include "ram.hpp"
#include "BlocoMemoria.hpp"
#include "fs.hpp"
#include "memoria.hpp"

using bloco_memoria::BlocoMemoria;

BlocoMemoria& RAM::acessoDado(int pos){
    if(pos > -1 && pos < tamanho)
        return memoria[pos];
    throw std::out_of_range("RAM (dado): Indice de memória fora dos limites!\npos =" + std::to_string(pos) + "\ntamanho = " + std::to_string(tamanho) + "\nconjuntos = " + std::to_string(conjuntos));
}

int& RAM::acessoPonteiro(int pos){
    if(pos > -1 && pos < tamanho)
        return ponteiroRelogio[pos];
    throw std::out_of_range("RAM (ponteiro): Indice de memória fora dos limites!\npos =" + std::to_string(pos) + "\ntamanho = " + std::to_string(tamanho) + "\nconjuntos = " + std::to_string(conjuntos));
}