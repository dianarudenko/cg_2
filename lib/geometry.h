#include "lib.h"
#include <cmath>
#include <iostream>

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
    int triangles_number;
    Triangle *triangles;
    Color color;
    float reflection [2];
    float specular;
    float refraction;
    float absorbtion;
    float transparency;
    Object() {
        triangles_number = 0;
        triangles = NULL;
        color = Color();
        absorbtion = 1;
        transparency = 0;
        reflection[0] = 0;
        reflection[1] = 0;
        specular = 0;
        refraction = 1;
    }
    virtual Intersection intersect(Ray r) = 0;
};

struct Plane: public Object {
    Vect normal;
    Vect dot;
    Color color1;
    Color color2;

    Plane(Vect normal, Vect dot, Color color1, Color color2): normal(normal),
                                                              dot(dot),
                                                              color1(color1),
                                                              color2(color2) {}
    Intersection intersect(Ray ray);
};

struct Icosahedron: public Object {
    float edge;
    Vect center;
    Vect axis;

    Icosahedron(const float edge, const Vect center, const Vect axis);
    Intersection intersect(Ray ray);
};

struct Cylinder: public Object {
    Vect center;
    float height;
    float rad;
    Vect axis;

    Cylinder(Vect center, float height, float rad, Vect axis): center(center),
                                                               height(height),
                                                               rad(rad),
                                                               axis(axis.normalize()) {};
    Intersection intersect(Ray ray);
};

struct Ellipsoid: public Object {
    Vect center;
    Vect axis_a;
    float a_rad;
    float b_rad;
    float c_rad;

    Ellipsoid(Vect center, float a_rad, float b_rad, float c_rad): center(center),
                                                                   a_rad(a_rad),
                                                                   b_rad(b_rad),
                                                                   c_rad(c_rad) {}
    Intersection intersect(Ray ray);
};

#endif