#include "Tema1.h"

#include <vector>
#include <iostream>

#include "transform2D1.h"
#include "object2D1.h"

#include "Projectile.h"
#include "Terrain.h"
#include "Tank.h"
#include "Explosion.h"
#include "Cloud.h"

using namespace std;
using namespace m1;

Tema1::Tema1() {
}

Tema1::~Tema1() {
}

void Tema1::Init() {
    glm::ivec2 resolution = window->GetResolution();

    // Set up the camera with an orthographic projection
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, resolution.x, 0, resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    // Initialize the terrain with given width, height, and number of segments
    terrain = std::unique_ptr<Terrain>(new Terrain(resolution.x, resolution.y / 2, 200));

    glm::vec3 playerTankColor(0.2f, 0.5f, 0.2f);
    glm::vec3 enemyTankColor(0.5f, 0.3f, 0.1f);

    // Initialize tanks with their respective colors and positions
    tank = std::unique_ptr<Tank>(new Tank(50, playerTankColor));
    enemyTank = std::unique_ptr<Tank>(new Tank(resolution.x - 100, enemyTankColor));

    // Add clouds to the scene at specified positions and speeds
    clouds.push_back(std::make_shared<Cloud>(200.0f, 600.0f, 20.0f));
    clouds.push_back(std::make_shared<Cloud>(600.0f, 635.0f, 15.0f));
    clouds.push_back(std::make_shared<Cloud>(1000.0f, 620.0f, 25.0f));
}

void Tema1::FrameStart() {
    // Clear the color and depth buffers
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();

    // Set the viewport to cover the entire window
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds) {
    // Create the base transformation matrix (identity matrix)
    glm::mat3 visMatrix = glm::mat3(1);

    // Obtain the height map and segment width from the terrain
    float segmentWidth = terrain->GetSegmentWidth();
    const std::vector<float>& heightMap = terrain->GetHeightMap();

    // Update the player's tank position and orientation based on the terrain
    tank->SetHeightMap(heightMap, segmentWidth);
    tank->UpdatePosition(heightMap, segmentWidth);

    // Update the enemy tank's position and orientation based on the terrain
    enemyTank->SetHeightMap(heightMap, segmentWidth);
    enemyTank->UpdatePosition(heightMap, segmentWidth);

    // Draw the terrain
    terrain->DrawTerrain(visMatrix, shaders, [this](Mesh* mesh, Shader* shader, const glm::mat3& modelMatrix) {
        RenderMesh2D(mesh, shader, modelMatrix);
    });

    // Draw the player's tank
    if (tank->IsActive()) {
        tank->DrawTank(visMatrix, shaders["VertexColor"], [this](Mesh* mesh, Shader* shader, const glm::mat3& modelMatrix) {
            RenderMesh2D(mesh, shader, modelMatrix);
        });

        // Calculate and draw the player's tank trajectory
        std::vector<glm::vec2> trajectoryPoints = tank->GetTrajectoryPoints(300.0f, 0.05f, 100);
        DrawTrajectory(trajectoryPoints, glm::mat3(1));
    }

    // Draw the enemy tank
    if (enemyTank->IsActive()) {
        enemyTank->DrawTank(visMatrix, shaders["VertexColor"], [this](Mesh* mesh, Shader* shader, const glm::mat3& modelMatrix) {
            RenderMesh2D(mesh, shader, modelMatrix);
        });

        // Calculate and draw the enemy tank trajectory
        std::vector<glm::vec2> enemyTrajectoryPoints = enemyTank->GetTrajectoryPoints(300.0f, 0.05f, 100);
        DrawTrajectory(enemyTrajectoryPoints, glm::mat3(1));
    }

    // Handle projectiles
    for (auto& projectile : projectiles) {
        if (!projectile->IsActive()) continue;

        // Update projectile position
        projectile->Update(deltaTimeSeconds);

        // Check collision with the player's tank
        if (projectile->CheckCollisionWithTank(tank.get())) {
            auto explosion = std::make_shared<Explosion>(projectile->GetPosition());
            explosions.push_back(explosion);
        }

        // Check collision with the enemy tank
        if (projectile->CheckCollisionWithTank(enemyTank.get())) {
            auto explosion = std::make_shared<Explosion>(projectile->GetPosition());
            explosions.push_back(explosion);
        }

        // Check collision with terrain or if the projectile is off-screen
        if (projectile->CheckCollisionWithTerrain(heightMap, segmentWidth) || projectile->IsOffScreen()) {
            if (projectile->HasCollided()) {
                auto explosion = std::make_shared<Explosion>(projectile->GetPosition());
                explosions.push_back(explosion);
                terrain->DeformTerrain(projectile->GetPosition(), 30.0f);
            }
            projectile->SetInactive();
        }
    }

    // Draw active projectiles
    for (const auto& projectile : projectiles) {
        if (projectile->IsActive()) {
            projectile->DrawProjectile(visMatrix, shaders["VertexColor"], [this](Mesh* mesh, Shader* shader, const glm::mat3& modelMatrix) {
                RenderMesh2D(mesh, shader, modelMatrix);
            });
        }
    }

    // Update explosions
    for (auto& explosion : explosions) {
        explosion->Update(deltaTimeSeconds);
    }

    // Draw active explosions
    for (const auto& explosion : explosions) {
        if (explosion->IsActive()) {
            explosion->DrawExplosion(visMatrix, shaders["VertexColor"], [this](Mesh* mesh, Shader* shader, const glm::mat3& modelMatrix) {
                RenderMesh2D(mesh, shader, modelMatrix);
            });
        }
    }

    // Update and draw clouds
    for (auto& cloud : clouds) {
        cloud->Update(deltaTimeSeconds);
        cloud->DrawCloud(visMatrix, shaders["VertexColor"], [this](Mesh* mesh, Shader* shader, const glm::mat3& modelMatrix) {
            RenderMesh2D(mesh, shader, modelMatrix);
        });
    }
}

