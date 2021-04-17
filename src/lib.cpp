#include "lib.h"
#include <iostream>
#include <cmath>

//--------------------------- COLOR ---------------------------//
const Color Color::operator*(float f) const {
    return Color(r * f, g * f, b * f, a);
}

const Color Color::operator/(float f) const {
    return Color(r / f, g / f, b / f, a);
}

Color Color::operator+(const Color c) const {
    return Color(r * a + c.r * c.a, g * a + c.g * c.a, b * a + c.b * c.a, (a + c.a) / 2) / (a + c.a);
}

Color Color::operator-(const Color c) const {
    return Color(r * a - c.r * c.a, g * a - c.g * c.a, b * a - c.b * c.a, a);
}

Color Color::normalize() const {
    return Color(1/r, 1/g, 1/b, 1/a);
}


//--------------------------- VECTOR ---------------------------//

Vect Vect::operator-(const Vect& v) const {
    return Vect(x - v.x, y - v.y, z - v.z);
}

Vect Vect::operator-() const {
    return Vect(-x, -y, -z);
}

Vect Vect::operator+(const Vect& v) const {
    return Vect(x + v.x, y + v.y, z + v.z);
}

Vect Vect::operator*(const float a) const {
    return Vect(x * a, y * a, z * a);
}

bool Vect::operator==(const Vect& v) const {
    return (x == v.x) && (y == v.y) && (z == v.z);
}

float Vect::len() {
    return sqrt(x * x + y * y + z * z);
}

Vect Vect::normalize() {
    float len = Vect::len();
    return Vect(x / len, y / len, z / len);
}

float Vect::dot(const Vect other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vect Vect::cross(Vect other) {
    float cross_x = y * other.z - z * other.y;
    float cross_y = z * other.x - x * other.z;
    float cross_z = x * other.y - y * other.x;
    return Vect(cross_x, cross_y, cross_z);
}

Vect Vect::rotateAroundAxis(Vect axis, float angle) {
    Vect res;
    axis = axis.normalize();
    res = *this * cos(angle) + axis.cross(*this) * sin(angle) + axis * axis.dot(*this) * (1 - cos(angle));
    return res;
}

Vect Vect::reflect(Vect normal) {
    Vect v = (*this).normalize();
    return (v - normal * (v.dot(normal)) * 2).normalize();
}

Vect Vect::refract(Vect normal, float refraction, float other_side) {
    if (refraction == 0) {
        return *this;
    }
    float cosi = -std::max(-1.f, std::min(1.f, dot(normal)));
    float etai = int(other_side * 1000) / 1000;
    float etat = refraction;
    Vect n = normal;
    if (cosi < 0) {
        cosi = -cosi;
        std::swap(etai, etat);
        n = -n;
    }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    if (k < -0.001) {
        throw (int)-1;
    }
    return (n * (eta * cosi - sqrtf(k)) + (*this) * eta).normalize();
}

//--------------------------- MATRIX ---------------------------//
    
Vect Matrix::operator*(const Vect& v) const {
    return Vect(v1.dot(v), v2.dot(v), v3.dot(v));
}