// Practico 10, escena 5: banco con aire, agua, vidrio y diamante.
#include <memory>
#include <checker.h>
#include <dielectric.h>
#include <escenas.h>
#include <plane.h>
#include <sphere.h>

void build_scene_05(Scene& scene, Camera& camera)
{
    camera.look_at(glm::vec3(0.0f, 1.8f, 5.0f), glm::vec3(0.0f, 0.7f, 0.0f),
                   glm::vec3(0.0f, 1.0f, 0.0f), 45.0f);
    scene.set_background(glm::vec3(0.05f, 0.07f, 0.1f));
    auto floor = std::make_shared<Checker>();
    floor->size = 0.4f;
    floor->kd = glm::vec3(0.85f); floor->ka = glm::vec3(0.05f);
    floor->kd2 = glm::vec3(0.08f, 0.08f, 0.3f);
    floor->ka2 = glm::vec3(0.005f, 0.005f, 0.018f);
    scene.add_object(std::make_shared<Plane>(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), floor));

    const float xs[] = {-2.4f, -0.8f, 0.8f, 2.4f};
    const float indices[] = {1.0f, 1.33f, 1.5f, 2.42f};
    for( int i = 0; i < 4; ++i ) {
        auto mat = std::make_shared<Dielectric>();
        mat->ior = indices[i];
        mat->transparency = 0.9f;
        mat->ks = glm::vec3(1.0f); mat->shininess = 150.0f;
        scene.add_object(std::make_shared<Sphere>(glm::vec3(xs[i], 0.8f, 0.0f), 0.7f, mat));
    }
    const glm::vec3 att(1.0f, 0.12f, 0.02f);
    scene.add_light(Light(glm::vec3(-3.0f, 6.0f, 4.0f), glm::vec3(2.6f), att));
    scene.add_light(Light(glm::vec3(4.0f, 4.0f, 3.0f), glm::vec3(1.0f), att));
    scene.set_max_depth(8U);
}
