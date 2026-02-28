#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string_view>

namespace Config{
    constexpr int TAMC1 = 64;
    constexpr int TAMC2 = 128;
    constexpr int TAMC3 = 256;
    constexpr int TAMRAM = 1024;
    constexpr int TAMMEM = 4096;

    constexpr int REPS = 90;
    constexpr int TAMFOR = 10;
    constexpr int NUMINST = 10000;

    constexpr int VIAS = 4;

    constexpr int CUSTO_L1  = 10;
    constexpr int CUSTO_L2  = 110;
    constexpr int CUSTO_L3  = 1110;
    constexpr int CUSTO_RAM = 101110;
    constexpr int CUSTO_SECUNDARIA = 1101110;

    constexpr int MAXINTER = 10000;
    constexpr int PORCENTOBASE = 5;
    constexpr int PORCENTOSIMULTANEO = 1;
    constexpr int MAXSIMULTANEOS = 4;

    constexpr std::string_view nomeArquivo = "dados.bin";
}
#endif