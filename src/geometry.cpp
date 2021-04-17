#include "lib.h"
#include "geometry.h"
#include <cmath>
#include <iostream>
#include <vector>

//--------------------------- TRIANGLE ---------------------------//
Intersection Triangle::intersect(Ray ray) {
    Intersection res;
    Vect ab = b - a;
    Vect ac = c - a;

    Vect cross_1 = ray.dir.cross(ac);
    float dot_1 = ab.dot(cross_1);

    if (abs(dot_1) < EPS) {
        return Intersection();
    }

    float f = 1 / dot_1;
    Vect pa = ray.pos - a;
    float u = f * pa.dot(cross_1);
    if (u < 0.0 || u > 1.0) {
        return Intersection();
    }

    Vect cross_2 = pa.cross(ab);
    float v = f * ray.dir.dot(cross_2);
    if (v < 0.0 || v + u > 1.0) {
        return Intersection();
    }

    float t = f * ac.dot(cross_2);
    if (t > EPS) {
        // std::cout <<".";
        res.hit = true;
        res.dist = t;
        res.pos = ray.pos + ray.dir.normalize() * t;
        res.color = color;
        res.normal = normal;
        res.reflected = ray.dir.reflect(normal);
        return res;
    }
    return Intersection();
}

//--------------------------- PLANE ---------------------------//

Intersection Plane::intersect(Ray ray) {
    Intersection res;
    float t = -(normal.dot(ray.pos) - normal.dot(dot)) / normal.dot(ray.dir);
    if (t < EPS) {
        return res;
    }
    res.hit = true;
    res.dist = t;
    res.pos = ray.pos + ray.dir * t;
    Vect axis1 = normal.cross(Vect(normal.x, -normal.z, normal.y)).normalize();
    if ((-normal.z == normal.y) && (normal.z == normal.y)) {
        axis1 = normal.cross(Vect(0, 0, 1)).normalize();
    }
    Vect axis2 = axis1.cross(normal);
    res.color = ((int)(.05 * res.pos.dot(axis1)) + (int)(.05 * res.pos.dot(axis2))) & 1 ? color1 : color2;
    res.normal = normal;
    res.reflected = ray.dir.reflect(normal);
    return res;
}

//--------------------------- ICOSAHEDRON ---------------------------//

Icosahedron::Icosahedron(float edge, Vect center, Vect axis): edge(edge),
                                                              center(center),
                                                              axis(axis) {
    triangles_number = 20;
    triangles = new Triangle[triangles_number];
    // float incribed_rad = (3 + sqrt(5)) * edge / (4 * sqrt(3));
    float cercumscribed_rad = sqrt(2 * (5 + sqrt(5))) * edge / 4;

    Vect _axis = axis.normalize();
    Vect top = center + _axis * cercumscribed_rad;
    Vect bottom = center - _axis * cercumscribed_rad;
    Vect normal = _axis.cross(Vect(_axis.x, -_axis.z, _axis.y)).normalize();
    normal = normal.rotateAroundAxis(_axis, M_PI / 3);
    if ((-_axis.z == _axis.y) && (_axis.z == _axis.y)) {
        normal = _axis.cross(Vect(0, 0, 1)).normalize();
    }

    float h = sqrt(3) * edge / 2;
    float tmp_rad = sqrt(7 + 2 * sqrt(5)) * edge / 4;
    float angle = (2 * M_PI) / 10;
    Vect * vertices = new Vect [10];
    int up_down = -1;
    for (int i = 0; i < 10; i++) {
        Vect tmp = center + _axis * (up_down * h / 2);
        Vect vertice = tmp + normal.rotateAroundAxis(_axis, angle * i) * tmp_rad;
        vertices[i] = vertice;
        up_down *= -1;
    }
    for (int i = 0; i < 5; i++) {
        triangles[i].a = bottom;
        triangles[i].b = vertices[(i * 2) % 10];
        triangles[i].c = vertices[((i + 1) * 2) % 10];
        Vect ab = triangles[i].b - triangles[i].a;
        Vect ac = triangles[i].c - triangles[i].a;
        triangles[i].normal = ac.cross(ab).normalize();
        triangles[19 - i].a = top;
        triangles[19 - i].b = vertices[(i * 2 + 1) % 10];
        triangles[19 - i].c = vertices[(i * 2 + 3) % 10];
        ab = triangles[19 - i].b - triangles[19 - i].a;
        ac = triangles[19 - i].c - triangles[19 - i].a;
        triangles[19 - i].normal = ab.cross(ac).normalize();
    }
    for (int i = 0; i < 10; i++) {
        triangles[i + 5].a = vertices[i];
        triangles[i + 5].b = vertices[(i + 1) % 10];
        triangles[i + 5].c = vertices[(i + 2) % 10];
        Vect ab = triangles[i + 5].b - triangles[i + 5].a;
        Vect ac = triangles[i + 5].c - triangles[i + 5].a;
        if (i % 2) {
            triangles[i + 5].normal = ab.cross(ac).normalize();
        } else {
            triangles[i + 5].normal = ac.cross(ab).normalize();
        }
    }
    delete vertices;
}

