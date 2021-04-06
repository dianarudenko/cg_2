#ifndef MYLIB
#define MYLIB

#define EPS 0.1

struct Color {
    float r;
    float g;
    float b;
    float a;

    Color(float r = 0.9, float g = 0.8, float b = 0.6, float a = 1): r(r), g(g), b(b), a(a) {}
    const Color operator*(float f) const;
    const Color operator/(float f) const;
    Color operator+(const Color c) const;
    Color operator-(const Color c) const;
    Color normalize() const;
};

struct Vect {
    float x;
    float y;
    float z;

    Vect(float x = 0, float y = 0, float z = 0): x(x), y(y), z(z) {}
    // Vect(const Vect& v);
    Vect operator-(const Vect& v) const;
    Vect operator-() const;
    Vect operator+(const Vect& v) const;
    Vect operator*(const float a) const;
    // Vect operator=(const Vect& v);
    float len();
    Vect normalize();
    float dot(Vect other);
    Vect cross(Vect other);
    Vect rotateAroundAxis(Vect axis, float angle);
    Vect reflect(Vect normal);
    Vect refract(Vect normal, float refraction);
};

struct Light {
    Vect pos;
    float intensity;
    // Color color;
    Light() {}
    Light(Vect pos, float intensity) : pos(pos), intensity(intensity) {}
};
#endif