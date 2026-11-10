#ifndef HUMANO_H
#define HUMANO_H
#include <string>
#include <iostream>

struct vec3{
    float x,y,z;
    vec3(float x, float y, float z) : x(x), y(y), z(z){}
};

struct vec2{
    float u,v;
    vec2(float u, float v) : u(u), v(v){}
};

enum Poligono{TRIANG=3, QUAD=4};

struct face{
    Poligono tipo;
    int vertice[4];
    int textura[4];
    int normal[4];
    int mat;
    face(Poligono tipo_, int v1, int v2, int v3, int v4, 
        int t1, int t2, int t3, int t4,
        int n1, int n2, int n3, int n4){
        vertice[0] = v1; vertice[1] = v2;
        vertice[2] = v3; vertice[3] = v4;
        textura[0] = t1; textura[1] = t2;
        textura[2] = t3; textura[3] = t4;
        normal[0] = n1; normal[1] = n2;
        normal[2] = n3; normal[3] = n4;
        tipo = tipo_;
        mat = -1;
    }
};

namespace ObjLoader{
    vec3 getVertice(std::string s);
    vec3 getNormal(std::string s);
    vec2 getTexCoord(std::string s);
    face getFace(Poligono tipo_, std::string s);
    void loadObg(unsigned& id, const char* objPath, const char* mtlPath = "");
}

#endif
