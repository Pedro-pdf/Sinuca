#include "loader.hpp"
#include "elementos.hpp"
#include <GL/glut.h>

Mesa::Mesa(unsigned MesaId) : MesaId(MesaId) {}

void Mesa::desenhar(){
    glPushMatrix();
    glCallList(MesaId);
    glPopMatrix();
}