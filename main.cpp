#include "loader.hpp"
#include "elementos.hpp"
#include <math.h>
#include <vector>
#include <GL/glut.h>
#include <cstdio>
#include <algorithm>

GLdouble matModelo[16], matProj[16];
GLint viewport[4];

float offX = 0, offZ = 0;
float ultimoValidoX, ultimoValidoZ;

int tamJanelaX = 1024;
int tamJanelaY = 728;

float CAM_X = 0.0f, CAM_Y = 0.0f, CAM_Z = 0.0f;
float ALVO_X = 0.0f, ALVO_Y = 0.115898f, ALVO_Z = 0.0f;

float raio = 2.0;
float phy = 70.0;
float theta = 0.0;
int limitSPhy = 80;
int limitBPhy = 30;

float raioMin = 0.5f;
float raioMax = 3.0f;

Mesa* mesa = nullptr;
Bola* bolaBranca = nullptr;
Taco* taco = nullptr;
Bola bolas[15];
unsigned int cenarioId;

enum Estado { CAMERA_LIVRE, MIRANDO, FORCA, IMPACTO, BOLA_EM_MOVIMENTO, POSICIONANDO };
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

int tempoAnterior = 0;
float acumulador = 0.0f;
float escalaTempo = 0.5f;

void reiniciaJogo(void){
    estado = CAMERA_LIVRE;
    int contador = 0;
    float esp = sqrt(3)*bolaBranca->Raio + 0.001f;
    for(int fileira = 0; fileira<5; fileira++){
        for(int i = 0; i <= fileira; i++){
            float x = -0.5 - esp*fileira;
            float z = (i - fileira / 2.0f) * 2.0 * bolaBranca->Raio;
            bolas[contador].x = x;
            bolas[contador].z = z;
            bolas[contador].y = bolaBranca->Raio + mesa->Ymesa;
            bolas[contador].caindo = false;
            bolas[contador].emJogo = true;
            bolas[contador].Vx = 0.0;
            bolas[contador].Vz = 0.0;
            
            contador++;
        }
    }
    bolaBranca->caindo = false;
    bolaBranca->emJogo = true;
    bolaBranca->Vx = 0.0;
    bolaBranca->Vz = 0.0;
    bolaBranca->x = 0.5;
    bolaBranca->z = 0.0;
    bolaBranca->y = bolaBranca->Raio + mesa->Ymesa;
}

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

void criarMatrizSombra(float matrizSombra[16], float plano[4], float posLuz[4]) {
    float dot = plano[0] * posLuz[0] +
                plano[1] * posLuz[1] +
                plano[2] * posLuz[2] +
                plano[3] * posLuz[3];

    matrizSombra[0]  = dot - posLuz[0] * plano[0];
    matrizSombra[4]  = 0.0f - posLuz[0] * plano[1];
    matrizSombra[8]  = 0.0f - posLuz[0] * plano[2];
    matrizSombra[12] = 0.0f - posLuz[0] * plano[3];

    matrizSombra[1]  = 0.0f - posLuz[1] * plano[0];
    matrizSombra[5]  = dot - posLuz[1] * plano[1];
    matrizSombra[9]  = 0.0f - posLuz[1] * plano[2];
    matrizSombra[13] = 0.0f - posLuz[1] * plano[3];

    matrizSombra[2]  = 0.0f - posLuz[2] * plano[0];
    matrizSombra[6]  = 0.0f - posLuz[2] * plano[1];
    matrizSombra[10] = dot - posLuz[2] * plano[2];
    matrizSombra[14] = 0.0f - posLuz[2] * plano[3];

    matrizSombra[3]  = 0.0f - posLuz[3] * plano[0];
    matrizSombra[7]  = 0.0f - posLuz[3] * plano[1];
    matrizSombra[11] = 0.0f - posLuz[3] * plano[2];
    matrizSombra[15] = dot - posLuz[3] * plano[3];
}

