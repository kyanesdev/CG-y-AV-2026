#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include <hit.h>
#include <light.h>
#include <material.h>
#include <object.h>
#include <ray.h>

class Scene {
public:
    void add_object(std::shared_ptr<Object> o)  { objects_.push_back(std::move(o)); }
    void add_light(const Light& l)              { lights_.push_back(l); }

    /// Fondo de color sólido
    void set_background(const glm::vec3& c)     { bg_horizon_ = bg_zenith_ = c; }
    /// Fondo degradado: horizon en la dirección horizontal, zenith hacia arriba
    /// (y hacia abajo se repite el del horizonte)
    void set_background(const glm::vec3& horizon, const glm::vec3& zenith) {
        bg_horizon_ = horizon; bg_zenith_ = zenith;
    }

    /// Color que obtiene el rayo r de la escena
    glm::vec3 trace(const Ray& r) const;

    /// Vista de control: la normal en el primer punto que toca el rayo, pasada
    /// del rango [-1,1] a [0,1] para poder visualizarla como un color (x rojo,
    /// y verde, z azul)
    glm::vec3 normal_color(const Ray& r) const;

    /// Modelo de iluminación de Phong en el punto h, con sombras, con cada
    /// componente separado para poder procesarla después (si hiciese falta)
    LocalTerms local_terms(const Ray& r, const Hit& h) const;

private:
    std::vector<std::shared_ptr<Object>> objects_;
    std::vector<Light> lights_;
    glm::vec3 bg_horizon_ = glm::vec3(0.0f);
    glm::vec3 bg_zenith_  = glm::vec3(0.0f);

    bool find_nearest(const Ray& r, Hit& closest) const;
    glm::vec3 background(const Ray& r) const;
    float light_transmission(const glm::vec3& p, const glm::vec3& n,
                             const glm::vec3& light_pos) const;
};

#endif // SCENE_H
