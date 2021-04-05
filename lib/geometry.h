#include "lib.h"
#include <cmath>

#ifndef MYGEOMETRY
#define MYGEOMETRY
struct Ray {
    Vect pos;
    Vect dir;
    Ray(): pos(), dir() {}
    Ray(Vect pos, Vect dir): pos(pos), dir(dir.normalize()) {}
};

struct Intersection {
    bool hit;
    float dist;
    Vect pos;
    Vect normal;
    Color color;
    Vect reflected;
    Vect refracted;
    // float specular;
    // float absorbtion;
    //bool real;
    Intersection(bool hit = false, float dist = 0.0): hit(hit), dist(dist), pos(), normal(), color(), reflected() {}
    Intersection(bool hit, float dist, Color color): hit(hit), dist(dist), color(color) {}
};

struct Triangle {
    Vect a;
    Vect b;
    Vect c;
    Color color;
    Vect normal;

    Triangle(): a(), b(), c() {}
    Triangle(Vect a, Vect b, Vect c): a(a), b(b), c(c) {}
    Intersection intersect(Ray ray);
};

struct Object {
    // bool real;
    int triangles_number;
    Triangle *triangles;
    Color color;
    float reflection [4];
    float specular;
    float refraction;
    // float absorbtion;
    Object() {
        triangles_number = 0;
        triangles = NULL;
        color = Color();
        reflection[0] = 0;
        reflection[1] = 1;
        reflection[2] = 0;
        reflection[4] = 0;
        specular = 0;
        refraction = 0;
        // absorbtion = 0;
    }
    virtual Intersection intersect(Ray r) = 0;
    virtual ~Object() { delete triangles; };
};

struct Icosahedron: public Object {
    float edge;
    Vect center;
    Vect axis;

    Icosahedron(const float edge, const Vect center, const Vect axis);
    virtual Intersection intersect(Ray ray);
};

struct Sphere: public Object {
    Vect center;
    float radius;
    // float reflection;
    // float specular;

    Sphere(const Vect &c, const float &r) : center(c), radius(r) {}

    Intersection intersect(Ray ray) {
        Intersection res;
        Vect L = center - ray.pos;
        float tca = L.dot(ray.dir);
        float d2 = L.dot(L) - tca*tca;
        if (d2 > radius*radius) return false;
        float thc = sqrtf(radius*radius - d2);
        float t0  = tca - thc;
        float t1 = tca + thc;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) res.hit = false;
        else {
            res.hit = true;
            res.dist = t0;
            res.pos = ray.pos + ray.dir * t0;
            res.normal = (res.pos - center).normalize();
            res.reflected = ray.dir.reflect(res.normal);
            res.color = color;
            res.refracted = ray.dir.refract(res.normal, refraction).normalize();
        }
        return res;
    }
};

#endif