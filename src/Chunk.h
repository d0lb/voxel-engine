#pragma once
#include <glm.hpp>
#include <vector>
#include "Block.h"
#include "Shader.h"

class Chunk {
public:
    static const int SIZE = 16;
    static const int HEIGHT = 256;

    Chunk(int chunkX, int chunkZ);
    ~Chunk();

    void buildMesh();
    void draw(const Shader& shader) const;
    void markDirty() { m_Dirty = true; }

    BlockType getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, BlockType type);

    int getChunkX() const { return m_ChunkX; }
    int getChunkZ() const { return m_ChunkZ; }

private:
    int m_ChunkX, m_ChunkZ;
    BlockType m_Blocks[SIZE][HEIGHT][SIZE];
    bool m_Dirty;

    unsigned int m_VAO, m_VBO;
    unsigned int m_VertexCount;

    void addFace(std::vector<float>& vertices,
        float x, float y, float z,   // block center pos
        int face,                    // 0=top,1=bottom,2=front,3=back,4=left,5=right
        BlockType type);

    bool isAir(int x, int y, int z) const;
};