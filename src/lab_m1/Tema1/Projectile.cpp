#include "Projectile.h"
#include "Tank.h"
#include "transform2D1.h"
#include "core/engine.h"
#include "utils/gl_utils.h"

Projectile::Projectile(glm::vec2 position, glm::vec2 velocity)
    : position(position), velocity(velocity), active(true), collided(false) {
    // Set the gravitational acceleration (applied on the Y-axis)
    acceleration = glm::vec2(0.0f, -200.0f); // Adjust this value to control the effect of gravity

    glm::vec3 color(1.0f, 0.0f, 0.0f);
    projectileMesh = std::unique_ptr<Mesh>(
        object2D1::CreateCircle("projectile", glm::vec3(0, 0, 0), 5.0f, color, true));

    radius = 5.0f;
}

bool Projectile::CheckCollisionWithTank(Tank* tank) {
    if (!tank)
        return false;

    // Get tank position and collision radius
    glm::vec2 tankPosition(tank->GetX(), tank->GetY());
    float tankRadius = tank->GetCollisionRadius();

    // Calculate distance between projectile and tank
    float distance = glm::length(position - tankPosition);

    if (distance <= tankRadius + radius) {
        collided = true;   // Mark projectile as collided
        active = false;    // Deactivate projectile
        tank->TakeDamage(20.0f); // Apply damage to the tank
        return true;

    }
    return false;
}

void Projectile::Update(float deltaTime) {
    // If the projectile is inactive, skip updating it
    if (!active) 
        return;

    // Update the position based on current velocity and time elapsed
    position += velocity * deltaTime;
    velocity += acceleration * deltaTime;

    // If the projectile falls below the screen, deactivate it
    if (position.y < 0) {
        active = false;
    }
}

void Projectile::DrawProjectile(glm::mat3 visMatrix, Shader* shader,
                                std::function<void(Mesh*, Shader*, const glm::mat3&)> RenderMesh2D) {
    // Skip drawing if the projectile is inactive
    if (!active) 
        return;

    // Apply a translation transformation to move the projectile to its current position
    glm::mat3 modelMatrix = visMatrix * transform2D1::Translate(position.x, position.y);

    // Render the projectile mesh using the calculated transformations
    RenderMesh2D(projectileMesh.get(), shader, modelMatrix);
}


bool Projectile::CheckCollisionWithTerrain(const std::vector<float>& heightMap, float segmentWidth) {
    // Determine the terrain segment below the projectile based on its X position
    int segmentIndex = static_cast<int>(position.x / segmentWidth);

    // Check if the calculated index is within the bounds of the height map
    if (segmentIndex < 0 || segmentIndex >= heightMap.size() - 1) {
        return false; // No collision if out of bounds
    }

    // Get the X positions and heights of the two adjacent terrain points
    float xA = segmentIndex * segmentWidth;
    float xB = (segmentIndex + 1) * segmentWidth;
    float yA = heightMap[segmentIndex];
    float yB = heightMap[segmentIndex + 1];

    // Interpolate between the two terrain heights to find the terrain height at the projectile's X position
    float t = (position.x - xA) / (xB - xA);
    float terrainY = yA + t * (yB - yA);

    // Check if the projectile is below or at the terrain height
    if (position.y - radius <= terrainY) {
        collided = true;
        active = false;
        return true;
    }

    return false;
}

bool Projectile::IsOffScreen() {
    float screenWidth = 1280.0f;
    float screenHeight = 720.0f;

    // Check if the projectile is outside the screen area
    if (position.x < 0 || position.x > screenWidth || position.y < 0 || position.y > screenHeight) {
        return true;
    }
    return false;
}