void Desenha(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    atualizarCamera();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(CAM_X, CAM_Y, CAM_Z,
              ALVO_X, ALVO_Y, ALVO_Z,
              0.0f, 1.0f, 0.0f);

    GLfloat posLuz[] = { 0.0f, 1.5f, 0.0f, 1.0f };

    float planoChao[4] = { 0.0f, 1.0f, 0.0f, -1*mesa->Ymesa };
    float matrizSombra[16];
    
    glLightfv(GL_LIGHT0, GL_POSITION, posLuz);
    
    glGetDoublev(GL_MODELVIEW_MATRIX, matModelo);
    glGetDoublev(GL_PROJECTION_MATRIX, matProj);
    glGetIntegerv(GL_VIEWPORT, viewport);


    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    mesa->desenhar();

    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);

    glPushMatrix();
        criarMatrizSombra(matrizSombra, planoChao, posLuz);
        glMultMatrixf(matrizSombra);
        if(!bolaBranca->caindo){
            bolaBranca->desenhar();
        }
        for(int i = 0; i<15; i++){
            if(bolas[i].emJogo){
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
    glPopMatrix();

    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_LIGHTING);

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
    glPushMatrix();
    glCallList(cenarioId);
    glPopMatrix();
    
    glutSwapBuffers();
}

bool clicouNaBola(int mx, int my) {
    double yTela = viewport[3] - my;

    double x0, y0, z0, x1, y1, z1;
    gluUnProject(mx, yTela, 0.0, matModelo, matProj, viewport, &x0, &y0, &z0);
    gluUnProject(mx, yTela, 1.0, matModelo, matProj, viewport, &x1, &y1, &z1);

    double dx = x1 - x0, dy = y1 - y0, dz = z1 - z0;
    double len = sqrt(dx*dx + dy*dy + dz*dz);
    dx /= len;  dy /= len;  dz /= len;

    double ox = x0 - bolaBranca->x;
    double oy = y0 - bolaBranca->y;
    double oz = z0 - bolaBranca->z;
    double r  = bolaBranca->Raio;

    double b = ox*dx + oy*dy + oz*dz;
    double c = ox*ox + oy*oy + oz*oz - r*r;
    double disc = b*b - c;

    if(disc >= 0){
        glutSetCursor(GLUT_CURSOR_INFO);
        return true;
    }
    return false;
}

bool cliqueNaMesa(int mx, int my, float& px, float& pz) {
    double yTela = viewport[3] - my;
    double x0, y0, z0, x1, y1, z1;
    gluUnProject(mx, yTela, 0.0, matModelo, matProj, viewport, &x0, &y0, &z0);
    gluUnProject(mx, yTela, 1.0, matModelo, matProj, viewport, &x1, &y1, &z1);

    double dx = x1 - x0, dy = y1 - y0, dz = z1 - z0;
    if (fabs(dy) < 1e-9) return false;

    double Yp = mesa->Ymesa + bolaBranca->Raio;
    double tt = (Yp - y0) / dy;
    if (tt < 0) return false;

    px = x0 + tt * dx;
    pz = z0 + tt * dz;
    return true;
}

bool posicaoValida(float px, float pz) {

    if(px > mesa->limitXmax || px < mesa->limitXmin){
        return false;
    }

    if(pz > mesa->limitZmax || pz < mesa->limitZmin){
        return false;
    }

    for (int k = 0; k < 6; k++) {
        Box& b = mesa->boxMesa[k];
        float cx = std::max(b.xmin, std::min(px, b.xmax));
        float cz = std::max(b.zmin, std::min(pz, b.zmax));
        float dx = px - cx, dz = pz - cz;
        if (dx*dx + dz*dz < bolaBranca->Raio * bolaBranca->Raio) return false;
    }

    for (int k = 0; k < 6; k++) {
        float dx = px - mesa->cacapaMesa[k].xCa, dz = pz - mesa->cacapaMesa[k].zCa;
        float r = mesa->cacapaMesa[k].raioCa;
        if (dx*dx + dz*dz < r*r) return false;
    }

    for (int i = 0; i < 15; i++) {
        if (!bolas[i].emJogo) continue;
        float dx = px - bolas[i].x, dz = pz - bolas[i].z;
        float s = 2.0f * bolaBranca->Raio;
        if (dx*dx + dz*dz < s*s) return false;
    }
    return true;
}

void iniciarArrasto(int mx, int my) {
    float px, pz;

    if(!cliqueNaMesa(mx, my, px, pz)){
        return;
    }

    offX = bolaBranca->x - px;
    offZ = bolaBranca->z - pz;
    ultimoValidoX = bolaBranca->x;
    ultimoValidoZ = bolaBranca->z;
    estado = POSICIONANDO;
}

