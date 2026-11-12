#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

#include <hit.h>
#include <ray.h>

class Scene;

/// Iluminación local de Phong separada en sus tres términos, para que cada
/// material pueda hacer uso del que necesite
struct LocalTerms {
    glm::vec3 ambient  = glm::vec3(0.0f);
    glm::vec3 diffuse  = glm::vec3(0.0f);
    glm::vec3 specular = glm::vec3(0.0f);

    glm::vec3 total(void) const { return ambient + diffuse + specular; }
};

/// Clase base de los materiales: los coeficientes del modelo de iluminación de
/// Phong y la forma en que el material usa la iluminación local. Las
/// implementaciones por defecto son las del material difuso; un material nuevo
/// deriva de esta clase y sobrescribe lo que cambia
class Material {
public:
    glm::vec3 ka = glm::vec3(0.02f);    ///< ambiente
    glm::vec3 kd = glm::vec3(0.8f);     ///< difuso
    glm::vec3 ks = glm::vec3(0.0f);     ///< especular (brillo de Phong)
    float shininess = 32.0f;            ///< exponente de Phong

    virtual ~Material() = default;

    /// Coeficientes en el punto p: una textura los hace variar con p
    virtual glm::vec3 ka_at(const glm::vec3& /*p*/) const { return ka; }
    virtual glm::vec3 kd_at(const glm::vec3& /*p*/) const { return kd; }

    /// Iluminación local en un punto. Le pide a la escena los términos de
    /// Phong (que trazan los rayos de sombra)
    virtual glm::vec3 local(const Scene& scene, const Ray& r, const Hit& h) const;
};

/// Difuso: Phong completo. Es el comportamiento de la clase base; la clase
/// existe para que la escena diga qué material usa
class Diffuse : public Material {};

#endif // MATERIAL_H
