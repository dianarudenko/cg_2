#include <vector>
#include <iostream>
// #include <algorithm>
#include "lib.h"
#include "geometry.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define SIZE 512

// Vect refract(Vect &I, const Vect &N, const float &refractive_index) { // Snell's law
//     float cosi = - std::max(-1.f, std::min(1.f, I.dot(N)));
//     float etai = 1, etat = refractive_index;
//     Vect n = N;
//     if (cosi < 0) {
//         cosi = -cosi;
//         std::swap(etai, etat); n = -N;
//     }
//     float eta = etai / etat;
//     float k = 1 - eta*eta*(1 - cosi*cosi);
//     return k < 0 ? Vect(0,0,0) : I*eta + n*(eta * cosi - sqrtf(k));
// }

Color trace_ray (const std::vector<Object *> &objects,
                 const std::vector<Light> &lights,
                 Ray ray, int depth = 1) {
    Intersection res;
    if (depth <= 0) {
        return res.color;
    }
    Object * the_object;
    for (Object * object: objects) {
        Intersection tmp = object->intersect(ray);
        if (tmp.hit && (!res.hit || res.dist - tmp.dist > EPS)) {
            res = tmp;
            the_object = object;
        // if (sph.ray_intersect(ray, dist)) {
        }
    }
    if (res.hit) {
        // std::cout << "1";
        float diffuse_light_intensity = 0;
        float specular_light_intensity = 0;
        for (Light light: lights) {
            Vect light_dir = (light.pos - res.pos).normalize();
            float light_dist = (light.pos - res.pos).len();
            Vect pos = light_dir.dot(res.normal) < 0 ? res.pos - res.normal * 1e-3 : res.pos + res.normal * 1e-3;
            Ray light_ray(pos, light_dir);
            Ray light_ray1(pos, light_dir - light_dir *2);
            Intersection tmp_res;
            for (Object * object: objects) {
                Intersection tmp = object->intersect(light_ray);
                if (tmp.hit && (!tmp_res.hit || (tmp_res.dist - tmp.dist > EPS))) {
                    tmp_res = tmp;
                }
            }
            if (tmp_res.hit && (tmp_res.dist < light_dist)) {
                continue;
            }
            diffuse_light_intensity += light.intensity * std::max(0.f, light_dir.dot(res.normal));
                                                    //    / (light_dir.len() * light_dir.len());
            specular_light_intensity += powf(std::max(0.f, ((light_dir).reflect(res.normal)).dot(ray.dir)),
                                             the_object->specular)
                                        * light.intensity;
        }
        // Vect refract_dir = refract(ray.dir, res.normal, the_object->refraction).normalize();
        res.color = res.color * diffuse_light_intensity * the_object->reflection[0] +
                    Color(1, 1, 1, 1) * (specular_light_intensity * the_object->reflection[1]);
        if (the_object->reflection[2] > 0) {
            Vect refl_pos = (res.reflected).dot(res.normal) < 0 ? res.pos - res.normal * 1e-3 : res.pos + res.normal * 1e-3;
            Ray reflected_ray(refl_pos, res.reflected);
            Color reflected_color = trace_ray(objects, lights, reflected_ray, depth - 1);
            res.color = res.color + reflected_color * the_object->reflection[2];
        }
        if (the_object->reflection[3] > 0) {
            Vect refr_pos = res.refracted.dot(res.normal) < 0 ? res.pos - res.normal * 1e-3 : res.pos + res.normal * 1e-3;
            Ray refracted_ray(refr_pos, res.refracted);
            Color refracted_color = trace_ray(objects, lights, refracted_ray, depth - 1);
            res.color = res.color + refracted_color * the_object->reflection[3];
        }
        // std::cout << refracted_color.r<< "," << refracted_color.g << "," << refracted_color.b << "\n";
    }
    return res.color;
}

