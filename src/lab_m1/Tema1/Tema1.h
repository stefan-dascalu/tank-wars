#pragma once

#include "components/simple_scene.h"
#include "Terrain.h"
#include "Tank.h"
#include "Explosion.h"
#include "Cloud.h"
#include <memory>
#include <vector>
#include <list>

namespace m1 {
    class Tema1 : public gfxc::SimpleScene {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void DrawTrajectory(const std::vector<glm::vec2>& trajectoryPoints, const glm::mat3& visMatrix);

        // Game objects
        std::unique_ptr<Terrain> terrain;            // The terrain object
        std::unique_ptr<Tank> tank;                  // The player's tank
        std::unique_ptr<Tank> enemyTank;             // The enemy's tank
        std::list<std::shared_ptr<Projectile>> projectiles; // Active projectiles
        std::list<std::shared_ptr<Explosion>> explosions;   // Active explosions
        std::vector<std::shared_ptr<Cloud>> clouds;           // Background clouds
        
        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;
    };
    
}   // namespace m1
