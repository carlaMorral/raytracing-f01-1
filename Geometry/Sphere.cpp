#include "Sphere.h"
#include "ScaleTG.h"

Sphere::Sphere(vec3 cen, float r, float data) :Object(data) {
    initCenter = cen;
    center = cen;
    radius = r;
    angle = 0;
}

bool Sphere::hit(const Ray& raig, float t_min, float t_max, HitInfo& info) const {
    vec3 oc = raig.initialPoint() - center;
    float a = dot(raig.dirVector(), raig.dirVector());
    float b = dot(oc, raig.dirVector());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    bool hit = false;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min) {
            info.t = temp;
            info.p = raig.pointAtParameter(info.t);
            info.normal = (info.p - center) / radius;
            info.mat_ptr = material.get();
            hit = true;
        }
        if(!hit){
            temp = (-b + sqrt(discriminant)) / a;
            if (temp < t_max && temp > t_min) {
                info.t = temp;
                info.p = raig.pointAtParameter(info.t);
                info.normal = (info.p - center) / radius;
                info.mat_ptr = material.get();
                hit = true;
            }
        }
    }

    //Textura
    if(dynamic_cast<MaterialTextura*>(info.mat_ptr) && hit) {
        vec3 point = info.normal; //Coincideix amb (info.p - center) / radius que es el que volem
        float theta = acos(point.y); // de 0 a PI
        float phi = atan2(point.x, point.z) + PI; // de 0 a 2PI
        phi += angle;
        while(phi > 2*PI){
            phi -= 2*PI;
        }
        float u = phi / (2*PI);
        float v = theta / PI;
        info.uv = vec2(u,v);
    }
    return hit;
}

void Sphere::aplicaTG(shared_ptr<TG> t) {
    if (dynamic_pointer_cast<ScaleTG>(t)) {
    //if(t->getTG()[3][0]==0 & t->getTG()[3][1]==0  & t->getTG()[3][2]==0 &
            //t->getTG()[0][0]==t->getTG()[1][1] & t->getTG()[0][0]==t->getTG()[2][2]){
        radius *= t->getTG()[0][0];
    }else{
    //if (dynamic_pointer_cast<TranslateTG>(t)) {
        vec4 c(center, 1.0);
        c = t->getTG() * c;
        center.x = c.x; center.y = c.y; center.z = c.z;
    }
}

void Sphere::applyAnimation(shared_ptr<CustomAnimation> anim, int nFrame){
    if(dynamic_pointer_cast<EllipseAnimation>(anim)){
        shared_ptr<EllipseAnimation> elAnim = dynamic_pointer_cast<EllipseAnimation>(anim);
        vec3 newPos = elAnim->getPosition(initCenter, nFrame);
        center = vec3(newPos.x, newPos.y, newPos.z);
    }else if(dynamic_pointer_cast<DoubleEllipseAnimation>(anim)){
        shared_ptr<DoubleEllipseAnimation> elAnim = dynamic_pointer_cast<DoubleEllipseAnimation>(anim);
        vec3 newPos = elAnim->getPosition(initCenter, nFrame);
        center = vec3(newPos.x, newPos.y, newPos.z);
    }else if(dynamic_pointer_cast<RotationAnimation>(anim)){
        shared_ptr<RotationAnimation> rotAnim = dynamic_pointer_cast<RotationAnimation>(anim);
        angle = rotAnim->getAngle(nFrame);
    }
}
