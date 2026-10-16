#include "Course.h"
#include "Primitives.h"
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Course::init() { segment_.load(primitives::cube()); }
void Course::reset() { next_ = 0; }

bool Course::test_crossing(const glm::vec3& previous, const glm::vec3& current) {
    if (finished()) return false;
    const Checkpoint& cp = checkpoints_[next_];
    const float a = glm::dot(previous-cp.center, cp.normal);
    const float b = glm::dot(current-cp.center, cp.normal);
    if (a > 0.0f || b < 0.0f || b == a) return false;
    // Interseccion REAL del segmento entre dos estados del FDM con el plano.
    const float t = -a/(b-a);
    const glm::vec3 intersection = previous + t*(current-previous);
    const glm::vec3 delta = intersection-cp.center;
    const glm::vec3 radial = delta - glm::dot(delta,cp.normal)*cp.normal;
    if (glm::dot(radial,radial) > (cp.radius-3.0f)*(cp.radius-3.0f)) return false;
    ++next_;
    return true;
}

void Course::collect(std::vector<RenderItem>& items) const {
    constexpr unsigned sides = 36;
    for (unsigned n=0; n<checkpoints_.size(); ++n) {
        const Checkpoint& cp = checkpoints_[n];
        const glm::vec3 color = (n<next_)?glm::vec3(0.25f,0.35f,0.39f)
            :(n==next_)?glm::vec3(1.0f,0.73f,0.10f)
                       :glm::vec3(0.21f,0.76f,0.91f);
        for (unsigned i=0;i<sides;++i) {
            const float angle=(static_cast<float>(i)+0.5f)*glm::two_pi<float>()/sides;
            const glm::vec3 center = cp.center + cp.radius*glm::vec3(std::cos(angle),std::sin(angle),0);
            glm::mat4 m = glm::translate(glm::mat4(1),center);
            m = glm::rotate(m,angle+glm::half_pi<float>(),{0,0,1});
            m = glm::scale(m,{2.0f*glm::pi<float>()*cp.radius/sides+0.7f,3.0f,3.0f});
            items.push_back({&segment_,m,color});
        }
    }
}