void Tema1::FrameEnd() {
    // No additional operations at the end of the frame
}

/*
 *  Input handling functions.
 */

void Tema1::OnInputUpdate(float deltaTime, int mods) {
    // Player's tank movement with 'A' and 'D' keys
    if (window->KeyHold(GLFW_KEY_A)) {
        tank->Move(-50.0f * deltaTime); // Move left
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        tank->Move(50.0f * deltaTime); // Move right
    }

    // Player's turret rotation with 'W' and 'S' keys
    if (window->KeyHold(GLFW_KEY_W)) {
        tank->RotateTurret(-1.5f * deltaTime); // Rotate turret upwards
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        tank->RotateTurret(1.5f * deltaTime); // Rotate turret downwards
    }

    // Enemy tank movement with left and right arrow keys
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        enemyTank->Move(-50.0f * deltaTime); // Move left
    }

    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        enemyTank->Move(50.0f * deltaTime); // Move right
    }

    // Enemy turret rotation with up and down arrow keys
    if (window->KeyHold(GLFW_KEY_UP)) {
        enemyTank->RotateTurret(-1.5f * deltaTime); // Rotate turret upwards
    }

    if (window->KeyHold(GLFW_KEY_DOWN)) {
        enemyTank->RotateTurret(1.5f * deltaTime); // Rotate turret downwards
    }
}

void Tema1::OnKeyPress(int key, int mods) {
    if (key == GLFW_KEY_SPACE) {
        // Fire a projectile from the player's tank
        auto projectile = tank->Shoot();

        if (projectile) {
            projectiles.push_back(projectile);
        }
    }

    if (key == GLFW_KEY_ENTER) {
        // Fire a projectile from the enemy tank
        auto projectile = enemyTank->Shoot();
        
        if (projectile) {
            projectiles.push_back(projectile);
        }
    }
}

void Tema1::OnKeyRelease(int key, int mods) {
    // Handle key release events if needed
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    //
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    //
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
    // Handle mouse button release events if needed
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
    // Handle mouse scroll events if needed
}

void Tema1::OnWindowResize(int width, int height) {
    // Handle window resize events if needed
}


void Tema1::DrawTrajectory(const std::vector<glm::vec2>& trajectoryPoints, const glm::mat3& visMatrix) {
    // Check if there are enough points to draw a trajectory (at least two)
    if (trajectoryPoints.size() < 2) 
        return; // Exit

    // Iterate through the trajectory points to draw lines between consecutive points
    for (size_t i = 0; i < trajectoryPoints.size() - 1; ++i) {
        glm::vec2 p1 = trajectoryPoints[i];
        glm::vec2 p2 = trajectoryPoints[i + 1];

        Mesh* line = object2D1::CreateLine("trajectoryLine", p1, p2, glm::vec3(1, 1, 0));

        RenderMesh2D(line, shaders["VertexColor"], visMatrix);
    }
}

