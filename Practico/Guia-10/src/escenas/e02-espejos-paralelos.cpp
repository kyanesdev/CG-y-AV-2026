// Practico 10, escena 2: dos espejos paralelos y perdida por rebotes.
#include <memory>
#include <checker.h>
#include <escenas.h>
#include <mirror.h>
#include <plane.h>
#include <sphere.h>

void build_scene_02(Scene& scene, Camera& camera)
{
    camera.look_at(glm::vec3(2.0f, 1.6f, 2.5f), glm::vec3(-6.0f, 0.7f, -1.5f),
                   glm::vec3(0.0f, 1.0f, 0.0f), 55.0f);
    scene.set_background(glm::vec3(0.02f, 0.02f, 0.03f));

    auto floor = std::make_shared<Checker>();
    floor->size = 0.5f;
    floor->kd = glm::vec3(0.85f); floor->ka = glm::vec3(0.05f);
    floor->kd2 = glm::vec3(0.1f); floor->ka2 = glm::vec3(0.006f);
    scene.add_object(std::make_shared<Plane>(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), floor));

    auto mirror = std::make_shared<Mirror>();
    mirror->ka = mirror->kd = mirror->ks = glm::vec3(0.0f);
    mirror->kr = 0.8f;
    scene.add_object(std::make_shared<Plane>(glm::vec3(-2.5f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), mirror));
    scene.add_object(std::make_shared<Plane>(glm::vec3(2.5f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), mirror));

    auto orange = std::make_shared<Diffuse>();
    orange->ka = glm::vec3(0.05f, 0.02f, 0.0f);
    orange->kd = glm::vec3(0.9f, 0.45f, 0.05f);
    orange->ks = glm::vec3(0.5f);
    orange->shininess = 50.0f;
    scene.add_object(std::make_shared<Sphere>(glm::vec3(0.0f, 0.7f, -1.0f), 0.7f, orange));
    scene.add_light(Light(glm::vec3(0.0f, 5.0f, 2.0f), glm::vec3(3.5f), glm::vec3(1.0f, 0.12f, 0.02f)));
    scene.set_max_depth(8U);  // Cambiar a 2U o 15U para comparar las copias.
}
