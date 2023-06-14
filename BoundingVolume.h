
#ifndef E3_OBJETOS_BOUNDINGVOLUME_H
#define E3_OBJETOS_BOUNDINGVOLUME_H
#include "include/glm/glm.hpp"
#include "include/glm/gtx/string_cast.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

using namespace glm;
class BoundingVolume {
public:
    virtual bool intersecta(BoundingVolume &bv)=0;
};

class BoundingSphere : public BoundingVolume{
public:
    vec3 centro; float radio;
    BoundingSphere(vec3 _centro, float _radio){
        centro = _centro; radio = _radio;
    }
    BoundingSphere(){};
    bool intersecta(BoundingVolume &bv){}
    bool intersecta(BoundingSphere &bs){
        auto temp = sqrt(pow(bs.centro.x - centro.x,2) +
                pow(bs.centro.y -centro.y,2) +
                pow(bs.centro.z - centro.z,2));
        auto radsum= abs(bs.radio) + abs(radio);
        return temp <= radsum;
    }
};


#endif //E3_OBJETOS_BOUNDINGVOLUME_H