#include "Terrain.h"
#include "transform2D1.h"
#include "object2D1.h"
#include <cmath>
#include <algorithm>
#include "components/simple_scene.h"

Terrain::Terrain(float width, float height, int pointsCount)
    : width(width), height(height), pointsCount(pointsCount) {
    GenerateTerrain();
}

// Custom function to generate height using a combination of sine waves
float Terrain::terrainFunction(float x) const {
    return 65.0f * sin(x / 80.0f) + 40.0f * sin(x / 200.0f) + 250.0f;
}

// Generates the height map by sampling the custom terrain function
void Terrain::GenerateHeightMap(int numPoints, float maxX) {
    heightMap.clear();
    float step = maxX / (numPoints - 1);

    // Populate the height map with terrain heights
    for (int i = 0; i < numPoints; ++i) {
        float x = i * step;
        float y = terrainFunction(x);
        heightMap.push_back(y);
    }
}

// Main function to generate the terrain (height map + mesh)
void Terrain::GenerateTerrain() {
    GenerateHeightMap(pointsCount, width);
    GenerateTerrainMesh();
}

// Generates the terrain mesh using triangle strips for efficient rendering
void Terrain::GenerateTerrainMesh() {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    float segmentWidth = GetSegmentWidth();

    // Create vertices and indices for the terrain using triangle strips
    for (size_t i = 0; i < heightMap.size(); ++i) {
        float x = i * segmentWidth;
        float y = heightMap[i];
        
        vertices.emplace_back(glm::vec3(x, y, 0), glm::vec3(0.4f, 0.6f, 0.2f)); // Top vertex
        vertices.emplace_back(glm::vec3(x, 0, 0), glm::vec3(0.4f, 0.6f, 0.2f)); // Bottom vertex
    }

    // Generate indices for triangle strip
    for (size_t i = 0; i < heightMap.size() - 1; ++i) {
        indices.push_back(2 * i);
        indices.push_back(2 * i + 1);
        indices.push_back(2 * i + 2);
        indices.push_back(2 * i + 3);
    }

    if (!terrainMesh) {
        terrainMesh.reset(new Mesh("terrain"));
    }

    terrainMesh->SetDrawMode(GL_TRIANGLE_STRIP);
    terrainMesh->InitFromData(vertices, indices);
}

// Draws the terrain using the provided rendering function
void Terrain::DrawTerrain(glm::mat3 visMatrix, std::unordered_map<std::string, Shader*> shaders, std::function<void(Mesh*, Shader*, const glm::mat3&)> RenderMesh2D) {
    RenderMesh2D(terrainMesh.get(), shaders["VertexColor"], visMatrix);
}

// Updates the terrain dimensions and regenerates it (e.g., on window resize)
void Terrain::UpdateDimensions(float newWidth, float newHeight) {
    width = newWidth;
    height = newHeight;

    GenerateTerrain();
}

// Deforms the terrain at a specific position with a given radius (impact of projectile)
void Terrain::DeformTerrain(const glm::vec2& position, float radius) {
    for (size_t i = 0; i < heightMap.size(); ++i) {
        float x = i * GetSegmentWidth();
        float distance = std::abs(x - position.x);

        // Check if the current point is within the deformation radius
        if (distance < radius) {
            float deformation = sqrt(radius * radius - distance * distance);
            heightMap[i] -= deformation;

            // Ensure the terrain height does not go below zero
            heightMap[i] = std::max(0.0f, heightMap[i]);
        }
    }
    GenerateTerrainMesh(); // Update the mesh after deformation
}

// Returns the height of the terrain at a specific x coordinate using linear interpolation
float Terrain::GetTerrainHeight(float x) const {
    int index = static_cast<int>(x / GetSegmentWidth());

    if (index < 0 || index >= heightMap.size() - 1) 
        return 0;

    float x1 = index * GetSegmentWidth();
    float x2 = (index + 1) * GetSegmentWidth();
    float y1 = heightMap[index];
    float y2 = heightMap[index + 1];

    // Perform linear interpolation between the two nearest points
    return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
}

// Returns the angle of the terrain at a specific x coordinate for tank alignment
float Terrain::GetTerrainAngle(float x) const {
    int index = static_cast<int>(x / GetSegmentWidth());

    if (index < 0 || index >= heightMap.size() - 1)
        return 0;

    float y1 = heightMap[index];
    float y2 = heightMap[index + 1];

    return atan2(y2 - y1, GetSegmentWidth());
}
