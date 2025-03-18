#include "Tank.h"
#include "Projectile.h"
#include "transform2D1.h"
#include "object2D1.h"

#include "core/engine.h"
#include "utils/gl_utils.h"

#include <cmath>
#include <iostream>

Tank::Tank(float initialX, glm::vec3 baseColor)
    : x(initialX), y(0), turretAngle(0.0f), angle(0.0f), baseColor(baseColor) {
    InitMeshes();
}

void Tank::InitMeshes() {
    glm::vec3 turretColor = baseColor * 1.2f;
    glm::vec3 gunColor = baseColor * 0.8f;

    // Define tank dimensions
    float baseWidth = 30.0f;
    baseHeight = 15.0f;
    float trapezoidHeight = baseHeight / 2.0f;
    float overlapFactor = 0.3f;

    turretRadius = 7.0f;
    gunLength = 25.0f;
    float gunWidth = 2.0f;

    // Create the lower trapezoid (base)
    float bottomTrapezoidTopWidth = baseWidth * 1.2f;
    trapezoid1Mesh = std::unique_ptr<Mesh>(
        object2D1::CreateTrapezoid("trapezoid1", glm::vec3(-baseWidth / 2, 0.0f, 0.0f), baseWidth, bottomTrapezoidTopWidth, trapezoidHeight, baseColor, true));

    // Calculate Y position for the upper trapezoid with overlap
    upperTrapezoidY = trapezoidHeight * (1.0f - overlapFactor);

    // Create the upper trapezoid (slightly taller)
    float topTrapezoidBottomWidth = bottomTrapezoidTopWidth;
    float topTrapezoidTopWidth = bottomTrapezoidTopWidth * 1.1f;
    trapezoid2Mesh = std::unique_ptr<Mesh>(
        object2D1::CreateTrapezoid("trapezoid2", glm::vec3(-topTrapezoidTopWidth / 2, upperTrapezoidY, 0.0f), topTrapezoidTopWidth, topTrapezoidBottomWidth, trapezoidHeight * 1.1f, baseColor, true));

    // Create the turret as an upward-facing semicircle
    turretCenterY = upperTrapezoidY + trapezoidHeight * 1.1f;
    float startAngle = glm::radians(180.0f);
    float endAngle = glm::radians(0.0f);
    turretMesh = std::unique_ptr<Mesh>(
        object2D1::CreateArc("turret", glm::vec3(0.0f, turretCenterY, 0.0f), turretRadius, startAngle, endAngle, turretColor));

    // Create the gun as a rectangle
    std::vector<VertexFormat> gunVertices = {
        VertexFormat(glm::vec3(-gunWidth / 2, 0.0f, 0.0f), gunColor),
        VertexFormat(glm::vec3(gunWidth / 2, 0.0f, 0.0f), gunColor),
        VertexFormat(glm::vec3(gunWidth / 2, gunLength, 0.0f), gunColor),
        VertexFormat(glm::vec3(-gunWidth / 2, gunLength, 0.0f), gunColor)
    };

    std::vector<unsigned int> gunIndices = {0, 1, 2, 0, 2, 3};
    gunMesh = std::unique_ptr<Mesh>(new Mesh("gun"));
    gunMesh->InitFromData(gunVertices, gunIndices);
}


void Tank::DrawTank(glm::mat3 visMatrix, Shader* shader,
                    std::function<void(Mesh*, Shader*, const glm::mat3&)> RenderMesh2D) {
    // Transformation matrix for the tank
    glm::mat3 tankMatrix = visMatrix * transform2D1::Translate(x, y) * transform2D1::Rotate(angle);

    // Draw the lower trapezoid (base)
    RenderMesh2D(trapezoid1Mesh.get(), shader, tankMatrix);

    // Draw the upper trapezoid
    RenderMesh2D(trapezoid2Mesh.get(), shader, tankMatrix);

    // Draw the turret
    float turretOffsetY = upperTrapezoidY + trapezoidHeight * 1.1f;
    glm::mat3 turretMatrix = tankMatrix;
    RenderMesh2D(turretMesh.get(), shader, turretMatrix);

    // Draw the gun with rotation
    glm::mat3 gunMatrix = turretMatrix
                          * transform2D1::Translate(0, turretOffsetY + turretRadius)
                          * transform2D1::Rotate(turretAngle);
    RenderMesh2D(gunMesh.get(), shader, gunMatrix);

    // Draw the health bar
    float healthBarWidth = 40.0f;
    float healthBarHeight = 5.0f;
    float healthPercentage = health / 100.0f;
    float healthBarYOffset = turretOffsetY + turretRadius + 22;

    // Frame of the health bar
    glm::mat3 healthBarFrameMatrix = visMatrix * transform2D1::Translate(x - healthBarWidth / 2, y + healthBarYOffset);
    RenderMesh2D(object2D1::CreateRectangle("healthFrame", glm::vec3(0, 0, 0), healthBarWidth, healthBarHeight, glm::vec3(0, 0, 0), false), shader, healthBarFrameMatrix);

    // Filled health bar
    glm::mat3 healthBarMatrix = visMatrix * transform2D1::Translate(x - healthBarWidth / 2, y + healthBarYOffset);
    RenderMesh2D(object2D1::CreateRectangle("healthBar", glm::vec3(0, 0, 0), healthBarWidth * healthPercentage, healthBarHeight, glm::vec3(1 - healthPercentage, healthPercentage, 0), true), shader, healthBarMatrix);
}


