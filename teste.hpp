#ifndef ELEMENTOS_H
#define ELEMENTOS_H
#include "loader.hpp"
#include <algorithm>
#include <cstdio>

class Bola{
    public:
        Bola(vec3 posicao);

        void desenhar();
        void atualizar(float dt);
        void colisaoBorda();
        void colisaoBolas();

        vec3 posicao;
        vec2 velocidade;
        bool ativa;
};

class Taco{
    public:
        Taco();

        void load();
        void desenhar();
        void mirar();
        void tacada();

        bool ativo;
};

class Mesa{
    Mesa();

    void load();
    void desenhar();
};




#endif