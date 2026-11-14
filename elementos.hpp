#ifndef ELEMENTOS_H
#define ELEMENTOS_H

#include <algorithm>
#include <cstdio>

class Box{
    public:
        float xmax, xmin, zmax, zmin;
};

class Mesa{
    public:
        Mesa(unsigned MesaId);

        void desenhar();

        unsigned MesaId;
        float Ymesa = 0.115898;
        Box boxMesa[6];

};

class Bola{
    public:
        Bola(unsigned BolaID, float x, float z);

        void desenhar();

        void atualizaPosicao(float h);

        void colisaoParede(Box box);

        bool estaAtiva();

        unsigned BolaId;
        float x, y, z;
        float Raio = 0.025486;
        float Vx = 0;
        float Vz = 0;
        bool ativa = 0;
};

class Taco{
    public:
        Taco(unsigned TacoId, float x, float y, float z);

        void desenhar();

        unsigned TacoId;
        float x, y, z;
        float comprimento = 1.50102;
        float elevTaco;
        float theta;
};



#endif