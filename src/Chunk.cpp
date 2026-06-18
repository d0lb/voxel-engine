#include "Chunk.h"
#include <glad/glad.h>
#include <gtc/type_ptr.hpp>
#include <array>

Chunk::Chunk(int chunkX, int chunkZ)
    : m_ChunkX(chunkX), m_ChunkZ(chunkZ), m_Dirty(true) {
    // fill with air
    for (int x = 0; x < SIZE; ++x)
        for (int y = 0; y < HEIGHT; ++y)
            for (int z = 0; z < SIZE; ++z)
                m_Blocks[x][y][z] = BlockType::Air;

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    m_VertexCount = 0;
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Chunk::addFace(std::vector<float>& vertices,
    float x, float y, float z,
    int face,
    BlockType type) {
    // pre-built ccw face data for every direction
    // each vertex: pos(3), uv(2), faceIdx(1), blockTypeId(1) -> 7 floats
    struct Vertex {
        float px, py, pz, u, v, faceIdx, blockTypeId;
    };
    static const Vertex topFace[6] = {
        {-0.5f, 0.5f,  0.5f, 0,0,0,0}, { 0.5f, 0.5f,  0.5f, 1,0,0,0}, { 0.5f, 0.5f, -0.5f, 1,1,0,0},
        {-0.5f, 0.5f,  0.5f, 0,0,0,0}, { 0.5f, 0.5f, -0.5f, 1,1,0,0}, {-0.5f, 0.5f, -0.5f, 0,1,0,0}
    };
    static const Vertex bottomFace[6] = {
        {-0.5f,-0.5f,0.5f, 0,1,1,0}, {-0.5f,-0.5f,-0.5f,0,0,1,0}, {0.5f,-0.5f,-0.5f,1,0,1,0},
        {-0.5f,-0.5f,0.5f, 0,1,1,0}, {0.5f,-0.5f,-0.5f,1,0,1,0}, {0.5f,-0.5f,0.5f,1,1,1,0}
    };
    static const Vertex frontFace[6] = {
        {-0.5f,-0.5f,0.5f, 0,0,2,0}, {0.5f,-0.5f,0.5f,1,0,2,0}, {0.5f,0.5f,0.5f,1,1,2,0},
        {-0.5f,-0.5f,0.5f, 0,0,2,0}, {0.5f,0.5f,0.5f,1,1,2,0}, {-0.5f,0.5f,0.5f,0,1,2,0}
    };
    static const Vertex backFace[6] = {
        {0.5f,-0.5f,-0.5f, 0,0,3,0}, {-0.5f,-0.5f,-0.5f,1,0,3,0}, {-0.5f,0.5f,-0.5f,1,1,3,0},
        {0.5f,-0.5f,-0.5f, 0,0,3,0}, {-0.5f,0.5f,-0.5f,1,1,3,0}, {0.5f,0.5f,-0.5f,0,1,3,0}
    };
    static const Vertex leftFace[6] = {
        {-0.5f,-0.5f,-0.5f, 0,0,4,0}, {-0.5f,-0.5f,0.5f,1,0,4,0}, {-0.5f,0.5f,0.5f,1,1,4,0},
        {-0.5f,-0.5f,-0.5f, 0,0,4,0}, {-0.5f,0.5f,0.5f,1,1,4,0}, {-0.5f,0.5f,-0.5f,0,1,4,0}
    };
    static const Vertex rightFace[6] = {
        {0.5f,-0.5f,0.5f, 0,0,5,0}, {0.5f,-0.5f,-0.5f,1,0,5,0}, {0.5f,0.5f,-0.5f,1,1,5,0},
        {0.5f,-0.5f,0.5f, 0,0,5,0}, {0.5f,0.5f,-0.5f,1,1,5,0}, {0.5f,0.5f,0.5f,0,1,5,0}
    };

    const Vertex* src = nullptr;
    switch (face) {
    case 0: src = topFace; break;
    case 1: src = bottomFace; break;
    case 2: src = frontFace; break;
    case 3: src = backFace; break;
    case 4: src = leftFace; break;
    case 5: src = rightFace; break;
    default: return;
    }

    float typeId = static_cast<float>(static_cast<int>(type));
    for (int i = 0; i < 6; ++i) {
        vertices.push_back(src[i].px + x);
        vertices.push_back(src[i].py + y);
        vertices.push_back(src[i].pz + z);
        vertices.push_back(src[i].u);
        vertices.push_back(src[i].v);
        vertices.push_back(src[i].faceIdx);
        vertices.push_back(typeId);
    }
}

void Chunk::buildMesh(const std::function<BlockType(int, int, int)>& getWorldBlock) {
    std::vector<float> vertices;
    vertices.reserve(SIZE * HEIGHT * SIZE * 6 * 6 * 7);

    // predicate: block is opaque (not Air and not Water)
    auto isSolid = [](BlockType t) {
        return t != BlockType::Air && t != BlockType::Water;
        };

    for (int x = 0; x < SIZE; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            for (int z = 0; z < SIZE; ++z) {
                BlockType type = m_Blocks[x][y][z];
                if (type == BlockType::Air) continue;

                float wx = (float)(m_ChunkX * SIZE + x);
                float wy = (float)y;
                float wz = (float)(m_ChunkZ * SIZE + z);

                // determines whether a face should be drawn:
                // draw if neighbor is Air OR (neighbor is Water and current block is solid) OR (neighbor is solid but different type)
                auto shouldDrawFace = [&](int nx, int ny, int nz) {
                    BlockType neighbor;
                    if (nx >= 0 && nx < SIZE && ny >= 0 && ny < HEIGHT && nz >= 0 && nz < SIZE)
                        neighbor = m_Blocks[nx][ny][nz];
                    else {
                        int worldX = m_ChunkX * SIZE + nx;
                        int worldZ = m_ChunkZ * SIZE + nz;
                        neighbor = getWorldBlock(worldX, ny, worldZ);
                    }
                    if (neighbor == BlockType::Air) return true;
                    // both solid: skip face if same type, draw if different
                    if (isSolid(type) && isSolid(neighbor)) {
                        return type != neighbor; // boundary between different solid blocks
                    }
                    // one solid, one water — draw
                    if (isSolid(type) && neighbor == BlockType::Water) return true;
                    if (type == BlockType::Water && isSolid(neighbor)) return true;
                    // water-water: not drawn (water is transparent, internal water faces are invisible)
                    return false;
                    };

                if (shouldDrawFace(x, y + 1, z)) addFace(vertices, wx, wy, wz, 0, type);
                if (shouldDrawFace(x, y - 1, z)) addFace(vertices, wx, wy, wz, 1, type);
                if (shouldDrawFace(x, y, z + 1)) addFace(vertices, wx, wy, wz, 2, type);
                if (shouldDrawFace(x, y, z - 1)) addFace(vertices, wx, wy, wz, 3, type);
                if (shouldDrawFace(x - 1, y, z)) addFace(vertices, wx, wy, wz, 4, type);
                if (shouldDrawFace(x + 1, y, z)) addFace(vertices, wx, wy, wz, 5, type);
            }
        }
    }

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW);

    const size_t stride = 7 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    m_VertexCount = static_cast<unsigned int>(vertices.size() / 7);
    m_Dirty = false;
}

void Chunk::draw(const Shader& shader) const {
    if (m_VertexCount == 0) return;
    shader.setMat4("uModel", glm::value_ptr(glm::mat4(1.0f)));
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
}

BlockType Chunk::getBlock(int x, int y, int z) const {
    if (x < 0 || x >= SIZE || y < 0 || y >= HEIGHT || z < 0 || z >= SIZE)
        return BlockType::Air;
    return m_Blocks[x][y][z];
}

void Chunk::setBlock(int x, int y, int z, BlockType type) {
    if (x < 0 || x >= SIZE || y < 0 || y >= HEIGHT || z < 0 || z >= SIZE) return;
    m_Blocks[x][y][z] = type;
    m_Dirty = true;
}