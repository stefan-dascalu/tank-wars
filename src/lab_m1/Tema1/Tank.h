#pragma once

#include <vector>
#include <memory>

#include "components/simple_scene.h"
#include "Projectile.h"

class Projectile;

class Tank {
public:
    Tank(float initialX, glm::vec3 baseColor);

    // Function to shoot a projectile from the tank's cannon
    std::shared_ptr<Projectile> Shoot();

    // Updates the tank's position and angle based on the terrain
    void UpdatePosition(const std::vector<float>& heightMap, float segmentWidth);

    void Move(float deltaX);

    // Rotates the turret by a specific angle offset
    void RotateTurret(float angleOffset);

    // Sets the turret to a specific angle
    void SetTurretAngle(float angle);

    // Draws the tank using the provided rendering function
    void DrawTank(glm::mat3 visMatrix, Shader* shader,
                  std::function<void(Mesh*, Shader*, const glm::mat3&)> RenderMesh2D);

    // Sets the current height map for the tank
    void SetHeightMap(const std::vector<float>& heightMap, float segmentWidth);

    // Reduces the tank's health by a specified damage value
    void TakeDamage(float damage);

    // Checks if the tank is still active (not destroyed)
    bool IsActive() const;

    // Getter functions
    float GetHealth() const { return health; }
    float GetX() const { return x; }
    float GetY() const { return y; }
    float GetCollisionRadius() const { return collisionRadius; }
    float GetAngle() const { return angle; }

    // Generates trajectory points for a projectile shot
    std::vector<glm::vec2> GetTrajectoryPoints(float initialSpeed, float timeStep, int numPoints);

    // Gets the position of the tip of the gun
    glm::vec2 GetGunTipPosition() const;

    // Gets the total turret angle (including tank base rotation)
    float GetTotalTurretAngle() const;

private:
    void InitMeshes();

    std::vector<float> currentHeightMap;       
    std::unique_ptr<Mesh> trapezoid1Mesh;      
    std::unique_ptr<Mesh> trapezoid2Mesh;      
    std::unique_ptr<Mesh> turretMesh;          
    std::unique_ptr<Mesh> gunMesh;   
    glm::vec3 baseColor;     

    float x, y;                                // Tank's current position               
    float baseHeight;                          
    float turretRadius;                        
    float turretAngle;                         
    float trapezoidHeight;                     
    float angle;                               // Angle of the tank based on terrain slope
    bool active = true;                        // Indicates if the tank is still active
    float currentSegmentWidth;                 
    float gunLength;                           
    float health = 100.0f;                     // Tank's health
    float collisionRadius = 15.0f;             // Radius used for collision detection
    float upperTrapezoidY;
    float turretCenterY;
};
