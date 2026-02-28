#ifndef FS_HPP
#define FS_HPP

#include "BlocoMemoria.hpp"

class FileSystem{
    public:
        static void criaMemoria_vazia(unsigned int tam);
        static void criaMemoria_Aleatoria(unsigned int tam);
        static bloco_memoria::BlocoMemoria buscaDado(unsigned int endBloco);
        static void setDado(bloco_memoria::BlocoMemoria bloco);
};
#endif