void Tank::TakeDamage(float damage) {
    // If the tank is already inactive, ignore the damage
    if (!active) 
        return;

    // Decrease the tank's health by the given damage amount
    health -= damage;

    // Ensure health does not drop below zero
    if (health < 0) 
        health = 0;

    // If health reaches zero, deactivate the tank
    if (health == 0) 
        active = false;
}


void Tank::SetHeightMap(const std::vector<float>& heightMap, float segmentWidth) {
    // Store the current height map and the width of each terrain segment
    currentHeightMap = heightMap;
    currentSegmentWidth = segmentWidth;
}


void Tank::RotateTurret(float angleOffset) {
    // Adjust the turret's angle by the specified offset
    turretAngle += angleOffset;

    // Limit the turret angle to a range between -60 and +60 degrees
    const float maxTurretAngle = glm::radians(60.0f);
    if (turretAngle > maxTurretAngle) {
        turretAngle = maxTurretAngle;
    } else if (turretAngle < -maxTurretAngle) {
        turretAngle = -maxTurretAngle;
    }
}


void Tank::SetTurretAngle(float angle) {
    // Set the turret's angle, ensuring it stays within the allowed range
    const float maxTurretAngle = glm::radians(60.0f);

    if (angle > maxTurretAngle) {
        turretAngle = maxTurretAngle;
    } else if (angle < -maxTurretAngle) {
        turretAngle = -maxTurretAngle;
    } else {
        turretAngle = angle;
    }
}


void Tank::Move(float deltaX) {
    // Adjust the tank's X position by the given amount
    x += deltaX;

    // Update the tank's Y position and orientation based on the terrain
    UpdatePosition(currentHeightMap, currentSegmentWidth);
}


bool Tank::IsActive() const {
    return active;
}


void Tank::UpdatePosition(const std::vector<float>& heightMap, float segmentWidth) {
    // Calculate the terrain segment index directly under the tank
    int segmentIndex = static_cast<int>(x / segmentWidth);

    // Check if the index is valid within the height map range
    if (segmentIndex >= 0 && segmentIndex < heightMap.size() - 1) {
        // Get the X positions and heights of two adjacent terrain points
        float xA = segmentIndex * segmentWidth;
        float xB = (segmentIndex + 1) * segmentWidth;
        float yA = heightMap[segmentIndex];
        float yB = heightMap[segmentIndex + 1];

        // Interpolate to find the Y position at the tank's X position
        float t = (x - xA) / (xB - xA);
        float terrainY = yA + t * (yB - yA);

        // Calculate the angle based on the slope of the terrain
        float dx = xB - xA;
        float dy = yB - yA;
        angle = -atan2(dy, dx);

        // Update the tank's Y position to match the terrain
        y = terrainY;
    }
}

std::shared_ptr<Projectile> Tank::Shoot() {
    // Check if the tank is active before shooting
    if (!active) {
        return nullptr;
    }
    
    glm::vec2 gunTip = GetGunTipPosition();

    float projectileSpeed = 300.0f;

    float angleTotal = angle + turretAngle;

    // Calculate the initial velocity of the projectile based on the angle
    glm::vec2 velocity = glm::vec2(sin(angleTotal), cos(angleTotal)) * projectileSpeed;

    // Create a projectile at the gun tip position with the calculated velocity
    return std::make_shared<Projectile>(gunTip, velocity);
}


std::vector<glm::vec2> Tank::GetTrajectoryPoints(float initialSpeed, float timeStep, int numPoints) {
    std::vector<glm::vec2> points;

    // Calculate the total angle (tank angle + turret angle)
    float angleTotal = angle + turretAngle;

    // Get the initial position of the projectile (gun tip)
    glm::vec2 initialPosition = GetGunTipPosition();

    // Calculate the initial velocity of the projectile
    glm::vec2 velocity = glm::vec2(sin(angleTotal), cos(angleTotal)) * initialSpeed;

    // Acceleration due to gravity
    glm::vec2 acceleration(0.0f, -200.0f);

    // Current position of the projectile
    glm::vec2 position = initialPosition;

    // Generate trajectory points
    for (int i = 0; i < numPoints; ++i) {
        // Update the position based on velocity and time step
        position += velocity * timeStep;
        velocity += acceleration * timeStep;
        points.push_back(position);

        // Stop if the projectile hits the ground
        if (position.y < 0) break;
    }

    return points;
}

glm::vec2 Tank::GetGunTipPosition() const {
    float angleTotal = angle + turretAngle;

    // Base position of the turret
    float turretBaseX = x;
    float turretBaseY = y + upperTrapezoidY + trapezoidHeight * 1.1f + turretRadius;

    // Calculate the position of the gun tip based on the gun's length and angle
    glm::vec2 gunTip(
        turretBaseX + gunLength * sin(angleTotal),
        turretBaseY + gunLength * cos(angleTotal)
    );

    return gunTip;
}

float Tank::GetTotalTurretAngle() const {
    return angle + turretAngle;
}
