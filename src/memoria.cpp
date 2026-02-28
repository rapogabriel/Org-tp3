#include <iostream>
#include <stdexcept>

#include "memoria.hpp"
#include "BlocoMemoria.hpp"
#include "config.hpp"

using Config::VIAS;

Memoria::Memoria(): tamanho(0), conjuntos(0), hit(0), miss(0){}

bool Memoria::ehVazio(){
    return tamanho == 0;
}

void Memoria::inicializar(int tam){
    if(tam < 1) return;
    memoria = std::make_unique<bloco_memoria::BlocoMemoria[]>(tam);
    ponteiroRelogio = std::make_unique<int[]>(tam);
    for(int i = 0; i < tam; ++i)
        ponteiroRelogio[i] = 0;
    conjuntos = tam / VIAS;
    tamanho = tam;
}

void Memoria::imprimir(){
    if(!ehVazio())
        for(int i = 0; i < tamanho; ++i)
            std::cout << memoria[i] << "\n";
    else
        std::cout << "Memoria Vazia!\n";
}

int Memoria::getTamanho(){ return tamanho; }

int& Memoria::acessoPonteiro(int pos){
    if(pos > -1 && pos < tamanho)
        return ponteiroRelogio[pos];
    throw std::out_of_range("Indice de memória fora dos limites! (ponteiro)");
}

BlocoMemoria& Memoria::acessoDado(int pos){
    if(pos > -1 && pos < tamanho)
        return memoria[pos];
    throw std::out_of_range("Indice de memória do dado fora dos limites!\npos =" + std::to_string(pos) + "\ntamanho = " + std::to_string(tamanho) + "\nConjuntos = " + std::to_string(conjuntos));
}

int Memoria::vitima(int indiceConjunto){
    int &ponteiro = ponteiroRelogio[indiceConjunto];
    int inicio = indiceConjunto * VIAS;

    while (true)
    {
        int index = inicio + (ponteiro % VIAS);
        auto &linha = memoria[index];

        // Se a linha estiver vazia, ela é usada imediatamente
        if (linha.endBloco == -1)
        {
            ++ponteiro;
            return index;
        }

        // Se o bit de uso estiver ativo, dou uma segunda chance
        if (linha.bitUso)
        {
            linha.bitUso = false;
            ++ponteiro;
        }
        else
        {
            // Caso contrário, essa linha é escolhida como vítima
            ++ponteiro;
            return index;
        }
    }
}

const int& Memoria::getConjuntos() const{
    return conjuntos;
}

float Memoria::getHitRate(){
    return hit > 0 ? (float)hit / (hit + miss) : 0.0f;
}

float Memoria::getMissRate(){
    return miss > 0 ? (float)miss / (hit + miss) : 0.0f;
}

Memoria::~Memoria(){}
