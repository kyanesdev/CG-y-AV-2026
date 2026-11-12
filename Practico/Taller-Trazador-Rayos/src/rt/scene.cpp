#include <scene.h>

bool Scene::find_nearest(const Ray& r, Hit& closest) const
{
    bool any = false;
    Hit current;

    for( const auto& obj : objects_ ){
        if( obj->intersect(r, current) && current.t < closest.t ){
            closest = current;
            any = true;
        }
    }
    return any;
}

glm::vec3 Scene::background(const Ray& r) const
{
    const float up = glm::max(glm::normalize(r.d()).y, 0.0f);
    return glm::mix(bg_horizon_, bg_zenith_, up);
}

glm::vec3 Scene::normal_color(const Ray& r) const
{
    // TODO (paso 3): normales como color.
    // Buscar la intersección más cercana (find_nearest); si no hay, devolver
    // el color del fondo. Si hay, ajustar el valor de la normal de [-1,1] a
    // [0,1] para cada componente y devolverla como color.
    // El resultado se puede ver con:  ./bin/rrt -n
    return background(r);
}

LocalTerms Scene::local_terms(const Ray& r, const Hit& h) const
{
    LocalTerms terms;
    const Material& m = *h.mat;

    const glm::vec3 v = glm::normalize(-r.d());     // hacia el observador
    const glm::vec3& n = h.n;                       // ya orientada hacia v

    // TODO (paso 4): modelo de iluminación de Phong.
    // Por defecto cada objeto sale de un color sólido tomado de la componente
    // difusa del material (kd).
    // Eliminar esta línea al escribir el paso 4:
    terms.diffuse = m.kd_at(h.p);

    // TODO (paso 4) Ambiente: ka en el punto (ka_at, no ka: una textura la
    // puede hacer variar)
    // terms.ambient = ...

    for( const auto& L : lights_ ){
        glm::vec3 s = L.pos() - h.p;                // hacia la luz
        const float dist = glm::length(s);
        s /= dist;

        // Sombra: ¿qué fracción de la luz llega hasta el punto? (paso 5)
        const float visibility = light_transmission(h.p, n, L.pos());
        if( visibility <= 0.0f ){
            continue;
        }

        // TODO (paso 4), por cada luz:
        //   - difuso:    kd_at(p) * max(n · s, 0)
        //   - especular: ks * max(R · v, 0)^shininess, con R la dirección de s
        //     reflejada respecto de n (glm::reflect(-s, n)). Si n · s <= 0 la
        //     luz está detrás de la superficie y no hay brillo.
        //   - los dos se multiplican por la intensidad que llega del foco:
        //     L.color() * L.attenuation(dist) * visibility
        //   - sumar en terms.diffuse y terms.specular.
        (void)v;
    }

    return terms;
}

float Scene::light_transmission(const glm::vec3& p, const glm::vec3& n,
                                const glm::vec3& light_pos) const
{
    // TODO (paso 5): sombras.
    // Lanzar un rayo de sombra desde p hacia la luz. Si toca algún objeto
    // antes de llegar a la luz, el punto está en sombra (devolver 0); si no,
    // le llega toda la luz (devolver 1).
    //   - Correr el origen un poco hacia afuera, p + n * EPSILON. Si no, el
    //     rayo choca con la propia superficie por redondeo y aparecen puntos
    //     negros sueltos (el "acné" de sombra).
    //   - Un objeto que está más allá de la luz no hace sombra: comparar el t
    //     de la intersección con la distancia a la luz.
    (void)p; (void)n; (void)light_pos;
    return 1.0f;
}

glm::vec3 Scene::trace(const Ray& r) const
{
    Hit h;
    if( !find_nearest(r, h) ){
        return background(r);
    }
    // La escena calcula los términos de Phong con sombras (local_terms) y el
    // material elige cuáles usa. La llamada es virtual: trace no necesita
    // saber qué material tiene el objeto
    return h.mat->local(*this, r, h);
}