void render(const int size) {
    std::vector<Color> framebuffer(size * size);
    Ray ray = Ray();
    // Vect a(size / 4, size / 3, size);
    // Vect b(size / 3, size / 3 * 2, size);
    // Vect c(size / 4 * 3, size / 2, size);
    // Triangle tr = Triangle(a, b, c);
    Vect ico_pos (size / 2, size / 2, size / 4);
    Vect ico_pos1 (size / 4, size / 4, 2*size/3);
    Icosahedron ico(100, ico_pos, Vect(0, 1, 0));
    // Icosahedron ico(100, Vect(3*size/4, size/3, size/3), Vect(0, 1, 0));
    // Icosahedron ico(50, Vect(3*size/4, size/3, size/3), Vect(0,1,0));
    ico.reflection[0] = 0.2;
    ico.reflection[1] = 0.8;
    ico.reflection[2] = 0.0;
    ico.reflection[3] = 1.2; //прозрачность
    ico.refraction = 1;
    ico.specular = 125;
    // ico.color = Color(0, 0, 0, 1);
    Icosahedron ico1(150, ico_pos1, Vect(1, 1, 0));
    ico1.reflection[0] = 0.3;
    ico1.reflection[1] = 0.3;
    ico1.reflection[2] = 0.3;
    ico1.reflection[3] = 0.0;
    ico.refraction = 50;
    ico1.specular = 50;
    Sphere sph1(Vect(size / 4, size / 4, 2*size/3), 120);
    sph1.reflection[0] = 0.6;
    sph1.reflection[1] = 0.3;
    sph1.reflection[2] = 0.8;
    sph1.reflection[3] = 0.0;
    sph1.specular = 150;
    sph1.color = Color(1, 0, 0, 1);
    Sphere sph(Vect(3*size/4, size/3, size/3), 50);
    // Sphere sph(Vect(size / 2, size / 2, size / 4), 50);
    sph.reflection[0] = 0.0;
    sph.reflection[1] = 1;
    sph.reflection[2] = 0.8;
    sph.reflection[3] = 1.2;
    ico.refraction = 1.5;
    sph.specular = 1425;
    sph.color = Color(1, 0, 0, 1);
    // for (int i = 0; i < 20; i++) {
    //     std::cout  << "(" << ico.triangles[i].a.x << ","
    //                << ico.triangles[i].a.y << ","
    //                << ico.triangles[i].a.z << "), ("
    //                << ico.triangles[i].b.x << ","
    //                << ico.triangles[i].b.y << ","
    //                << ico.triangles[i].b.z << "), ("
    //                << ico.triangles[i].c.x << ","
    //                << ico.triangles[i].c.y << ","
    //                << ico.triangles[i].c.z << ")\n";
    // }
    ray.pos = Vect(size/2, size/2, -size);
    // ray.dir = Vect(0, 0, 1);
    // std::cout<<"3\n";
    // Intersection intr = tr.intersect(ray);
    // std::cout << intr.hit << " " << intr.dist << "\n";
    std::vector<Object *> objects;
    objects.push_back(&ico);
    objects.push_back(&ico1);
    // objects.push_back(&sph1);
    objects.push_back(&sph);
    Light light_1(Vect(2*size/3, size/2, -size), 5);
    Light light_2(Vect(0, -size/2, size/4), 2);
    Light light_3(Vect(-size/4, size/2, size/2), 2);
    Light light_4(Vect(2*size/3, size / 3, -size/2), 2);
    Light light_5(Vect(3*size/4 + 150, size/4-150, size/2-150), 2);
    std::vector<Light> lights;
    lights.push_back(light_1);
    lights.push_back(light_2);
    lights.push_back(light_3);
    lights.push_back(light_4);
    lights.push_back(light_5);
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            // float x1 =  (2*(x + 0.5)/(float)size  - 1)*tan(fov/2.);
            // float y1 = -(2*(y + 0.5)/(float)size - 1)*tan(fov/2.);
            Vect pix_pos(x, y, 0);
            ray.dir = (pix_pos - ray.pos).normalize();
            // ray.dir = Vect(x1, y1, -1).normalize();
            // std::cout<<"5\n";
            // std::cout<<ray.dir.x << " "<<ray.dir.y<<" "<<ray.dir.z<<"\n";
            framebuffer[x + y * size] = trace_ray(objects, lights, ray, 4);
            // if (framebuffer[x + y * size] == Color(0, 0, 0, 1)) {

            // }
        }
    }
    
    unsigned char *data = new unsigned char[size * size * 4];
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            data[0 + x * 4 + y * size * 4] = (unsigned char)(255 * std::max(0.f, std::min(1.f, framebuffer[x + y * size].r)));
            data[1 + x * 4 + y * size * 4] = (unsigned char)(255 * std::max(0.f, std::min(1.f, framebuffer[x + y * size].g)));
            data[2 + x * 4 + y * size * 4] = (unsigned char)(255 * std::max(0.f, std::min(1.f, framebuffer[x + y * size].b)));
            data[3 + x * 4 + y * size * 4] = (unsigned char)(255 * std::max(0.f, std::min(1.f, framebuffer[x + y * size].a)));
        }
    }
    stbi_write_png("327_rudenko_v4v5.png", size, size, 4, data, size * 4);
}

int main() {
    std::cout<<"start\n";
    render(SIZE);
    return 0;
}