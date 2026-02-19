#include <random>
#include <chrono>
#include <memory>
#include <limits>
#include <iostream>
#include "ram.hpp"
#include "BlocoMemoria.hpp"
using namespace std;
namespace Memoria{
    RAM::RAM() : tamanhoVetor(0){}
    void RAM::criarRAM(int tamanho){
        memoria = make_unique<bloco_memoria::BlocoMemoria[]>(tamanho);
        tamanhoVetor = tamanho;
    }
    bool RAM::ehVazia(){
        if(!tamanhoVetor)
            return 1;
        return 0;
    }
    int RAM::getTamanho(){
        return tamanhoVetor;
    }
    void RAM::criarRAM_vazia(int tamanho){
        criarRAM(tamanho);
        for(int i = 0; i < tamanhoVetor; ++i){
            memoria[i].endBloco = i;
            for(int j = 0; j < 4; ++j){
                memoria[i].palavras[j] = 0;
            }
        }
    }
    void RAM::criarRAM_aleatoria(int tamanho){
        criarRAM(tamanho);
        auto tempo = chrono::high_resolution_clock::now().time_since_epoch().count();
        mt19937_64 motor(tempo);
        uniform_int_distribution<> dist(numeric_limits<int>::min(), numeric_limits<int>::max());
        for(int i = 0; i < tamanhoVetor; ++i){
            memoria[i].endBloco = i;
            for(int j = 0; j < 4; ++j){
                memoria[i].palavras[j] = dist(motor);
            }
        }
    }
    void RAM::setDado(int endereco, bloco_memoria::BlocoMemoria conteudo){
        memoria[endereco] = conteudo;
    }
    bloco_memoria::BlocoMemoria RAM::getDado(int endereco) const{
        return memoria[endereco];
    }
    void RAM::imprimir(){
        cout << "Conteudo da RAM" << endl;
        int i = 0;
        for(; i < tamanhoVetor - 1; ++i)
            cout << memoria[i] << "," << flush;
        cout << memoria[i] << endl;
    }
}