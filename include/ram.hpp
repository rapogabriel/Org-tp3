#ifndef RAM_HPP
#define RAM_HPP
#include <memory>
#include "BlocoMemoria.hpp"
#include "memoria.hpp"
using namespace std;
class RAM : public Memoria{
    BlocoMemoria& acessoDado(int pos) override;
};

#endif