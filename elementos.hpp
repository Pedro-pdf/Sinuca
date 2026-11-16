#ifndef ELEMENTOS_H
#define ELEMENTOS_H

#include <algorithm>
#include <cstdio>

class Box{
    public:
        float xmax, xmin, zmax, zmin;
};

class Cacapa{
    public:
        float xCa, zCa;
        float raioCa = 0.058;
};

class Mesa{
    public:
        Mesa(unsigned MesaId);

        void desenhar();

        unsigned MesaId;
        float Ymesa = 0.115898;
        float limitXmax;
        float limitXmin;
        float limitZmax;
        float limitZmin;
        Box boxMesa[6];
        Cacapa cacapaMesa[6];

};

class Bola{
    public:
        Bola(); 
        Bola(unsigned BolaID, float x, float z);

        void desenhar();

        void atualizaPosicao(float h);

        void colisaoParede(Box box);

        bool caindoCacapa(Cacapa ca);

        void inicioQueda(Cacapa ca);

        void Queda(float dt);

        bool estaAtiva();

        unsigned BolaId;
        float x, z;
        float y = 0.115898 + 0.025486;
        float Raio = 0.025486;
        float Vx = 0;
        float Vy = 0;
        float Vz = 0;
        float cx;
        float cz;
        float tempoQueda;
        bool emJogo = true;
        bool caindo = false;
        float rot[16];
};

void ColisaoBolas(Bola* a, Bola* b);

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