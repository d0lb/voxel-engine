#include "World.h"

World::World() {
    generateWorld();
}

World::~World() {}

void World::generateWorld() {
    for (int x = -2; x <= 2; ++x) {
        for (int z = -2; z <= 2; ++z) {
            glm::vec3 pos(x, 0.0f, z);
            m_Blocks.emplace_back(BlockType::Stone, pos);
        }
    }
}

void World::draw(const Shader& shader) const {
    for (const auto& block : m_Blocks) {
        block.draw(shader);
    }
}