Intersection Icosahedron::intersect(Ray ray) {
    Intersection tmp, res;
    for (int i = 0; i < triangles_number; i++) {
        tmp = triangles[i].intersect(ray);
        if (tmp.hit && !res.hit) {
            res = tmp;
        } else if (tmp.hit && res.dist > tmp.dist) {
            res = tmp;
        }
    }
    res.color = color;
    return res;
}

//--------------------------- CYLINDER ---------------------------//

Intersection Cylinder::intersect(Ray ray) {
    Intersection res;
    Vect upper = center + axis * (height / 2);
    Vect lower = center - axis * (height / 2);
    Vect delta_p = ray.pos - center;

    float a = (ray.dir - axis * ray.dir.dot(axis)).dot(ray.dir - axis * ray.dir.dot(axis));
    float b = 2 * (ray.dir - axis * ray.dir.dot(axis)).dot(delta_p - axis * delta_p.dot(axis));
    float c = (delta_p - axis * delta_p.dot(axis)).dot(delta_p - axis * delta_p.dot(axis)) - rad * rad;

    float dist = -1;
    if (a == 0) {
        if (b == 0) {
            return res;
        }
        dist = -c / b;
        if (dist < EPS) {
            return res;
        }
    } else {
        float d = b * b - 4 * a * c;
        if (d < 0) {
            return res;
        }
        float t1 = (-b + sqrtf(d)) / (2 * a);
        float t2 = (-b - sqrtf(d)) / (2 * a);
        float height1 = abs(axis.dot(ray.pos + ray.dir * t1 - center));
        float height2 = abs(axis.dot(ray.pos + ray.dir * t2 - center));
        if ((t1 > 0) && (axis.dot(ray.pos + ray.dir * t1 - lower) > 0) && (height1 < height / 2)) {
            if (dist < EPS) {
                dist = t1;
            } else {
                dist = std::min(dist, t1);
            }
        }
        if ((t2 > 0) && (axis.dot(ray.pos + ray.dir * t2 - upper) < 0) && (height2 < height / 2)) {
            if (dist < EPS) {
                dist = t2;
            } else {
                dist = std::min(dist, t2);
            }
        }
    }
    Vect pos = ray.pos + ray.dir * dist;
    Vect normal = (pos - (center + axis * axis.dot(pos - center))).normalize();

    float t3 = -(axis.dot(ray.pos) - axis.dot(upper)) / axis.dot(ray.dir);
    if ((t3 > 0) && ((upper - (ray.pos + ray.dir * t3)).len() < rad)) {
        if (dist < EPS) {
            dist = t3;
        } else if (t3 < dist) {
            dist = t3;
            normal = axis;
        }
    }
    float t4 = -(axis.dot(ray.pos) - axis.dot(lower)) / axis.dot(ray.dir);
    if ((t4 > 0) && ((lower - (ray.pos + ray.dir * t4)).len() < rad)) {
        if (dist < EPS) {
            dist = t4;
        } else if (t4 < dist) {
            dist = t4;
            normal = -axis;
        }
    }
    if (dist < EPS) {
        return res;
    }

    res.hit = true;
    res.color = color;
    res.dist = dist;
    res.pos = ray.pos + ray.dir * dist;
    res.normal = normal;
    res.reflected = ray.dir.reflect(res.normal);
    return res;
}

//--------------------------- ELLIPSOID ---------------------------//

Intersection Ellipsoid::intersect(Ray ray) {
    Intersection res;

    Matrix M = Matrix(Vect(1 / a_rad, 0, 0),
                           Vect(0, 1 / b_rad, 0),
                           Vect(0, 0, 1 / c_rad));
    Vect pos = M * ray.pos - M * center;
    Vect dir = M * ray.dir;

    float a = dir.dot(dir);
    float b = 2 * pos.dot(dir);
    float c = pos.dot(pos) - 1;

    float dist = -1;

    if (a == 0) {
        if (b == 0) {
            return res;
        }
        dist = -c / b;
        if (dist < EPS) {
            return res;
        }
    } else {
        float d = b * b - 4 * a * c;
        if (d < 0) {
            return res;
        }
        float t1 = (-b + sqrtf(d)) / (2 * a);
        float t2 = (-b - sqrtf(d)) / (2 * a);
        dist = t1;
        if (dist < EPS) {
            if (t2 < EPS) {
                return res;
            }
            dist = t2;
        } else {
            if (t2 >= EPS)
                dist = std::min(dist, t2);
        }
    }
    res.hit = true;
    res.dist = dist;
    res.pos = ray.pos + ray.dir * dist;
    res.normal = {(res.pos - center).x / a_rad / a_rad,
                  (res.pos - center).y / b_rad / b_rad,
                  (res.pos - center).z / c_rad / c_rad};
    res.normal = res.normal.normalize();
    res.color = color;
    return res;
}