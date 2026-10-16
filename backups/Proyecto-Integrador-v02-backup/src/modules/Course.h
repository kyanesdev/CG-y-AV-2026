#pragma once
#include "Aircraft.h"
#include "Mesh.h"
#include <array>
#include <glm/glm.hpp>
#include <vector>

class Course {
public:
    void init();
    void reset();
    // Devuelve true SOLO cuando cruza en orden el aro activo.
    bool test_crossing(const glm::vec3& previous, const glm::vec3& current);
    void collect(std::vector<RenderItem>& items) const;
    unsigned completed() const { return next_; }
    bool finished() const { return next_ == checkpoints_.size(); }
    int score() const { return static_cast<int>(next_)*100; }
private:
    struct Checkpoint {
        glm::vec3 center;
        glm::vec3 normal;
        float radius;
    };
    const std::array<Checkpoint,5> checkpoints_{{
        {{  0,120,-480}, {0,0,-1}, 48.0f},
        {{  0,125,-900}, {0,0,-1}, 48.0f},
        {{ 58,130,-1320},{0,0,-1}, 48.0f},
        {{-58,130,-1730},{0,0,-1}, 48.0f},
        {{  0,125,-2140},{0,0,-1}, 48.0f}
    }};
    Mesh segment_;
    unsigned next_ = 0;
};
