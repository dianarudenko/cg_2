#include <vector>
#include <iostream>
#include <ctime>
// #include <algorithm>
#include "lib.h"
#include "geometry.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Color trace_ray (const std::vector<Object *> &objects,
                 const std::vector<Light> &lights,
                 Ray ray,
                 std::vector<Object *> * refraction_stack,
                 int depth = 1) {
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
        }
    }
    if (res.hit) {
        Vect normal = res.normal;
        float diffuse_light_intensity = 0;
        float specular_light_intensity = 0;
        for (Light light: lights) {
            Vect light_dir = (light.pos - res.pos).normalize();
            float light_dist = (light.pos - res.pos).len();
            Vect pos = light_dir.dot(res.normal) < 0 ? res.pos - res.normal * 1e-2 : res.pos + res.normal * 1e-2;
            Ray light_ray(pos, light_dir);
            bool visible = true;
            float intensity_coeff = 1;
            while (visible) {
                Intersection tmp_res;
                Object * tmp_object;
                for (Object * object: objects) {
                    Intersection tmp = object->intersect(light_ray);
                    if (tmp.hit) {
                        tmp_res = tmp;
                        tmp_object = object;
                        break;
                    }
                }
                if (tmp_res.hit && (tmp_res.dist < light_dist)) {
                    if (tmp_object->transparency < EPS) {
                        visible = false;
                    } else {
                        light_ray.pos = light_dir.dot(tmp_res.normal) < 0 ?
                                        tmp_res.pos - tmp_res.normal * 1e-2 :
                                        tmp_res.pos + tmp_res.normal * 1e-2;
                        intensity_coeff = tmp_object->transparency;
                    }
                } else {
                    break;
                }
            }
            if (!visible) {
                continue;
            }
            diffuse_light_intensity += light.intensity * std::max(0.f, light_dir.dot(normal)) *
                                       intensity_coeff;
            specular_light_intensity += powf(std::max(0.f, ((light_dir).reflect(normal)).dot(ray.dir)),
                                             the_object->specular);
        }
        res.color = res.color * diffuse_light_intensity * the_object->absorbtion +
                    Color(1, 1, 1, 1) * specular_light_intensity * the_object->reflection[0];
        if (the_object->reflection[1] > 0) {
            Vect refl_pos = (res.reflected).dot(res.normal) < 0 ? res.pos - res.normal * 1e-2 : res.pos + res.normal * 1e-2;
            Ray reflected_ray(refl_pos, res.reflected);
            Color reflected_color = trace_ray(objects, lights, reflected_ray, refraction_stack, depth - 1);
            res.color = res.color + reflected_color * the_object->reflection[1];
        }
        if (the_object->transparency > 0.0001) {
            float other_refraction;
            if (refraction_stack->size() == 0) {
                other_refraction = 1;
            } else {
                other_refraction = refraction_stack->back()->refraction;
            }
            bool do_refract = true;
            Vect refracted;
            try {
                refracted = ray.dir.refract(res.normal, the_object->refraction, other_refraction);
            } catch(int) {
                do_refract = false;
            }
            if (do_refract) {
                Vect refr_pos = refracted.dot(res.normal) < 0 ? res.pos - res.normal * 1e-2 : res.pos + res.normal * 1e-2;
                Ray refracted_ray(refr_pos, refracted);
                if (refraction_stack->size() != 0) {
                    bool contains = false;
                    for (int i = 0; i < int(refraction_stack->size()); i++) {
                        if (refraction_stack->at(refraction_stack->size() - i - 1) == the_object) {
                            contains = true;
                            refraction_stack->erase(refraction_stack->end() - i - 1);
                            break;
                        }
                    }
                    if (!contains) {
                        refraction_stack->push_back(the_object);
                    }
                } else {
                    refraction_stack->push_back(the_object);
                }
                Color refracted_color = trace_ray(objects, lights, refracted_ray, std::move(refraction_stack), depth - 1);
                res.color = res.color + refracted_color * the_object->transparency;
            }
        }
    }
    return res.color;
}

