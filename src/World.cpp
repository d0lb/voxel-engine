#include "World.h"
#include "Camera.h"

extern Camera* g_Camera;

World::World(unsigned int seed) : m_Generator(seed) { }

World::~World() {}

void World::update(const glm::vec3& cameraPos) {
    
    int camChunkX = static_cast<int>(floor(cameraPos.x / Chunk::SIZE));
    int camChunkZ = static_cast<int>(floor(cameraPos.z / Chunk::SIZE));

   
    for (int dx = -m_ViewDistance; dx <= m_ViewDistance; ++dx) {
        for (int dz = -m_ViewDistance; dz <= m_ViewDistance; ++dz) {
            int cx = camChunkX + dx;
            int cz = camChunkZ + dz;
            ChunkCoord key{ cx, cz };
            if (m_Chunks.find(key) == m_Chunks.end()) {
                loadChunk(cx, cz);
            }
        }
    }

    
    auto it = m_Chunks.begin();
    while (it != m_Chunks.end()) {
        int cx = it->first.x;
        int cz = it->first.z;
        if (abs(cx - camChunkX) > m_ViewDistance + 1 || abs(cz - camChunkZ) > m_ViewDistance + 1) {
            it = m_Chunks.erase(it);
        }
        else {
            ++it;
        }
    }
}

void World::draw(const Shader& shader) const {
    for (const auto& pair : m_Chunks) {
        Chunk& chunk = *pair.second;
        int wx = chunk.getChunkX() * Chunk::SIZE;
        int wz = chunk.getChunkZ() * Chunk::SIZE;
        glm::vec3 min(wx, 0, wz);
        glm::vec3 max(wx + Chunk::SIZE, Chunk::HEIGHT, wz + Chunk::SIZE);
        if (g_Camera && !g_Camera->isAABBVisible(min, max))
            continue;
        chunk.draw(shader);
    }
}

Chunk* World::getChunk(int chunkX, int chunkZ) {
    ChunkCoord key{ chunkX, chunkZ };
    auto it = m_Chunks.find(key);
    if (it != m_Chunks.end()) {
        return it->second.get();
    }
    return nullptr;
}

void World::loadChunk(int chunkX, int chunkZ) {
    auto chunk = std::make_unique<Chunk>(chunkX, chunkZ);
    m_Generator.generate(*chunk);
    chunk->buildMesh();
    m_Chunks[{chunkX, chunkZ}] = std::move(chunk);
}

void World::unloadChunk(int chunkX, int chunkZ) {
    m_Chunks.erase({ chunkX, chunkZ });
}