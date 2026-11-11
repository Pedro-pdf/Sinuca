#include "loader.hpp"
#include "elementos.hpp"
#include <GL/glut.h>

int tamJanelaX = 1024;
int tamJanelaY = 728;

const float CAM_X = 0.0f, CAM_Y = 2.0f, CAM_Z = 2.0f;
const float ALVO_X = 0.0f, ALVO_Y = 0.0f, ALVO_Z = 0.0f;

Mesa* mesa = nullptr;

void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Inicializa(void)
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    

    GLfloat ambiente[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat difusa[]   = { 0.9f, 0.9f, 0.9f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa);

    glShadeModel(GL_SMOOTH);
}

void Desenha(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(CAM_X, CAM_Y, CAM_Z,
              ALVO_X, ALVO_Y, ALVO_Z,
              0.0f, 1.0f, 0.0f);

    GLfloat posLuz[] = { 2.0f, 6.0f, 3.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, posLuz);

    // Mesa
    glPushMatrix();
    mesa->desenhar();
    glPopMatrix();

    glutSwapBuffers();
}

void idle() {
    glutPostRedisplay();
}

void teclado(unsigned char tecla, int x, int y) {
    switch (tecla) {
        case 27:
            exit(0);
            break;
    }
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(tamJanelaX, tamJanelaY);
    glutCreateWindow("Sinuca");

    Inicializa();

    mesa = new Mesa(0);
    ObjLoader::loadObg(mesa->MesaId, "Mesa.obj", "Mesa.mtl");

    glutDisplayFunc(Desenha);
    glutReshapeFunc(AlteraTamanhoJanela);
    glutIdleFunc(idle);
    glutKeyboardFunc(teclado);

    glutMainLoop();
    return 0;
}