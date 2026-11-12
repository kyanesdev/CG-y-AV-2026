// Practico 10, escena 4: vidrios, agua y sombra semitransparente.
#include <memory>
#include <checker.h>
#include <dielectric.h>
#include <escenas.h>
#include <plane.h>
#include <sphere.h>

void build_scene_04(Scene& scene, Camera& camera)
{
    camera.look_at(glm::vec3(0.0f, 2.4f, 9.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                   glm::vec3(0.0f, 1.0f, 0.0f), 40.0f);
    scene.set_background(glm::vec3(0.55f, 0.65f, 0.8f), glm::vec3(0.05f, 0.12f, 0.35f));

    auto floor = std::make_shared<Checker>();
    floor->size = 1.0f;
    floor->ka = glm::vec3(0.12f); floor->kd = glm::vec3(0.75f);
    floor->ka2 = glm::vec3(0.01f, 0.03f, 0.08f);
    floor->kd2 = glm::vec3(0.05f, 0.15f, 0.45f);
    scene.add_object(std::make_shared<Plane>(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), floor));

    auto glass = std::make_shared<Dielectric>();
    glass->ior = 1.5f; glass->transparency = 0.8f;
    glass->ks = glm::vec3(1.0f); glass->shininess = 100.0f;
    scene.add_object(std::make_shared<Sphere>(glm::vec3(-1.3f, 1.2f, 2.5f), 1.2f, glass));

    auto water = std::make_shared<Dielectric>();
    water->ior = 1.33f; water->transparency = 0.9f;
    water->ks = glm::vec3(1.0f); water->shininess = 100.0f;
    scene.add_object(std::make_shared<Sphere>(glm::vec3(1.7f, 0.8f, 3.0f), 0.8f, water));

    // Esferas opacas: coeficiente ambiental ka = 0.1 * kd.
    const glm::vec3 centers[] = {
        glm::vec3(-2.6f, 0.8f, -1.5f),
        glm::vec3(-0.5f, 0.8f, -2.0f),
        glm::vec3(1.8f, 0.8f, -1.0f)
    };
    const glm::vec3 colors[] = {
        glm::vec3(0.75f, 0.1f, 0.08f),
        glm::vec3(0.85f, 0.6f, 0.05f),
        glm::vec3(0.1f, 0.55f, 0.2f)
    };
    for( int i = 0; i < 3; ++i ) {
        auto mat = std::make_shared<Diffuse>();
        mat->kd = colors[i]; mat->ka = 0.1f * colors[i];
        mat->ks = glm::vec3(0.4f); mat->shininess = 50.0f;
        scene.add_object(std::make_shared<Sphere>(centers[i], 0.8f, mat));
    }
    const glm::vec3 att(1.0f, 0.12f, 0.02f);
    scene.add_light(Light(glm::vec3(-5.0f, 9.0f, 7.0f), glm::vec3(3.0f), att));
    scene.add_light(Light(glm::vec3(6.0f, 7.0f, 5.0f), glm::vec3(2.0f), att));
    scene.set_max_depth(8U);
}
