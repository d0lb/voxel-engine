#include "Chunk.h"
#include <glad/glad.h>
#include <gtc/type_ptr.hpp>
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

bool Chunk::isAir(int x, int y, int z) const {
    if (x < 0 || x >= SIZE || y < 0 || y >= HEIGHT || z < 0 || z >= SIZE)
        return true; // outside chunk -> treat as air
    return m_Blocks[x][y][z] == BlockType::Air;
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

void Chunk::buildMesh() {
    std::vector<float> vertices;
    vertices.reserve(SIZE * HEIGHT * SIZE * 6 * 6 * 7); // rough estimate

    for (int x = 0; x < SIZE; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            for (int z = 0; z < SIZE; ++z) {
                BlockType type = m_Blocks[x][y][z];
                if (type == BlockType::Air) continue;

                float wx = (float)(m_ChunkX * SIZE + x);
                float wy = (float)y;
                float wz = (float)(m_ChunkZ * SIZE + z);

                // only add faces that touch air or chunk border
                if (isAir(x, y + 1, z)) addFace(vertices, wx, wy, wz, 0, type);
                if (isAir(x, y - 1, z)) addFace(vertices, wx, wy, wz, 1, type);
                if (isAir(x, y, z + 1)) addFace(vertices, wx, wy, wz, 2, type);
                if (isAir(x, y, z - 1)) addFace(vertices, wx, wy, wz, 3, type);
                if (isAir(x - 1, y, z)) addFace(vertices, wx, wy, wz, 4, type);
                if (isAir(x + 1, y, z)) addFace(vertices, wx, wy, wz, 5, type);
            }
        }
    }

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW);

    // vertex layout: pos(3), uv(2), faceIdx(1), blockType(1) -> 7 floats
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