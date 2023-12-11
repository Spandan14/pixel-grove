#include "terrain.h"

#include <cmath>
#include "glm/glm.hpp"
#include <utility>

// Constructor
TerrainGenerator::TerrainGenerator()
{
    // Task 8: turn off wireframe shading

    // Define resolution of terrain generation
    m_resolution = 100;

    // Generate random vector lookup table
    m_lookupSize = 1024;
    m_randVecLookup.reserve(m_lookupSize);

    // Initialize random number generator
    std::srand(1230);

    // Populate random vector lookup table
    for (int i = 0; i < m_lookupSize; i++)
    {
        m_randVecLookup.push_back(glm::vec2(std::rand() * 2.0 / RAND_MAX - 1.0,
                                            std::rand() * 2.0 / RAND_MAX - 1.0));
    }
}

// Destructor
TerrainGenerator::~TerrainGenerator()
{
    m_randVecLookup.clear();
}

// Helper for generateTerrain()
void addPointToVector(glm::vec3 point, std::vector<float>& vector) {
    vector.push_back(point.y);
    vector.push_back(point.z);
    vector.push_back(point.x);
}

// Generates the geometry of the output triangle mesh
std::vector<float> TerrainGenerator::generateTerrain() {
    std::vector<float> verts;
    verts.reserve(m_resolution * m_resolution * 6);

    for(int x = 0; x < m_resolution; x++) {
        for(int y = 0; y < m_resolution; y++) {
            int x1 = x;
            int y1 = y;

            int x2 = x + 1;
            int y2 = y + 1;

            glm::vec3 p1 = getPosition(x1,y1);
            glm::vec3 p2 = getPosition(x2,y1);
            glm::vec3 p3 = getPosition(x2,y2);
            glm::vec3 p4 = getPosition(x1,y2);

            glm::vec3 n1 = getNormal(x1,y1);
            glm::vec3 n2 = getNormal(x2,y1);
            glm::vec3 n3 = getNormal(x2,y2);
            glm::vec3 n4 = getNormal(x1,y2);

            // tris 1
            // x1y1z1
            // x2y1z2
            // x2y2z3
            addPointToVector(p1, verts);
            addPointToVector(n1, verts);
            addPointToVector(getColor(n1, p1), verts);

            addPointToVector(p2, verts);
            addPointToVector(n2, verts);
            addPointToVector(getColor(n2, p2), verts);

            addPointToVector(p3, verts);
            addPointToVector(n3, verts);
            addPointToVector(getColor(n3, p3), verts);

            // tris 2
            // x1y1z1
            // x2y2z3
            // x1y2z4
            addPointToVector(p1, verts);
            addPointToVector(n1, verts);
            addPointToVector(getColor(n1, p1), verts);

            addPointToVector(p3, verts);
            addPointToVector(n3, verts);
            addPointToVector(getColor(n3, p3), verts);

            addPointToVector(p4, verts);
            addPointToVector(n4, verts);
            addPointToVector(getColor(n4, p4), verts);
        }
    }
    return verts;
}

// Samples the (infinite) random vector grid at (row, col)
glm::vec2 TerrainGenerator::sampleRandomVector(int row, int col)
{
    std::hash<int> intHash;
    int index = intHash(row * 41 + col * 43) % m_lookupSize;
    return m_randVecLookup.at(index);
}

// Takes a grid coordinate (row, col), [0, m_resolution), which describes a vertex in a plane mesh
// Returns a normalized position (x, y, z); x and y in range from [0, 1), and z is obtained from getHeight()
glm::vec3 TerrainGenerator::getPosition(int row, int col) {
    // Normalizing the planar coordinates to a unit square
    // makes scaling independent of sampling resolution.
    float x = 1.0 * row / m_resolution;
    float y = 1.0 * col / m_resolution;
    float z = getHeight(x, y);
    return glm::vec3(x,y,z);
}

// ================== Students, please focus on the code below this point

