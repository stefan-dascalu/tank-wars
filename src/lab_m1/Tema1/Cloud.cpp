#include "Cloud.h"

#include "transform2D1.h"
#include "object2D1.h"

Cloud::Cloud(float startX, float startY, float speed)
    : x(startX), y(startY), speed(speed) {
    InitMesh();
}

void Cloud::InitMesh() {
    // Vectors to store the vertices and indices for the cloud mesh
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Define the relative positions for each circle that will form the cloud
    std::vector<glm::vec2> circleOffsets = {
        glm::vec2(-30.0f, 0.0f),  // Left circle
        glm::vec2(-10.0f, 15.0f), // Top-left circle
        glm::vec2(10.0f, 10.0f),  // Top-right circle
        glm::vec2(30.0f, 0.0f)    // Right circle
    };

    glm::vec3 cloudColor(0.9f, 0.9f, 0.9f);
    int numSegments = 20;
    int indexOffset = 0;

    // Loop through each offset to generate a circle at that position
    for (const auto& offset : circleOffsets) {
        // Temporary vectors for storing the vertices and indices of each circle
        std::vector<VertexFormat> circleVertices;
        std::vector<unsigned int> circleIndices;

        // Generate vertices for the circle based on the number of segments
        for (int i = 0; i <= numSegments; ++i) {
            float angle = 2.0f * M_PI * i / numSegments; // Angle in radians
            float x = offset.x + 20.0f * cos(angle);
            float y = offset.y + 20.0f * sin(angle);

            // Add the vertex to the circle with a predefined color
            circleVertices.emplace_back(glm::vec3(x, y, 0), cloudColor);
            circleIndices.push_back(indexOffset + i);
        }

        // Append the circle's vertices and indices to the main mesh vectors
        vertices.insert(vertices.end(), circleVertices.begin(), circleVertices.end());
        indices.insert(indices.end(), circleIndices.begin(), circleIndices.end());

        indexOffset += numSegments + 1;
    }

    // Create a new mesh for the cloud using the vertices and indices generated
    cloudMesh = std::unique_ptr<Mesh>(new Mesh("cloud"));
    cloudMesh->SetDrawMode(GL_TRIANGLE_FAN);
    cloudMesh->InitFromData(vertices, indices);
}

void Cloud::Update(float deltaTime) {
    // Move the cloud to the right based on its speed
    x += speed * deltaTime;

    // If the cloud moves off the right side of the screen, reset it to the left
    if (x > 1280 + 50.0f) {
        x = -50.0f;
    }
}

void Cloud::DrawCloud(glm::mat3 visMatrix, Shader* shader,
                      std::function<void(Mesh*, Shader*, const glm::mat3&)> RenderMesh2D) {
    // Apply translation to move the cloud to its current position
    glm::mat3 modelMatrix = visMatrix * transform2D1::Translate(x, y);

    // Render the cloud mesh with the applied transformations
    RenderMesh2D(cloudMesh.get(), shader, modelMatrix);
}
