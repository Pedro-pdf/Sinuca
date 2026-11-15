#include "loader.hpp"
#include "elementos.hpp"
#include <math.h>
#include <vector>
#include <GL/glut.h>

int tamJanelaX = 1024;
int tamJanelaY = 728;

float CAM_X = 0.0f, CAM_Y = 0.0f, CAM_Z = 0.0f;
float ALVO_X = 0.0f, ALVO_Y = 0.115898f, ALVO_Z = 0.0f;

float raio = 2.0;
int phy = 70;
int theta = 0;
int limitSPhy = 80;
int limitBPhy = 30;

float raioMin = 0.5f;
float raioMax = 3.0f;

Mesa* mesa = nullptr;
Bola* bolaBranca = nullptr;
Taco* taco = nullptr;
Bola bolas[15];

enum Estado { CAMERA_LIVRE, MIRANDO, FORCA, IMPACTO, BOLA_EM_MOVIMENTO };
Estado estado = CAMERA_LIVRE;

float distanciaTaco = 0.15;
float distanciaTMax = 0.3;
float distanciaTMin = 0.15;
int sentido = 1.0;

float t;
float velocidade;
float velHorizontal;

float dt = 1.0f/60.0f;
float SUBSTEPS = 10.0;
float h = dt/SUBSTEPS;

vec3 posiInic(0,0,0);

void configuraVisualizacao(void){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(CAM_X, CAM_Y, CAM_Z,
              ALVO_X, ALVO_Y, ALVO_Z,
              0.0f, 1.0f, 0.0f);
}

void atualizarCamera() {
    CAM_X = ALVO_X + sin((phy*3.14)/180)*cos((theta*3.14)/180)*raio;
    CAM_Y = ALVO_Y + cos((phy*3.14)/180)*raio;
    CAM_Z = ALVO_Z + sin((phy*3.14)/180)*sin((theta*3.14)/180)*raio;
}

void zoom(float delta) {
    raio += delta;
    if (raio < raioMin) raio = raioMin;
    if (raio > raioMax) raio = raioMax;
    glutPostRedisplay();
}

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
    for(int i = 0; i<15; i++){
        if(bolas[i].emJogo || bolas[i].caindo){
            bolas[i].desenhar();
        }
    }
    
    if(estado == MIRANDO || estado == FORCA || estado == IMPACTO || distanciaTaco == 0.0){
        float raioTaco = distanciaTaco + bolaBranca->Raio;
        float phyTaco = phy + 8.0f;
        taco->elevTaco = 90 - phyTaco;
        taco->theta = theta;
        taco->x = ALVO_X + sin((phyTaco*3.14)/180)*cos((theta*3.14)/180)*raioTaco;
        taco->y = ALVO_Y + cos((phyTaco*3.14)/180)*raioTaco;
        taco->z = ALVO_Z + sin((phyTaco*3.14)/180)*sin((theta*3.14)/180)*raioTaco;
        taco->desenhar();
    }
    
    glutSwapBuffers();
}

void teclado(unsigned char tecla, int x, int y) {
    switch (tecla) {
        case 27:
            exit(0);
            break;

        case 13:
            if(estado == CAMERA_LIVRE){
                estado = MIRANDO;
                raio = 0.5;
                raioMax = 1.5;
                phy = 70;
                limitSPhy = 70;
                limitBPhy = 40;
                ALVO_X = bolaBranca->x;
                ALVO_Y = bolaBranca->y;
                ALVO_Z = bolaBranca->z;
            }
            else if(estado == MIRANDO){
                estado = CAMERA_LIVRE;
                raio = 1.0;
                raioMax = 3.0;
                limitSPhy = 80;
                limitBPhy = 30;
                ALVO_X = 0.0;
                ALVO_Y = 0.115898;
                ALVO_Z = 0.0;
            }
            else if(estado == FORCA){
                estado = MIRANDO;
            }
            break;

        case ' ':
            if(estado == MIRANDO){
                estado = FORCA;
            }
            else if(estado == FORCA){
                estado = IMPACTO;
                t = (distanciaTaco - 0.15f) / (0.30f - 0.15f);
                velocidade = 0.35f + t * t * (7.0f - 0.35f);
                velHorizontal = velocidade * cos((taco->elevTaco*3.14)/180.0);
                bolaBranca->Vx = velHorizontal * -cos((theta*3.14)/180.0);
                bolaBranca->Vz = velHorizontal * -sin((theta*3.14)/180.0);
            }
            break;

        case 'w':
            if(estado != FORCA){
                phy -= 2;
                if(phy < limitBPhy){
                    phy = limitBPhy;
                }
                break;
            }

        case 's':
            if(estado != FORCA){
                phy += 2;
                if(phy > limitSPhy){
                    phy = limitSPhy;
                }
                break;
            }

        case 'a':
            if(estado != FORCA){
                theta = (theta+2)%360;
                break;
            }

        case 'd':
            if(estado != FORCA){
                theta = (theta-2)%360;
                break;
            }
    }

    glutPostRedisplay();
}

void scroll(int botao, int status, int x, int y) {
    if(status != GLUT_DOWN){
        return;
    }
    if (botao == 3){
        zoom(-0.5f);
    }
    else if(botao == 4){
        zoom(+0.5f);
    }
}