void render(const int size) {
    std::vector<Color> framebuffer(size * size);
    Ray ray = Ray();
    Vect ico_pos (size / 2, size / 3, size / 4);
    Vect ico_pos1 (size / 4 - 20, size / 4, 2*size/3 + 20);

    //Icosahedron
    Icosahedron ico(size/2.5, ico_pos, Vect(0, 1, 0.2));
    ico.absorbtion = 0.6;
    ico.reflection[0] = 0.6;
    ico.reflection[1] = 0.2;
    ico.transparency = 1.4;
    ico.refraction = 1.5;
    ico.specular = 50;
    ico.color = Color(0.2, 0.2, 0.2, 1);

    //Inner icosahedron
    Icosahedron inner_ico(ico.edge * 0.95, ico_pos, Vect(0, 1, 0.2));
    inner_ico.reflection[1] = ico.reflection[1];
    inner_ico.transparency = ico.transparency; //прозрачность
    inner_ico.refraction = 1;
    inner_ico.color = Color(0.2,0.2,0.2);
    float ico_rad = sqrt(2 * (5 + sqrt(5))) * ico.edge / 4;
    float cyl_height = 40;

    //Stand
    Cylinder stand(ico.center + ico.axis * (ico_rad + cyl_height / 2 - 2), cyl_height, size / 2, ico.axis);
    stand.absorbtion = 0.6;
    stand.reflection[0] = 0.5;
    stand.reflection[1] = 0.5;
    stand.transparency = 0.0;
    stand.refraction = 10;
    stand.specular = 250;
    stand.color = Color(0.3, 0.3, 0.2, 1) - Color() / 3;

    //Plane
    Plane plane(-ico.axis, ico.center + ico.axis * (ico_rad + cyl_height), Color(0.3, 1, 1), Color(0.3, 1, 1));
    plane.reflection[0] = 0;
    plane.reflection[1] = 0;
    plane.absorbtion = 0.5;
    ray.pos = Vect(size/2, size/2, -size);

    std::vector<Object *> objects;
    objects.push_back(&ico);
    objects.push_back(&inner_ico);
    objects.push_back(&stand);
    objects.push_back(&plane);
    
    Ellipsoid * cavity;

    float ico_small_rad = (3 + sqrt(5)) * ico.edge / (4 * sqrt(3));
    for (int i = 0; i < 60; i++) {
        float rad_a = (4 + ((float) rand()) / RAND_MAX * 10) * (size / 512);
        // Let it be spheres

        // float rad_b = (8 + ((float) rand()) / RAND_MAX * 10) * (SIZE / 512);
        // float rad_c = (4 + ((float) rand()) / RAND_MAX * 10) * (SIZE / 512);
        // float rad = std::max(std::max(rad_a, rad_b), rad_c);

        // The bubbles may be colored

        // float r_color = ((float) rand()) / RAND_MAX;
        // float g_color = ((float) rand()) / RAND_MAX;
        // float b_color = ((float) rand()) / RAND_MAX;
        float x_pos, y_pos, z_pos;
        do {
            x_pos = ((float) rand()) / RAND_MAX * (ico_small_rad - rad_a) * 0.9 * (rand() % 2 ? -1 : 1);
            y_pos = ((float) rand()) / RAND_MAX * (ico_small_rad - rad_a) * 0.9 * (rand() % 2 ? -1 : 1);
            z_pos = ((float) rand()) / RAND_MAX * (ico_small_rad - rad_a) * 0.9 * (rand() % 2 ? -1 : 1);
        } while ((Vect(x_pos, y_pos, z_pos)).len() > (ico_small_rad - rad_a) * 0.9);
        cavity = new Ellipsoid(ico.center + Vect(x_pos, y_pos, z_pos), rad_a, rad_a, rad_a);
        cavity->color = Color(0.1, 0.1, 0.1);
        cavity->refraction = 1.2;
        cavity->transparency = 1.2;
        cavity->absorbtion = 0.7;
        objects.push_back(cavity);
    }

    Light light_1(Vect(2*size/3, size/2, -size), 5);
    Light light_3(Vect(-size/4, size/2, size/2), 4);
    Light light_4(Vect(2*size/3, size / 3, -size/2), 2);
    std::vector<Light> lights;
    lights.push_back(light_1);
    lights.push_back(light_3);
    lights.push_back(light_4);


    std::vector<Object *> refraction_stack;
    refraction_stack.clear();
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            Vect pix_pos(x, y, 0);
            ray.dir = (pix_pos - ray.pos).normalize();
            framebuffer[x + y * size] = trace_ray(objects, lights, ray, &refraction_stack, 8);
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
    stbi_write_png("327_rudenko_v5v2.png", size, size, 4, data, size * 4);
}

int main(int argc, char ** argv) {
    std::cout<<"start\n";
    int size = 512;
    if (argc > 2) {
        if ((std::string(argv[1]) == "-w") || (std::string(argv[2]) == "1024")) {
            size = 1024;
        }
    }
    time_t start_time = std::time(NULL);
    render(size);
    time_t end_time = std::time(NULL);
    std::cout << "\nTime: " << end_time - start_time << "s\n";
    return 0;
}