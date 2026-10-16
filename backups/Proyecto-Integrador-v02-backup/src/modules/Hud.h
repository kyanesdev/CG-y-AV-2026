#pragma once
#include "Aircraft.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include <vector>

// HUD 2D de geometria propia: tres instrumentos reales y progreso del circuito.
class Hud {
public:
    void init();
    void update_text(float speed_kmh, float altitude_m, float roll_degrees,
                     unsigned checkpoints, int score, bool paused);
    void collect(std::vector<RenderItem>& items, int width, float pitch, float roll) const;
private:
    Mesh cube_;
    Mesh text_;
    bool has_text_ = false;
};