void atualizaCena(int valor){
    switch(estado){
        case FORCA:
            distanciaTaco += 0.01 * sentido;
            if(distanciaTaco > distanciaTMax){
                distanciaTaco = distanciaTMax;
                sentido *= -1;
            }
            else if(distanciaTaco < distanciaTMin){
                distanciaTaco = distanciaTMin;
                sentido *= -1;
            }
            break;

        case IMPACTO:
            distanciaTaco -= 0.1;
            if(distanciaTaco <= 0.0){
                distanciaTaco = 0.0;
                limitSPhy = 80;
                limitBPhy = 30;
                ALVO_X = 0.0;
                ALVO_Y = 0.115898;
                ALVO_Z = 0.0;
                estado = BOLA_EM_MOVIMENTO;
            }
            break;

        case BOLA_EM_MOVIMENTO:
            distanciaTaco = distanciaTMin;
            raio = 2.5;
            phy = 30;
            theta = 90;
            bolaBranca->Queda(dt);
            for(int i = 0; i<15; i++){
                bolas[i].Queda(dt);
            }
            for (int i = 0; i < SUBSTEPS; i++) {
                if(bolaBranca->emJogo){
                    bolaBranca->atualizaPosicao(h);
                    for (int k = 0; k < 6; k++){
                        if(bolaBranca->caindoCacapa(mesa->cacapaMesa[k])){
                            bolaBranca->inicioQueda(mesa->cacapaMesa[k]);
                            break;
                        }
                        bolaBranca->colisaoParede(mesa->boxMesa[k]);
                    }
                }
                for(int i = 0; i<15; i++){
                    if(bolas[i].emJogo){
                        bolas[i].atualizaPosicao(h);
                        for (int k = 0; k < 6; k++){
                            if(bolas[i].caindoCacapa(mesa->cacapaMesa[k])){
                                bolas[i].inicioQueda(mesa->cacapaMesa[k]);
                                break;
                            }
                        }
                        for(int k = 0; k<6; k++){
                            bolas[i].colisaoParede(mesa->boxMesa[k]);
                        }
                    }
                }

                for(int i = 0; i<15; i++){
                    ColisaoBolas(bolaBranca, &bolas[i]);
                }

                for(int i = 0; i<15; i++){
                    for(int j = i+1; j<15; j++){
                        ColisaoBolas(&bolas[i], &bolas[j]);
                    }
                }


            }
            bool movimento = false;
            for(int i = 0; i<15; i++){
                if(bolas[i].estaAtiva() || bolas[i].caindo){
                    movimento = true;
                    break;
                }
            }
            if(bolaBranca->estaAtiva() || bolaBranca->caindo){
                movimento = true;
            }

            if(!movimento){
                if(!bolaBranca->emJogo){
                    bolaBranca->x = posiInic.x;
                    bolaBranca->y = posiInic.y;
                    bolaBranca->z = posiInic.z;
                    bolaBranca->emJogo = true;
                }
                estado = CAMERA_LIVRE;
                limitSPhy = 80;
                limitBPhy = 30;
                raioMax = 3.0;
            }
            break;
            
    }

    glutPostRedisplay();
    glutTimerFunc(16, atualizaCena, 0);
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(tamJanelaX, tamJanelaY);
    glutCreateWindow("Sinuca");

    Inicializa();

    mesa = new Mesa(0);
    ObjLoader::loadObg(mesa->MesaId, "Mesa.obj", "Mesa.mtl");

    bolaBranca = new Bola(0, 0.5, 0.0);
    ObjLoader::loadObg(bolaBranca->BolaId, "BolaB.obj", "BolaB.mtl");
    posiInic.y = bolaBranca->y;
    int contador = 0;
    float esp = sqrt(3)*bolaBranca->Raio + 0.001f;
    for(int fileira = 0; fileira<5; fileira++){
        for(int i = 0; i <= fileira; i++){
            float x = -0.5 - esp*fileira;
            float z = (i - fileira / 2.0f) * 2.0 * bolaBranca->Raio;
            bolas[contador].x = x;
            bolas[contador].z = z;
            contador++;
        }
    }
    ObjLoader::loadObg(bolas[0].BolaId, "Bola1.obj", "bola1.mtl");
    ObjLoader::loadObg(bolas[1].BolaId, "Bola2.obj", "bola2.mtl");
    ObjLoader::loadObg(bolas[2].BolaId, "Bola3.obj", "bola3.mtl");
    ObjLoader::loadObg(bolas[3].BolaId, "Bola4.obj", "bola4.mtl");
    ObjLoader::loadObg(bolas[4].BolaId, "Bola5.obj", "bola5.mtl");
    ObjLoader::loadObg(bolas[5].BolaId, "Bola6.obj", "bola6.mtl");
    ObjLoader::loadObg(bolas[6].BolaId, "Bola7.obj", "bola7.mtl");
    ObjLoader::loadObg(bolas[7].BolaId, "Bola8.obj", "bola8.mtl");
    ObjLoader::loadObg(bolas[8].BolaId, "Bola9.obj", "bola9.mtl");
    ObjLoader::loadObg(bolas[9].BolaId, "Bola10.obj", "bola10.mtl");
    ObjLoader::loadObg(bolas[10].BolaId, "Bola11.obj", "bola11.mtl");
    ObjLoader::loadObg(bolas[11].BolaId, "Bola12.obj", "bola12.mtl");
    ObjLoader::loadObg(bolas[12].BolaId, "Bola13.obj", "bola13.mtl");
    ObjLoader::loadObg(bolas[13].BolaId, "Bola14.obj", "bola14.mtl");
    ObjLoader::loadObg(bolas[14].BolaId, "Bola15.obj", "bola15.mtl");

    taco = new Taco(2, 0.0, 0.5, 0.0);
    ObjLoader::loadObg(taco->TacoId, "Taco.obj", "Taco.mtl");


    glutDisplayFunc(Desenha);
    glutReshapeFunc(AlteraTamanhoJanela);
    glutKeyboardFunc(teclado);
    glutMouseFunc(scroll);
    glutTimerFunc(16, atualizaCena, 0);

    glutMainLoop();
    return 0;
}



//2.36
//0.19397
//1.37