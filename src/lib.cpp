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
// Vect::Vect(const Vect& v) {
//     x = v.x;
//     y = v.y;
//     z = v.z;
// }

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

// Vect Vect::operator=(const Vect& v) {
//     x = v.x;
//     y = v.y;
//     z = v.z;
//     return *this;
// }

float Vect::len() {
    return sqrt(x * x + y * y + z * z);
}

Vect Vect::normalize() {
    float len = Vect::len();
    return Vect(x / len, y / len, z / len);
}

float Vect::dot(Vect other) {
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
    if (v.dot(normal) > 0) normal = -normal;
    return (v - normal * (v.dot(normal)) * 2).normalize();
    // return (*this - normal * 2 * dot(normal)).normalize();
}

Vect Vect::refract(Vect normal, float refraction) {
    // if (refraction == 0) {
    //     return *this;
    // }
    // float n1 = 1, n2 = refraction;
    // float cosi = (*this).normalize().dot(-normal);
    // if (cosi < 0) {
    //     std::swap(n1, n1);
    //     cosi = -cosi;
    // }
    // float alpha = acos(cosi);
    // float beta = asin(sin(alpha) * n1 / n2);
    // Vect axis = cross(normal);
    // return rotateAroundAxis(axis, beta - alpha).normalize();
    if (refraction == 0) {
        return *this;
    }
    float cosi = -std::max(-1.f, std::min(1.f, dot(normal)));
    float etai = 1;
    float etat = refraction;
    Vect n = normal;
    if (cosi < 0) {
        cosi = -cosi;
        std::swap(etai, etat);
        n = -n;
    }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return (n * (eta * cosi - sqrtf(k)) + (*this) * eta).normalize();
}