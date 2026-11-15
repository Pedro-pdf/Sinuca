#include "loader.hpp"
#include "elementos.hpp"
#include <math.h>
#include <GL/glut.h>

float FUNDOCA = 0.03;

Mesa::Mesa(unsigned MesaId) : MesaId(MesaId) {
    boxMesa[0] = {1.20978, 1.08186, 0.485328, -0.48294};
    boxMesa[1] = {-1.08197, -1.08197, 0.485328, -0.48294};
    boxMesa[2] = {1.02645, 0.057337, -0.542077, -0.669951};
    boxMesa[3] = {1.02645, 0.057337, 0.672339, 0.544464};
    boxMesa[4] = {-0.057452, -1.02656, -0.542077, -0.669951};
    boxMesa[5] = {-0.057452, -1.02656, 0.672339, 0.544464};

    cacapaMesa[0] = {1.11006, 0.570423};
    cacapaMesa[1] = {1.11006, -0.570423};
    cacapaMesa[2] = {-1.11006, -0.570423};
    cacapaMesa[3] = {-1.11006, 0.570423};
    cacapaMesa[4] = {-0.000057, -0.587131};
    cacapaMesa[5] = {-0.000057, 0.587131};
}

void Mesa::desenhar(){
    glPushMatrix();
    glCallList(MesaId);
    glPopMatrix();
}

Bola::Bola(unsigned BolaId, float x, float z)
    : BolaId(BolaId), x(x), z(z) {}

Bola::Bola() : Bola(0, 0.0f, 0.0f) {}

void Bola::desenhar(){
    glPushMatrix();
    glTranslatef(x, y, z);
    glCallList(BolaId);
    glPopMatrix();
}

void Bola::atualizaPosicao(float h){
    float velocidade = sqrt(Vx*Vx + Vz*Vz);
    float atrito = 0.3 * 10.0;

    if(velocidade > atrito*h){
        float ax = -(Vx/velocidade)*atrito;
        float az = -(Vz/velocidade)*atrito;

        Vx = Vx + ax*h;
        Vz = Vz + az*h;
        
        x = x + Vx*h;
        z = z+ Vz*h;
    }
    else{
        Vx = 0.0;
        Vz = 0.0;
    }
}

void Bola::colisaoParede(Box box){
    float px = std::max(box.xmin, std::min(x, box.xmax));
    float pz = std::max(box.zmin, std::min(z, box.zmax));

    float dx = x - px;
    float dz = z - pz;
    float d = sqrt(dx*dx + dz*dz);

    if (d > Raio){
        return;
    }

    float nx, nz;
    nx = dx / d; 
    nz = dz / d;
    float e = 0.75;
    
    float pen = Raio - d;
    x += nx * pen;
    z += nz * pen;

    Vx = Vx -(1+e)*(Vx*nx + Vz*nz)*nx;
    Vz = Vz -(1+e)*(Vx*nx + Vz*nz)*nz;
}

bool Bola::caindoCacapa(Cacapa ca){
    float dx = x - ca.xCa;
    float dz = z - ca.zCa;
    float d = sqrt(dx*dx + dz*dz);

    return d <= ca.raioCa;
}

void Bola::inicioQueda(Cacapa ca){
    caindo = true;
    emJogo = false;
    tempoQueda = 0;
    Vy = 0;
    cx = ca.xCa;
    cz = ca.zCa;
    Vx = 0;
    Vz = 0;
}

void Bola::Queda(float dt){
    if(!caindo){
        return;
    }

    tempoQueda += dt;

    if (tempoQueda < 0.15f) {
        float f = dt / 0.15f;
        x += (cx - x) * f;
        z += (cz - z) * f;
    }

    Vy -= 10 * dt;
    y  += Vy * dt;
    if(y <= FUNDOCA){
        y = FUNDOCA;
    }

    if (tempoQueda >= 1.0) caindo = false;

}

bool Bola::estaAtiva(){
    if(Vx == 0.0 && Vz == 0.0){
        return 0;
    }
    return 1;
}

void ColisaoBolas(Bola* a, Bola* b){
    if(!a->emJogo || !b->emJogo){
        return;
    }
    float dx = b->x - a->x;
    float dz = b->z - a->z;
    float d = sqrt(dx*dx + dz*dz);

    float e = 0.95;

    float soma = 2.0*a->Raio;

    if(d > soma){
        return;
    }

    float nx = dx/d;
    float nz = dz/d;

    float Vrel = (b->Vx - a->Vx)*nx + (b->Vz - a->Vz)*nz;
    float k = ((1+e) * Vrel)/2;

    a->Vx = a->Vx + k*nx;
    a->Vz = a->Vz + k*nz;

    b->Vx = b->Vx -k*nx;
    b->Vz = b->Vz -k*nz;

    float pen = soma - d;

    a->x -= (nx * pen)/2.0; 
    a->z -= (nz * pen)/2.0;
    b->x += (nx * pen)/2.0;
    b->z += (nz * pen)/2.0;
}

Taco::Taco(unsigned TacoId, float x, float y, float z)
    : TacoId(TacoId), x(x), y(y), z(z) {}

void Taco::desenhar(){
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(-theta, 0, 1, 0);
    glRotatef(elevTaco, 0, 0, 1);
    glCallList(TacoId);
    glPopMatrix();
}




//0.03632
//0.095292
//1.50102