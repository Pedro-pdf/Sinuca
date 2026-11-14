#include "loader.hpp"
#include "elementos.hpp"
#include <math.h>
#include <GL/glut.h>

Mesa::Mesa(unsigned MesaId) : MesaId(MesaId) {}

void Mesa::desenhar(){
    glPushMatrix();
    glCallList(MesaId);
    glPopMatrix();
}

Bola::Bola(unsigned BolaId, float x, float z)
    : BolaId(BolaId), x(x), y(0), z(z) {}

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

bool Bola::estaAtiva(){
    if(Vx == 0.0 && Vz == 0.0){
        return 0;
    }
    return 1;
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