// PLANTILLA DE PRACTICA PARA EL PARCIAL. Cambiar solo estos valores y bloques.
// Escena 12: una esfera de ejemplo sobre piso, mas una esfera de vidrio.
#include <memory>
#include <checker.h>
#include <dielectric.h>
#include <escenas.h>
#include <mirror.h>
#include <plane.h>
#include <sphere.h>
#include <triangle.h>

void build_scene_12(Scene& scene, Camera& camera)
{
    // 1. CAMARA: (ojo), (a donde mira), (arriba), FOV vertical en grados.
    camera.look_at(glm::vec3(0.0f, 2.5f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                   glm::vec3(0.0f, 1.0f, 0.0f), 45.0f);

    // 2. FONDO: gris-azul uniforme.
    scene.set_background(glm::vec3(0.05f, 0.07f, 0.12f));

    // 3. PISO: plano infinito con dibujo tipo tablero.
    auto piso = std::make_shared<Checker>();
    piso->size = 0.7f;
    piso->kd = glm::vec3(0.7f); piso->ka = glm::vec3(0.06f);
    piso->kd2 = glm::vec3(0.06f, 0.15f, 0.4f);
    piso->ka2 = glm::vec3(0.01f, 0.02f, 0.06f);
    scene.add_object(std::make_shared<Plane>(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), piso));

    // 4. ESFERA OPACA: centro, radio y colores.
    auto opaco = std::make_shared<Diffuse>();
    opaco->ka = glm::vec3(0.06f, 0.01f, 0.01f);       // ambiental
    opaco->kd = glm::vec3(0.8f, 0.1f, 0.08f);        // color difuso
    opaco->ks = glm::vec3(0.4f);                    // brillo
    opaco->shininess = 40.0f;                       // concentracion del brillo
    scene.add_object(std::make_shared<Sphere>(glm::vec3(-1.0f, 1.0f, 0.0f), 1.0f, opaco));

    // 5. ESFERA DE VIDRIO: indice de refraccion y transparencia para sombras.
    auto vidrio = std::make_shared<Dielectric>();
    vidrio->ior = 1.5f;
    vidrio->transparency = 0.8f;
    vidrio->ks = glm::vec3(1.0f);
    vidrio->shininess = 100.0f;
    scene.add_object(std::make_shared<Sphere>(glm::vec3(1.3f, 1.0f, 0.0f), 1.0f, vidrio));

    // 6. LUCES: posicion, RGB, atenuacion. Agregar otras lineas si corresponde.
    const glm::vec3 atenuacion(1.0f, 0.12f, 0.02f);
    scene.add_light(Light(glm::vec3(-3.0f, 7.0f, 5.0f), glm::vec3(3.0f), atenuacion));

    // 7. RECURSION: cuantos rebotes se permiten.
    scene.set_max_depth(8U);
}
