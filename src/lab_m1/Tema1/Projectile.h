#pragma once

#include "transform2D1.h"
#include "object2D1.h"

#include <glm/glm.hpp>
#include "components/simple_scene.h"
#include <memory>
#include <vector>

class Tank;

class Projectile {
public:
    Projectile(glm::vec2 position, glm::vec2 velocity);

    void Update(float deltaTime);

    // Draws the projectile using the provided transformation matrix and shader
    void DrawProjectile(glm::mat3 visMatrix, Shader* shader, std::function<void(Mesh*, Shader*, const glm::mat3&)> RenderMesh2D);

    // Collision checking functions
    bool CheckCollisionWithTerrain(const std::vector<float>& heightMap, float segmentWidth);
    bool CheckCollisionWithTank(Tank* tank);

    bool HasCollided() const { return collided; }

    bool IsOffScreen();

    // Getter for the current position of the projectile
    glm::vec2 GetPosition() const { return position; }

    bool IsActive() const { return active; }

    float GetRadius() const { return radius; }

    // Deactivates the projectile, setting it as inactive
    void SetInactive() { active = false; }

private:
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec2 acceleration;

    bool active;    // Indicates if the projectile is currently active
    bool collided = false;
    float radius;

    std::unique_ptr<Mesh> projectileMesh;
};
