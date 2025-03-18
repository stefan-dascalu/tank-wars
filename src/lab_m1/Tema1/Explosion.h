#pragma once

#include <glm/glm.hpp>
#include "components/simple_scene.h"
#include <memory>

class Explosion {
public:
    Explosion(glm::vec2 position);

    void Update(float deltaTime);

    // Draws the explosion using the provided rendering function
    void DrawExplosion(glm::mat3 visMatrix, Shader* shader,
                       std::function<void(Mesh*, Shader*, const glm::mat3&)> RenderMesh2D);

    // Checks if the explosion is still active
    bool IsActive() const { return active; }

private:
    glm::vec2 position;
    std::unique_ptr<Mesh> explosionMesh;
    
    float timeElapsed;
    float duration;
    bool active;        // Whether the explosion is active
};
