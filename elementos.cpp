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

Taco::Taco(unsigned TacoId, float x, float y, float z)
    : TacoId(TacoId), x(x), y(y), z(z) {}

void Taco::desenhar(){
    float ax = ey*dz - ez*dy;
    float ay = ez*dx - ex*dz;
    float az = ex*dy - ey*dx;
    float seno   = sqrt(ax*ax + ay*ay + az*az);
    float cosseno = ex*dx + ey*dy + ez*dz;

    glPushMatrix();
    glTranslatef(x, y, z);
    if (seno > 0){
        glRotatef(atan2(seno, cosseno) * 180.0f / 3.14f, ax/seno, ay/seno, az/seno);
    }
    else if (cosseno < 0){
        glRotatef(180, 0,1,0);
    }
    glTranslatef(-pontaX, -pontaY, -pontaZ);
    glCallList(TacoId);
    glPopMatrix();
}




//0.03632
//0.095292
//1.50102