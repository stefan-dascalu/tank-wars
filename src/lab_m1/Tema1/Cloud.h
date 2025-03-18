#pragma once

#include <glm/glm.hpp>
#include "components/simple_scene.h"
#include <memory>

class Cloud {
public:
    Cloud(float startX, float startY, float speed);

    void Update(float deltaTime);

    // Draws the cloud using the provided rendering function
    void DrawCloud(glm::mat3 visMatrix, Shader* shader,
                   std::function<void(Mesh*, Shader*, const glm::mat3&)> RenderMesh2D);

private:
    void InitMesh();
    std::unique_ptr<Mesh> cloudMesh;

    float x, y;                          // Position of the cloud
    float speed;
};
