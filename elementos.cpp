#include "loader.hpp"
#include "elementos.hpp"
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