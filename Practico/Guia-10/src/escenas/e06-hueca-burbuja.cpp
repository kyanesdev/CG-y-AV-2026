// Practico 10, escena 6: esfera maciza, esfera hueca y burbuja de aire en agua.
#include <memory>
#include <checker.h>
#include <dielectric.h>
#include <escenas.h>
#include <plane.h>
#include <sphere.h>

void build_scene_06(Scene& scene, Camera& camera)
{
    camera.look_at(glm::vec3(0.0f, 2.2f, 6.0f), glm::vec3(0.0f, 0.8f, 0.0f),
                   glm::vec3(0.0f, 1.0f, 0.0f), 45.0f);
    scene.set_background(glm::vec3(0.05f, 0.07f, 0.1f));

    auto floor = std::make_shared<Checker>();
    floor->size = 0.4f;
    floor->kd = glm::vec3(0.85f); floor->ka = glm::vec3(0.05f);
    floor->kd2 = glm::vec3(0.06f, 0.3f, 0.1f);
    floor->ka2 = glm::vec3(0.004f, 0.018f, 0.006f);
    scene.add_object(std::make_shared<Plane>(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), floor));

    auto glass = std::make_shared<Dielectric>();
    glass->ior = 1.5f; glass->transparency = 0.9f;
    glass->ks = glm::vec3(1.0f); glass->shininess = 150.0f;
    scene.add_object(std::make_shared<Sphere>(glm::vec3(-2.0f, 0.9f, 0.0f), 0.8f, glass));

    const glm::vec3 center(0.0f, 0.9f, 0.0f);
    // Caparazon de vidrio: esfera exterior y superficie interior con indice inverso.
    scene.add_object(std::make_shared<Sphere>(center, 0.8f, glass));
    auto hollow_in = std::make_shared<Dielectric>(*glass);
    hollow_in->ior = 1.0f / 1.5f;
    hollow_in->ks = glm::vec3(0.0f);
    scene.add_object(std::make_shared<Sphere>(center, 0.768f, hollow_in));

    // Burbuja: aire dentro de agua, indice relativo n_aire / n_agua.
    auto bubble = std::make_shared<Dielectric>(*glass);
    bubble->ior = 1.0f / 1.33f;
    scene.add_object(std::make_shared<Sphere>(glm::vec3(2.0f, 0.9f, 0.0f), 0.8f, bubble));

    const glm::vec3 att(1.0f, 0.12f, 0.02f);
    scene.add_light(Light(glm::vec3(-3.0f, 6.0f, 4.0f), glm::vec3(2.6f), att));
    scene.add_light(Light(glm::vec3(4.0f, 4.0f, 3.0f), glm::vec3(1.0f), att));
    scene.set_max_depth(8U);
}
