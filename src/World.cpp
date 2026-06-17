#include "World.h"
#include <glad/glad.h>
#include <glm.hpp> 

World::World() {
    generateWorld();
}

World::~World() {}

void World::generateWorld() {
    int worldSize = 16; // 16x16 blocks
    float waterRadius = 3.0f;
    float sandRadius = 4.0f;

    for (int x = -worldSize / 2; x < worldSize / 2; ++x) {
        for (int z = -worldSize / 2; z < worldSize / 2; ++z) {
            // Calculate distance from center (0,0)
            float dist = glm::distance(glm::vec2(x, z), glm::vec2(0.0f, 0.0f));

            // Determine surface block type
            BlockType surfaceType;
            if (dist <= waterRadius) {
                surfaceType = BlockType::Water;
            }
            else if (dist <= sandRadius) {
                surfaceType = BlockType::Sand;
            }
            else {
                surfaceType = BlockType::Grass;
            }

            // Place surface block at y=0
            glm::vec3 surfacePos(static_cast<float>(x), 0.0f, static_cast<float>(z));
            m_Blocks.emplace_back(surfaceType, surfacePos);

            // Place dirt layer at y=-1 (under everything)
            glm::vec3 dirtPos(static_cast<float>(x), -1.0f, static_cast<float>(z));
            m_Blocks.emplace_back(BlockType::Dirt, dirtPos);

            // Place two stone layers at y=-2 and y=-3
            glm::vec3 stonePos1(static_cast<float>(x), -2.0f, static_cast<float>(z));
            m_Blocks.emplace_back(BlockType::Stone, stonePos1);
            glm::vec3 stonePos2(static_cast<float>(x), -3.0f, static_cast<float>(z));
            m_Blocks.emplace_back(BlockType::Stone, stonePos2);
        }
    }
}

void World::draw(const Shader& shader) const {
    for (const auto& block : m_Blocks) {
        block.draw(shader);
    }
}