void arrastar(int mx, int my) {
    if(estado != POSICIONANDO){
        return;
    }

    float px, pz;

    if (!cliqueNaMesa(mx, my, px, pz)){
        return;
    }

    float nx = px + offX, nz = pz + offZ;
    if (posicaoValida(nx, nz)) {
        bolaBranca->x = nx;
        bolaBranca->z = nz;
        ultimoValidoX = nx;
        ultimoValidoZ = nz;
    }
    glutPostRedisplay();
}

void teclado(unsigned char tecla, int x, int y) {
    switch (tecla) {
        case 27:
            reiniciaJogo();
            break;

        case 13:
            if(estado == CAMERA_LIVRE){
                phy = 70;
                estado = MIRANDO;
                raio = 0.5;
            }
            else if(estado == MIRANDO){
                estado = CAMERA_LIVRE;
                raio = 1.0;
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
                theta += 2;
                if(theta >= 360){
                    theta -= 360;
                }
                break;
            }

        case 'd':
            if(estado != FORCA){
                theta -= 2;
                if(theta < 0.0){
                    theta += 360;
                }
                break;
            }

    }

    glutPostRedisplay();
}

void tecladoEspecial(int tecla, int x, int y) {
    switch (tecla) {
        case GLUT_KEY_UP:
            if(estado != FORCA){
                phy -= 0.5;
                if(phy < limitBPhy){
                    phy = limitBPhy;
                }
                break;
            }

        case GLUT_KEY_DOWN:
            if(estado != FORCA){
                phy += 0.5;
                if(phy > limitSPhy){
                    phy = limitSPhy;
                }
                break;
            }

        case GLUT_KEY_LEFT:
            if(estado != FORCA){
                theta += 0.5;
                if(theta >= 360){
                    theta -= 360;
                }
                break;
            }

        case GLUT_KEY_RIGHT:
            if(estado != FORCA){
                theta -= 0.5;
                if(theta < 0.0){
                    theta += 360;
                }
                break;
            }
    }

    glutPostRedisplay();
}

void scroll(int botao, int status, int x, int y) {
    if(botao == GLUT_LEFT_BUTTON){
        if(status == GLUT_DOWN){
            if(estado == POSICIONANDO){
                estado = CAMERA_LIVRE;
                glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            }
            else if(estado == CAMERA_LIVRE && clicouNaBola(x, y)){
                iniciarArrasto(x, y);
            }
        }
        return;
    }
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
    int agora = glutGet(GLUT_ELAPSED_TIME);
    float real = (agora - tempoAnterior) / 1000.0f;
    tempoAnterior = agora;
    if(real > 0.1f){
        real = 0.1f;
    }

    acumulador += real * escalaTempo;
    while(acumulador >= dt){
        switch(estado){
            case POSICIONANDO:
                limitSPhy = 80;
                limitBPhy = 30;
                raioMin = 0.5f;
                raioMax = 3.0f;
                break;
    
            case CAMERA_LIVRE:
                limitSPhy = 80;
                limitBPhy = 30;
                raioMin = 0.5f;
                raioMax = 3.0f;
                ALVO_X = 0.0;
                ALVO_Y = 0.115898;
                ALVO_Z = 0.0;
                break;
    
            case MIRANDO:
                raioMax = 1.5;
                limitSPhy = 70;
                limitBPhy = 40;
                ALVO_X = bolaBranca->x;
                ALVO_Y = bolaBranca->y;
                ALVO_Z = bolaBranca->z;
                break;
    
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
                        estado = POSICIONANDO;
                        glutSetCursor(GLUT_CURSOR_INFO);
                        offX = 0;
                        offZ = 0;
                        ultimoValidoX = bolaBranca->x;
                        ultimoValidoZ = bolaBranca->z;
                        bolaBranca->emJogo = true;
                        bolaBranca->y = mesa->Ymesa + bolaBranca->Raio;
                    }
                    else{
                        estado = CAMERA_LIVRE;
                    }
                }
                break;
    
    
        }
        acumulador -= dt;
    }

    glutPostRedisplay();
    glutTimerFunc(1, atualizaCena, 0);
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

    ObjLoader::loadObg(cenarioId, "cenario.obj", "cenario.mtl");


    glutDisplayFunc(Desenha);
    glutReshapeFunc(AlteraTamanhoJanela);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(tecladoEspecial);
    glutMouseFunc(scroll);
    glutPassiveMotionFunc(arrastar);
    tempoAnterior = glutGet(GLUT_ELAPSED_TIME);
    glutTimerFunc(1, atualizaCena, 0);

    glutMainLoop();
    return 0;
}
