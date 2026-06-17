#pragma once
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include "Shader.h"

enum class BlockType {
    Stone
    // Grass, Dirt, Wood... add later
};

class Block {
public:
    Block(BlockType type, const glm::vec3& position);
    ~Block();

    void draw(const Shader& shader) const;

    // Getters / setters
    glm::vec3 getPosition() const { return m_Position; }
    void setPosition(const glm::vec3& pos) { m_Position = pos; }
    BlockType getType() const { return m_Type; }

    // Durability (placeholder)
    float getDurability() const;

private:
    BlockType m_Type;
    glm::vec3 m_Position;

    unsigned int m_VAO, m_VBO;
    unsigned int m_VertexCount;

    void setupMesh();
};