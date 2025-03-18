#include "Explosion.h"

#include "transform2D1.h"
#include "object2D1.h"

Explosion::Explosion(glm::vec2 position)
    : position(position), timeElapsed(0.0f), duration(1.0f), active(true) {
    // Create a circle mesh to represent the explosion
    explosionMesh = std::unique_ptr<Mesh>(object2D1::CreateCircle("explosion", glm::vec3(0, 0, 0),
                                                                 10.0f, glm::vec3(1, 0.5f, 0), true));
}

void Explosion::Update(float deltaTime) {
    // Update the elapsed time
    timeElapsed += deltaTime;

    // Deactivate the explosion after its duration
    if (timeElapsed >= duration) {
        active = false;
    }
}

void Explosion::DrawExplosion(glm::mat3 visMatrix, Shader* shader,
                              std::function<void(Mesh*, Shader*, const glm::mat3&)> RenderMesh2D) {
    // Calculate scaling factor based on elapsed time
    float scale = timeElapsed / duration * 2.0f; // Explosion grows twice its size

    // Apply transformations to position and scale the explosion
    glm::mat3 modelMatrix = visMatrix * transform2D1::Translate(position.x, position.y) * transform2D1::Scale(scale, scale);
                                        
    // Render the explosion mesh
    RenderMesh2D(explosionMesh.get(), shader, modelMatrix);
}
