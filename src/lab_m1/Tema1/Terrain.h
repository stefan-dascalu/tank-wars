#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <glm/glm.hpp>
#include "components/simple_scene.h"

class Shader;

class Terrain {
public:
    Terrain(float width, float height, int pointsCount);

    // Draws the terrain using the provided rendering function
    void DrawTerrain(glm::mat3 visMatrix, std::unordered_map<std::string, Shader*> shaders, std::function<void(Mesh*, Shader*, const glm::mat3&)> RenderMesh2D);

    void UpdateDimensions(float newWidth, float newHeight);

    // Deforms the terrain at a given position with a specified radius
    void DeformTerrain(const glm::vec2& position, float radius);

    // Getters for terrain properties
    float GetTerrainHeight(float x) const;
    float GetTerrainAngle(float x) const;
    float GetSegmentWidth() const { return width / static_cast<float>(heightMap.size() - 1); }
    const std::vector<float>& GetHeightMap() const { return heightMap; }

private:
    float width;
    float height;
    int pointsCount;                 // Number of points in the terrain
    std::vector<float> heightMap; 
    std::unique_ptr<Mesh> terrainMesh;

    // Generates the height map based on a sine wave function
    void GenerateHeightMap(int numPoints, float maxX);

    void GenerateTerrain();

    void GenerateTerrainMesh();

    // Custom function to calculate height using sine waves
    float terrainFunction(float x) const;
};
