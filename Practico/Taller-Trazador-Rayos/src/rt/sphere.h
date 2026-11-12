#ifndef SPHERE_H
#define SPHERE_H

#include <object.h>

class Sphere : public Object {
public:
    Sphere(const glm::vec3& center, float radius, std::shared_ptr<Material> m) :
        Object(std::move(m)), c_(center), r_(radius) {}

    bool intersect(const Ray& r, Hit& out) const override {
        // TODO (paso 2): intersección rayo-esfera.
        // Un punto P está en la esfera si |P - c|^2 = r^2. Al reemplazar P por
        // el rayo, P(t) = o + t d: queda una cuadrática a t^2 + b t + cc = 0
        // (cc y no c, para no confundir el término independiente con el centro).
        //   - Discriminante negativo: el rayo no toca la esfera.
        //   - De las dos raíces, quedarse con la más chica que sea mayor que
        //     EPSILON. Si el origen del rayo está adentro de la esfera, la
        //     primera raíz es negativa y la segunda es la intersección que se
        //     ve desde dentro de la esfera.
        //   - Si ninguna es mayor que EPSILON, la esfera está detrás.
        //   - Completar out como en Plane. La normal hacia afuera es
        //     (P - c) normalizada.
        (void)r; (void)out;
        return false;
    }

private:
    glm::vec3 c_;
    float r_;
};

#endif // SPHERE_H
