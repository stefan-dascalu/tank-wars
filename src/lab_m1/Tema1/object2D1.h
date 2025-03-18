#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"
#include "transform2D1.h"

namespace object2D1 {

    // Create square with given bottom left corner, length and color
    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);

    // Creates a line between two points with the given color
    Mesh* CreateLine(std::string name, glm::vec2 p1, glm::vec2 p2, glm::vec3 color);

    // Creates a circle with the specified center, radius, and color
    Mesh* CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill = true);

    // Creates a rectangle with the specified corner, width, height, and color
    Mesh* CreateRectangle(const std::string& name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill = false);

    // Creates a trapezoid with specified bottom width, top width, height, and color
    Mesh* CreateTrapezoid(const std::string& name, glm::vec3 leftBottomCorner, float bottomWidth, float topWidth, float height, glm::vec3 color, bool fill = true);

    // Creates an arc with the specified center, radius, start angle, end angle, and color
    Mesh* CreateArc(const std::string& name, glm::vec3 center, float radius, float startAngle, float endAngle, glm::vec3 color);
}