// Helper for computePerlin() and, possibly, getColor()
float interpolate(float A, float B, float alpha) {
    // Task 4: implement your easing/interpolation function below
    float ease = 3.0f * pow(alpha, 2) - 2.0f * pow(alpha, 3);

    return A + ease * (B - A);
}

// Takes a normalized (x, y) position, in range [0,1)
// Returns a height value, z, by sampling a noise function
float TerrainGenerator::getHeight(float x, float y) {

    // Task 6: modify this call to produce noise of a different frequency
    // float z = computePerlin(x * 5, y * 5) / 2;

    // z *= 2;
    // makes landscape more polar - higher highs, lower lows
    // z *= 0.5f;
    // makes landscape flatter - lower highs, higher lows

    // Task 7: combine multiple different octaves of noise to produce fractal perlin noise
    float z1 = computePerlin(x * 2, y * 2) / 2;
    float z2 = computePerlin(x * 4, y * 4) / 4;
    float z3 = computePerlin(x * 8, y * 8) / 8;
    float z4 = computePerlin(x * 16, y * 16) / 16;

    // Return 0 as placeholder
    return z1 + z2 + z3 + z4;
}

// Computes the normal of a vertex by averaging neighbors
glm::vec3 TerrainGenerator::getNormal(int row, int col) {


    // Task 9: Compute the average normal for the given input indices
    std::vector<std::pair<int, int>> coords = {{-1, -1},
                                               { 0, -1},
                                               { 1, -1},
                                               { 1,  0},
                                               { 1,  1},
                                               { 0,  1},
                                               {-1,  1},
                                               {-1,  0},
                                               {-1, -1}};
    glm::vec3 v = getPosition(row, col);
    glm::vec3 totalNormal = glm::vec3(0, 0, 0);
    for (int i = 0; i < 8; i++) {
        glm::vec3 n1 = getPosition(row + coords[i].first, col + coords[i].second);
        glm::vec3 n2 = getPosition(row + coords[i + 1].first, col + coords[i + 1].second);
        totalNormal = totalNormal + glm::cross(n1 - v, n2 - v);
    }

    // Return up as placeholder
    return glm::normalize(totalNormal);
}

// Computes color of vertex using normal and, optionally, position
glm::vec3 TerrainGenerator::getColor(glm::vec3 normal, glm::vec3 position) {
    // Task 10: compute color as a function of the normal and position
    float color = position.z > -0.05 ? 1.0f : 0.5f;

    // Return white as placeholder
    return glm::vec3(color, color, color);
}

// Computes the intensity of Perlin noise at some point
float TerrainGenerator::computePerlin(float x, float y) {
    // Task 1: get grid indices (as ints)
    int topLeftX = floor(x);
    int topLeftY = floor(y);

    // Task 2: compute offset vectors
    glm::vec2 topLeftOffset = glm::vec2(x - (float) topLeftX, y - (float) topLeftY);
    glm::vec2 topRightOffset = glm::vec2(x - (float) topLeftX - 1, y - (float) topLeftY);
    glm::vec2 bottomRightOffset = glm::vec2(x - (float) topLeftX - 1, y - (float) topLeftY - 1);
    glm::vec2 bottomLeftOffset = glm::vec2(x - (float) topLeftX, y - (float) topLeftY - 1);

    // Task 3: compute the dot product between the grid point direction vectors and its offset vectors
    float A = glm::dot(sampleRandomVector(topLeftX, topLeftY), topLeftOffset);
    float B = glm::dot(sampleRandomVector(topLeftX + 1, topLeftY), topRightOffset);
    float C = glm::dot(sampleRandomVector(topLeftX + 1, topLeftY + 1), bottomRightOffset);
    float D = glm::dot(sampleRandomVector(topLeftX, topLeftY + 1), bottomLeftOffset);

    // Task 5: Debug this line to properly use your interpolation function to produce the correct value
    return interpolate(interpolate(A, B, x - topLeftX), interpolate(D, C, x - topLeftX), y - topLeftY);

    // need to interpolate based on the coordinates, bug fixed

}//