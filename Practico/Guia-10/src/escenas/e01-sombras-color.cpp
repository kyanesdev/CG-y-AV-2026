// Practico 10, escena 1: sombras de colores (ejemplo literal de la guia).
#include <memory>
#include <escenas.h>
#include <plane.h>
#include <sphere.h>

void build_scene_01(Scene& scene, Camera& camera)
{
    camera.look_at(glm::vec3(0.0f, 6.0f, 7.0f), glm::vec3(0.0f, 0.3f, 0.0f),
                   glm::vec3(0.0f, 1.0f, 0.0f), 45.0f);
    scene.set_background(glm::vec3(0.02f));

    auto floor = std::make_shared<Diffuse>();
    floor->ka = glm::vec3(0.02f);
    floor->kd = glm::vec3(0.8f);
    scene.add_object(std::make_shared<Plane>(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), floor));

    auto white = std::make_shared<Diffuse>();
    white->ka = glm::vec3(0.02f);
    white->kd = glm::vec3(0.8f);
    white->ks = glm::vec3(0.3f);
    white->shininess = 30.0f;
    scene.add_object(std::make_shared<Sphere>(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, white));

    const glm::vec3 att(1.0f, 0.12f, 0.02f);
    scene.add_light(Light(glm::vec3(0.0f, 5.0f, -4.0f), glm::vec3(2.4f, 0.0f, 0.0f), att));
    scene.add_light(Light(glm::vec3(-3.464f, 5.0f, 2.0f), glm::vec3(0.0f, 2.4f, 0.0f), att));
    scene.add_light(Light(glm::vec3(3.464f, 5.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.4f), att));
    scene.set_max_depth(8U);
}
