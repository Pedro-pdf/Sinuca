#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "loader.hpp"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <GL/glut.h>

struct Mtl {  
    float ka[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    float kd[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    float ks[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float ns = 0.0f;
    std::string mapKd;
    GLuint texId = 0;
};

std::map<std::string, Mtl> lerMtl(const std::string& caminho){
    std::map<std::string, Mtl> mats;

    std::ifstream arq(caminho);
    if (!arq.is_open()) {
        std::cout << "AVISO: nao abriu " << caminho << "\n";
        return mats;
    }

    std::string linha, atual;
    while (std::getline(arq, linha)) {
        std::istringstream ss(linha);
        std::string tipo;
        ss >> tipo;

        if (tipo == "newmtl") {
            ss >> atual;
            mats[atual] = Mtl();
            continue;
        }
        if (atual.empty()) continue;

        Mtl& m = mats[atual];
        if      (tipo == "Ka") ss >> m.ka[0] >> m.ka[1] >> m.ka[2];
        else if (tipo == "Kd") ss >> m.kd[0] >> m.kd[1] >> m.kd[2];
        else if (tipo == "Ks") ss >> m.ks[0] >> m.ks[1] >> m.ks[2];
        else if (tipo == "Ns") {
            float ns;
            ss >> ns;
            m.ns = std::min(ns, 1000.0f) * 128.0f / 1000.0f;
        }
        else if (tipo == "map_Kd") ss >> m.mapKd;
    }
    return mats;
}

void aplicarMtl(const Mtl& m){
    GLfloat kd[4] = { m.kd[0], m.kd[1], m.kd[2], 1.0f };
    GLfloat ka[4] = { m.kd[0], m.kd[1], m.kd[2], 1.0f };   // ambiente = difuso
    GLfloat ks[4] = { m.ks[0] * 0.2f, m.ks[1] * 0.2f, m.ks[2] * 0.2f, 1.0f };

    float shininess = m.ns * 128.0f / 1000.0f;
    if (shininess < 1.0f)   shininess = 1.0f;
    if (shininess > 128.0f) shininess = 128.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   ka);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   kd);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  ks);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

GLuint carregarTextura(const std::string& caminho, bool avisar){
    int w = 0, h = 0, canais = 0;

    stbi_set_flip_vertically_on_load(1);
    unsigned char* dados = stbi_load(caminho.c_str(), &w, &h, &canais, 4);
    if (!dados) {
        if (avisar)
            std::cout << "AVISO: nao abriu a imagem " << caminho
                      << " (" << stbi_failure_reason() << ")\n";
        return 0;
    }

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, dados);

    stbi_image_free(dados);
    return tex;
}

vec3 ObjLoader::getVertice(std::string s){
    float x,y,z;
    sscanf(s.c_str(), "v %f %f %f", &x, &y, &z);
    vec3 result(x,y,z);
    return result;
}

vec3 ObjLoader::getNormal(std::string s){
    float x,y,z;
    sscanf(s.c_str(), "vn %f %f %f", &x, &y, &z);
    vec3 result(x,y,z);
    return result;
}

vec2 ObjLoader::getTexCoord(std::string s){
    float u = 0, v = 0;
    sscanf(s.c_str(), "vt %f %f", &u, &v);
    vec2 result(u,v);
    return result;
}

face ObjLoader::getFace(Poligono tipo_, std::string s){
    int v[4]  = {0, 0, 0, 0};
    int n[4]  = {0, 0, 0, 0};
    int vt[4] = {0, 0, 0, 0};
    if (tipo_ == Poligono::TRIANG) {
        int r = sscanf(s.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
                       &v[0], &vt[0], &n[0],
                       &v[1], &vt[1], &n[1],
                       &v[2], &vt[2], &n[2]);
        if (r != 9) {
            vt[0] = vt[1] = vt[2] = 0;
            sscanf(s.c_str(), "f %d//%d %d//%d %d//%d",
                   &v[0], &n[0], &v[1], &n[1], &v[2], &n[2]);
        }
    }
    else if (tipo_ == Poligono::QUAD) {
        int r = sscanf(s.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                       &v[0], &vt[0], &n[0],
                       &v[1], &vt[1], &n[1],
                       &v[2], &vt[2], &n[2],
                       &v[3], &vt[3], &n[3]);
        if (r != 12) {
            vt[0] = vt[1] = vt[2] = vt[3] = 0;
            sscanf(s.c_str(), "f %d//%d %d//%d %d//%d %d//%d",
                   &v[0], &n[0], &v[1], &n[1], &v[2], &n[2], &v[3], &n[3]);
        }
    }

    return face(tipo_, v[0], v[1], v[2], v[3], vt[0], vt[1], vt[2], vt[3], n[0], n[1], n[2], n[3]);
}

