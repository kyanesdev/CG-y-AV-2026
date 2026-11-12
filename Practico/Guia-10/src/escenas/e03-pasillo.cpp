// Practico 10, escena 3: dos paredes finitas de espejo (cuatro triangulos).
#include <memory>
#include <checker.h>
#include <escenas.h>
#include <mirror.h>
#include <plane.h>
#include <sphere.h>
#include <triangle.h>

void build_scene_03(Scene& scene, Camera& camera)
{
    camera.look_at(glm::vec3(-2.0f, 2.0f, 7.5f), glm::vec3(3.0f, 1.4f, 0.0f),
                   glm::vec3(0.0f, 1.0f, 0.0f), 45.0f);
    scene.set_background(glm::vec3(0.02f, 0.03f, 0.08f));

    auto floor = std::make_shared<Checker>();
    floor->size = 1.0f;
    floor->ka = glm::vec3(0.07f); floor->kd = glm::vec3(0.7f);
    floor->ka2 = glm::vec3(0.01f); floor->kd2 = glm::vec3(0.1f);
    scene.add_object(std::make_shared<Plane>(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), floor));

    auto wall = std::make_shared<Mirror>();
    wall->ka = glm::vec3(0.0f); wall->kd = glm::vec3(0.02f);
    wall->ks = glm::vec3(0.0f); wall->kr = 0.9f;
    for( float x : {-3.0f, 3.0f} ) {
        const glm::vec3 a(x, 0.0f, 14.0f), b(x, 0.0f, -12.0f);
        const glm::vec3 c(x, 5.0f, -12.0f), d(x, 5.0f, 14.0f);
        scene.add_object(std::make_shared<Triangle>(a, b, c, wall));
        scene.add_object(std::make_shared<Triangle>(a, c, d, wall));
    }

    auto red = std::make_shared<Diffuse>();
    red->ka = glm::vec3(0.08f, 0.01f, 0.01f);
    red->kd = glm::vec3(0.75f, 0.08f, 0.06f);
    red->ks = glm::vec3(0.5f); red->shininess = 60.0f;
    scene.add_object(std::make_shared<Sphere>(glm::vec3(0.2f, 1.0f, 0.0f), 1.0f, red));

    auto green = std::make_shared<Diffuse>();
    green->ka = glm::vec3(0.01f, 0.06f, 0.02f);
    green->kd = glm::vec3(0.1f, 0.6f, 0.2f);
    green->ks = glm::vec3(0.4f); green->shininess = 40.0f;
    scene.add_object(std::make_shared<Sphere>(glm::vec3(1.2f, 0.6f, 2.5f), 0.6f, green));

    auto ball_mirror = std::make_shared<Mirror>();
    ball_mirror->ka = glm::vec3(0.02f); ball_mirror->kd = glm::vec3(0.1f);
    ball_mirror->ks = glm::vec3(1.0f); ball_mirror->shininess = 150.0f;
    ball_mirror->kr = 0.85f;
    scene.add_object(std::make_shared<Sphere>(glm::vec3(-1.3f, 0.9f, -2.5f), 0.9f, ball_mirror));

    scene.add_light(Light(glm::vec3(0.0f, 9.0f, 5.0f), glm::vec3(4.0f), glm::vec3(1.0f, 0.12f, 0.02f)));
    scene.set_max_depth(8U); // Comparar con 1U.
}
