#include "caralho.hpp"
#include <stdlib.h>
#include <GL/glut.h>

int tamJanelaX = 450;
int tamJanelaY = 300;

void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / h, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

unsigned cuboId;

void Inicializa(void)
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);   // fundo escuro
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    ObjLoader::loadObg(cuboId, "Moon.obj", "Moon.mtl");
}

void Desenha(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(3, 3, 5,     // câmera
              0, 0, 0,     // alvo
              0, 1, 0);

    GLfloat posLuz[] = { 2.0f, 5.0f, 3.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, posLuz);

    glCallList(cuboId);                // desenha o cubo

    glutSwapBuffers();
}

int main(int argc, char** argv){

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(5, 5);
    glutInitWindowSize(tamJanelaX, tamJanelaY);
    glutCreateWindow("Teste OBJ");     // janela primeiro

    Inicializa();                      // aqui dentro o loadObg roda

    glutDisplayFunc(Desenha);
    glutReshapeFunc(AlteraTamanhoJanela);
    glutMainLoop();
    return 0;

	return 0;
}