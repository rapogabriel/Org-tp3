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
    throw std::out_of_range("Indice de memória fora dos limites! (dado, RAM)");
}