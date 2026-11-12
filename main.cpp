#include "loader.hpp"
#include "elementos.hpp"
#include <math.h>
#include <GL/glut.h>

int tamJanelaX = 1024;
int tamJanelaY = 728;

float CAM_X = 0.0f, CAM_Y = 0.0f, CAM_Z = 0.0f;
float ALVO_X = 0.0f, ALVO_Y = 0.0f, ALVO_Z = 0.0f;

float raio = 2.0;
int phy = 70;
int theta = 0;
int limitSPhy = 70;
int limitBPhy = 30;

float raioMin = 0.5f;
float raioMax = 3.0f;

void atualizarCamera() {
    CAM_X = sin((phy*3.14)/180)*cos((theta*3.14)/180)*raio;
    CAM_Y = cos((phy*3.14)/180)*raio;
    CAM_Z = sin((phy*3.14)/180)*sin((theta*3.14)/180)*raio;
}

void zoom(float delta) {
    raio += delta;
    if (raio < raioMin) raio = raioMin;
    if (raio > raioMax) raio = raioMax;
    glutPostRedisplay();
}

Mesa* mesa = nullptr;
Bola* bolaBranca = nullptr;


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
    GLfloat especular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    glLightfv(GL_LIGHT0, GL_SPECULAR, especular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa);

    glShadeModel(GL_SMOOTH);
}

void Desenha(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    atualizarCamera();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(CAM_X, CAM_Y, CAM_Z,
              ALVO_X, ALVO_Y, ALVO_Z,
              0.0f, 1.0f, 0.0f);

    GLfloat posLuz[] = { 2.0f, 6.0f, 3.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, posLuz);

    // Mesa
    mesa->desenhar();
    bolaBranca->desenhar();
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
        
        case 'w':
            phy -= 2;
            if(phy < limitBPhy){
                phy = limitBPhy;
            }
            break;

        case 's':
            phy += 2;
            if(phy > limitSPhy){
                phy = limitSPhy;
            }
            break;

        case 'a':
            theta = (theta+2)%360;
            break;

        case 'd':
            theta = (theta-2)%360;
            break;
    }
    glutPostRedisplay();
}

void scroll(int botao, int estado, int x, int y) {
    if(estado != GLUT_DOWN){
        return;
    }
    if (botao == 3){
        zoom(-0.5f);
    }
    else if(botao == 4){
        zoom(+0.5f);
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
    bolaBranca = new Bola(0, 0.0f, 0.0f);
    ObjLoader::loadObg(bolaBranca->BolaId, "BolaB.obj", "BolaB.mtl");
    bolaBranca->y = mesa->Ymesa + bolaBranca->Raio;

    glutDisplayFunc(Desenha);
    glutReshapeFunc(AlteraTamanhoJanela);
    glutKeyboardFunc(teclado);
    glutIdleFunc(idle);
    glutKeyboardFunc(teclado);
    glutMouseFunc(scroll);

    glutMainLoop();
    return 0;
}