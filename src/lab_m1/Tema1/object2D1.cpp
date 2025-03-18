#include "object2D1.h"
#include <vector>
#include "core/engine.h"
#include "utils/gl_utils.h"

namespace object2D1 {

Mesh* CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

// Creates a line segment between two points with a specified color
Mesh* CreateLine(std::string name, glm::vec2 p1, glm::vec2 p2, glm::vec3 color) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(p1.x, p1.y, 0), color),
        VertexFormat(glm::vec3(p2.x, p2.y, 0), color)
    };

    std::vector<unsigned int> indices = { 0, 1 };

    Mesh* line = new Mesh(name);
    line->SetDrawMode(GL_LINES);
    line->InitFromData(vertices, indices);
    return line;
}

// Creates a rectangle mesh with the specified bottom-left corner, width, height, and color
Mesh* CreateRectangle(const std::string& name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill) {
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices = {
        VertexFormat(corner, color),                                          // Bottom-left
        VertexFormat(corner + glm::vec3(width, 0, 0), color),                 // Bottom-right
        VertexFormat(corner + glm::vec3(width, height, 0), color),            // Top-right
        VertexFormat(corner + glm::vec3(0, height, 0), color)                 // Top-left
    };

    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices;

    if (!fill) {
        // If not filled, draw only the outline using a line loop
        rectangle->SetDrawMode(GL_LINE_LOOP);
        indices = { 0, 1, 2, 3 };
    } else {
        // If filled, draw two triangles to form the rectangle
        indices = { 0, 1, 2, 0, 2, 3 };
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}

// Creates a circle mesh with the specified center, radius, and color
Mesh* CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill) {
    int numTriangles = 50; // Number of triangles to approximate the circle
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Add the center vertex of the circle
    vertices.push_back(VertexFormat(center, color));

    for (int i = 0; i <= numTriangles; ++i) {
        float angle = 2.0f * M_PI * i / numTriangles;
        float x = center.x + radius * cos(angle); // X coordinate
        float y = center.y + radius * sin(angle); // Y coordinate
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));

        if (i > 0) {
            indices.push_back(0);     // Center of the circle
            indices.push_back(i);     // Current point on the circumference
            indices.push_back(i + 1); // Next point on the circumference
        }
    }

    Mesh* circle = new Mesh(name);

    if (!fill) {
        // If not filled, draw only the outline of the circle
        circle->SetDrawMode(GL_LINE_LOOP);
        indices.clear();
        for (int i = 1; i <= numTriangles + 1; ++i) {
            indices.push_back(i);
        }
    }

    circle->InitFromData(vertices, indices);
    return circle;
}

// Creates a trapezoid using the specified bottom-left corner, bottom width, top width, height, and color
Mesh* CreateTrapezoid(const std::string& name, glm::vec3 leftBottomCorner, float bottomWidth, float topWidth, float height, glm::vec3 color, bool fill) {
    glm::vec3 corner = leftBottomCorner;

    // Define vertices for the trapezoid
    std::vector<VertexFormat> vertices = {
        VertexFormat(corner, color),                                                                                // Bottom-left
        VertexFormat(corner + glm::vec3(bottomWidth, 0, 0), color),                                                 // Bottom-right
        VertexFormat(corner + glm::vec3(bottomWidth - (bottomWidth - topWidth) / 2, height, 0), color),             // Top-right
        VertexFormat(corner + glm::vec3((bottomWidth - topWidth) / 2, height, 0), color)                            // Top-left
    };

    Mesh* trapezoid = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

    // If not filled, draw only the outline
    if (!fill) {
        trapezoid->SetDrawMode(GL_LINE_LOOP);
    }

    trapezoid->InitFromData(vertices, indices);
    
    return trapezoid;
}

// Creates an arc with the specified center, radius, start angle, end angle, and color
Mesh* CreateArc(const std::string& name, glm::vec3 center, float radius, float startAngle, float endAngle, glm::vec3 color) {
    int numSegments = 50;
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Add the center vertex
    vertices.push_back(VertexFormat(center, color)); 

    // Generate vertices along the arc
    for (int i = 0; i <= numSegments; ++i) {
        float angle = startAngle + (endAngle - startAngle) * i / numSegments;
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
        indices.push_back(i);
    }

    Mesh* arc = new Mesh(name);
    arc->SetDrawMode(GL_TRIANGLE_FAN);
    arc->InitFromData(vertices, indices);

    return arc;
}

} // namespace object2D1

