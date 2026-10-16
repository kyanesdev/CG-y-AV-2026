#pragma once
#include "Aircraft.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include <vector>

class Scene {
public:
    void init();
    void collect(std::vector<RenderItem>& items) const;
    static float terrain_height(float x, float z);
private:
    struct Piece {
        unsigned mesh;
        glm::mat4 transform{1.0f};
        glm::vec3 color{1.0f};
    };
    Mesh terrain_;
    Mesh cube_;
    Mesh cone_;
    std::vector<Piece> pieces_;
    void add_cube(glm::vec3 center, glm::vec3 size, glm::vec3 color);
    void add_tree(float x, float z);
};
