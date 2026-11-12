#ifndef ELEMENTOS_H
#define ELEMENTOS_H

#include <algorithm>
#include <cstdio>

class Mesa{
    public:
        Mesa(unsigned MesaId);

        void desenhar();

        unsigned MesaId;
        float Ymesa = 0.000036;
};

class Bola{
    public:
        Bola(unsigned BolaID, float x, float z);

        void desenhar();

        unsigned BolaId;
        float x, y, z;
        float Raio = 0.025486;
};




#endif