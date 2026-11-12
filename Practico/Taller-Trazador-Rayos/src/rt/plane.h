#ifndef PLANE_H
#define PLANE_H

#include <object.h>

/// Plano infinito, modelado como un punto y una normal
class Plane : public Object {
public:
    Plane(const glm::vec3& point, const glm::vec3& normal, std::shared_ptr<Material> m) :
        Object(std::move(m)), p0_(point), n_(glm::normalize(normal)) {}

    bool intersect(const Ray& r, Hit& out) const override {
        // TODO (paso 1): intersección rayo-plano.
        // Un punto P está en el plano si (P - p0) · n = 0. Reemplazar P por el
        // rayo, P(t) = o + t d, y despejar t.
        //   - Si d · n es (casi) cero, el rayo es paralelo al plano: no hay
        //     intersección (hacer comparación para punto flotante: valor
        //     absoluto con 1e-7 y no con 0.0).
        //   - Si t <= EPSILON, el plano está detrás del origen del rayo.
        //   - Si no, completar los datos en la intersección (Hit& out): t, el
        //     punto P(t) (ver Ray::at), el material (mat_.get()) y la normal
        //     con out.set_face(r, n_), que la orienta hacia el lado del que
        //     viene el rayo.
        (void)r; (void)out;
        return false;
    }

private:
    glm::vec3 p0_;
    glm::vec3 n_;
};

#endif // PLANE_H
