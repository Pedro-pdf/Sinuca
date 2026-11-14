#include "loader.hpp"
#include "elementos.hpp"
#include <math.h>
#include <GL/glut.h>

Mesa::Mesa(unsigned MesaId) : MesaId(MesaId) {
    boxMesa[0] = {1.20978, 1.08186, 0.485328, -0.48294};
    boxMesa[1] = {-1.08197, -1.08197, 0.485328, -0.48294};
    boxMesa[2] = {1.02645, 0.057337, -0.542077, -0.669951};
    boxMesa[3] = {1.02645, 0.057337, 0.672339, 0.544464};
    boxMesa[4] = {-0.057452, -1.02656, -0.542077, -0.669951};
    boxMesa[5] = {-0.057452, -1.02656, 0.672339, 0.544464};
}

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