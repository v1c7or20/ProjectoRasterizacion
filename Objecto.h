#ifndef LEARNOPENGL_OBJETO_H
#define LEARNOPENGL_OBJETO_H
#include <glad/glad.h>
#include "include/glm/glm.hpp"
#include "include/glm/gtx/string_cast.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"
#include <vector>
#include "include/shader_s.h"
#include "BoundingVolume.h"
using namespace std;
using namespace glm;

class Objeto {
public:
    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec2> textureCoords;
    vec3 color;
    float escala=20;
    vector<GLuint> indices;
    GLuint indices_size;
    float x0, y0, z0, a0, xt, yt, zt;
    vec3 v0;
    float startTime;
    GLuint vao;
    mat4 model;
    bool visible=true;
    BoundingSphere *bs;
    GLint POSITION_ATTRIBUTE=0, NORMAL_ATTRIBUTE=1, TEXCOORD0_ATTRIBUTE=8;
    virtual GLuint setup()=0;
    virtual void display(Shader &sh, float radius_n)=0;
    virtual void actualizarPosicion(float tiempo)=0;
    virtual bool expire(float t1)=0;
    virtual void obtenerBS(){};
    virtual void actualizarBS(){};
    Objeto() {
        bs = new BoundingSphere;
    }
};

class Esfera:public Objeto{
public:
    vec3 centro;
    float radius;
    int slices, stacks;
    Esfera() {
        escala = 0.5;
        centro = vec3(0.0);
    }
    Esfera(vec3 _centro) {
        escala = 0.5;
        centro = _centro;
    }
    Esfera(vec3 _centro, float _radius, int _slices, int _stacks) {
        escala = 0.5;
        centro = _centro;
        radius = _radius;
        slices = _slices;
        stacks = _stacks;
    }
    GLuint setup();

    void display(Shader &sh, float radius_n);
    void actualizarPosicion(float tiempo);
    bool expire(float t1);
    void actualizarBS();
};


#endif //LEARNOPENGL_OBJETO_H