void ObjLoader::loadObg(unsigned& id, const char* objPath, const char* mtlPath){
    std::vector<vec3> vertices;
    std::vector<vec3> normais;
    std::vector<vec2> texCoords;
    std::vector<face> faces;

    std::map<std::string, Mtl> materiais;
    std::vector<std::string> nomesMat;
    int matAtual = -1;

        if (mtlPath[0] != '\0'){
        materiais = lerMtl(mtlPath);

        for (auto& par : materiais){
            Mtl& m = par.second;
            if (m.mapKd.empty()) continue;

            m.texId = carregarTextura(m.mapKd, true);

            if (m.texId){
                m.kd[0] = m.kd[1] = m.kd[2] = 1.0f;
            }
        }
    }

    std::ifstream arq(objPath);
    if (!arq.is_open()){
        std::cout << "ERRO: nao abriu " << objPath << "\n";
        return;
    }

    std::string line;
    while (std::getline(arq, line)){
        while (!line.empty() && (line.back() == '\r' || line.back() == ' '))
            line.pop_back();

        if (line.rfind("usemtl ", 0) == 0) {
            std::istringstream ss(line);
            std::string t, nome;
            ss >> t >> nome;

            auto it = std::find(nomesMat.begin(), nomesMat.end(), nome);
            if (it == nomesMat.end()) {
                nomesMat.push_back(nome);
                matAtual = (int)nomesMat.size() - 1;
            } else {
                matAtual = (int)(it - nomesMat.begin());
            }
        }
        else if (line.rfind("vt ", 0) == 0) {
            texCoords.push_back(getTexCoord(line));
        }
        else if (line.rfind("vn ", 0) == 0) {
            normais.push_back(getNormal(line));
        }
        else if (line.rfind("v ", 0) == 0) {
            vertices.push_back(getVertice(line));
        }
        else if (line.rfind("f ", 0) == 0) {
            int nSpace = 0;
            for (char c : line)
                if (c == ' ') nSpace++;

            if (nSpace != 3 && nSpace != 4) {
                std::cout << "AVISO: face com " << nSpace
                          << " vertices ignorada (exporte com Triangulate Faces)\n";
                continue;
            }

            face tempFace = getFace((Poligono)nSpace, line);
            tempFace.mat = matAtual;
            faces.push_back(tempFace);
        }
    }

    if (nomesMat.empty() && materiais.size() == 1) {
        nomesMat.push_back(materiais.begin()->first);
        for (auto& f : faces) f.mat = 0;
    }
    else if (nomesMat.empty() && materiais.size() > 1) {
        std::cout << "AVISO: .mtl tem varios materiais, mas o .obj nao tem usemtl\n";
    }

    id = glGenLists(1);
    glNewList(id, GL_COMPILE);
    glPolygonMode(GL_FRONT, GL_FILL);

    int ultimo = -2;
    for (size_t i = 0; i < faces.size(); i++) {
        const face& f = faces[i];

        if (f.mat != ultimo) {
            ultimo = f.mat;
            if (ultimo >= 0) {
                auto it = materiais.find(nomesMat[ultimo]);
                if (it != materiais.end()) {
                    aplicarMtl(it->second);
                    if (it->second.texId) {
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, it->second.texId);
                    } else {
                        glDisable(GL_TEXTURE_2D);
                    }
                }
            } else {
                glDisable(GL_TEXTURE_2D);
            }
        }

        int n = (f.tipo == Poligono::TRIANG) ? 3 : 4;

        bool ok = true, comTex = !texCoords.empty();
        for (int k = 0; k < n; k++) {
            int vi = f.vertice[k] - 1;
            int ni = f.normal[k]  - 1;
            int ti = f.textura[k] - 1;
            if (vi < 0 || vi >= (int)vertices.size() ||
                ni < 0 || ni >= (int)normais.size())
                ok = false;
            if (ti < 0 || ti >= (int)texCoords.size())
                comTex = false;
        }
        if (!ok) continue;

        glBegin(n == 3 ? GL_TRIANGLES : GL_QUADS);
        for (int k = 0; k < n; k++) {
            glNormal3fv(&normais[f.normal[k] - 1].x);
            if (comTex) {
                const vec2& t = texCoords[f.textura[k] - 1];
                glTexCoord2f(t.u, t.v);
            }
            glVertex3fv(&vertices[f.vertice[k] - 1].x);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    glEndList();
}
