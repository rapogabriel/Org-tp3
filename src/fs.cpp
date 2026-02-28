#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

#include "fs.hpp"
#include "config.hpp"

using Config::nomeArquivo;
using Config::CUSTO_SECUNDARIA;

void FileSystem::criaMemoria_vazia(unsigned int tam){
    std::ofstream arquivo(std::string(nomeArquivo), std::ios::binary);
    int novoTam = tam * 4;
    if(arquivo.is_open()){
        for(int i = 0; i < novoTam; ++i){
            int valor = 0;
            arquivo.write(reinterpret_cast<const char*>(&valor), sizeof(int));
        }
        arquivo.close();
    }
}

void FileSystem::criaMemoria_Aleatoria(unsigned int tam){
    std::ofstream arquivo(std::string(nomeArquivo), std::ios::binary);
    int novoTam = tam * 4;
    auto tempo = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::mt19937_64 motor(tempo);
        std::uniform_int_distribution<> dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    if(arquivo.is_open()){
        for(int i = 0; i < novoTam; ++i){
            int valor = dist(motor);
            arquivo.write(reinterpret_cast<const char*>(&valor), sizeof(int));
        }
        arquivo.close();
    }
}

bloco_memoria::BlocoMemoria FileSystem::buscaDado(unsigned int endBloco){
    std::ifstream arquivo(std::string(nomeArquivo), std::ios::binary);
    bloco_memoria::BlocoMemoria add;
    if(arquivo.is_open()){
        add.endBloco = endBloco;
        add.cacheHit = 5;
        add.custo = CUSTO_SECUNDARIA;
        add.bitUso = true;
        int pos = endBloco * 4;
        std::streampos deslocamento = pos * sizeof(int);
        arquivo.seekg(deslocamento);
        for(int i = 0; i < 4; ++i)
            arquivo.read(reinterpret_cast<char*>(&add.palavras[i]), sizeof(int));
        arquivo.close();
    }
    return add;
}

void FileSystem::setDado(bloco_memoria::BlocoMemoria bloco){
    std::ofstream arquivo(std::string(nomeArquivo), std::ios::binary);
    if(arquivo.is_open()){
        int pos = bloco.endBloco * 4;
        std::streampos deslocamento = pos * sizeof(int);
        arquivo.seekp(deslocamento);
        for(int i = 0; i < 4; ++i)
            arquivo.write(reinterpret_cast<char*>(&bloco.palavras[i]), sizeof(int));
        arquivo.close();
    }
}