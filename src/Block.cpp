#include "Block.h"
#include <glad/glad.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

Block::Block(BlockType type, const glm::vec3& position)
    : m_Type(type), m_Position(position) {
    setupMesh();
}

Block::~Block() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Block::setupMesh() {
    // Each vertex: position (3), uv (2), face index (1) => 6 floats
    // face indices: 0=top, 1=bottom, 2=front, 3=back, 4=left, 5=right
    float vertices[] = {
        // Front face (z = +0.5) -> нормаль (0,0,1)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 2.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 2.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 2.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 2.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 2.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 2.0f,

        // Back face (z = -0.5) -> нормаль (0,0,-1)
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 3.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 3.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 3.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 3.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 3.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 3.0f,

         // Top face (y = +0.5) -> нормаль (0,1,0)
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,

          // Bottom face (y = -0.5) -> нормаль (0,-1,0)
          -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
          -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
           0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
          -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
           0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
           0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

           // Right face (x = +0.5) -> нормаль (1,0,0)
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 5.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 5.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 5.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 5.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 5.0f,
            0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 5.0f,

            // Left face (x = -0.5) -> нормаль (-1,0,0)
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 4.0f,
            -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 4.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 4.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 4.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 4.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 4.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // UV attribute (location 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Face index attribute (location 2)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_VertexCount = 36;
}

void Block::draw(const Shader& shader) const {
    // Set block type uniform
    shader.setInt("uBlockType", static_cast<int>(m_Type));

    // Model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);
    shader.setMat4("uModel", glm::value_ptr(model));

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
}

float Block::getDurability() const {
    switch (m_Type) {
    case BlockType::Stone: return 1.5f;
    case BlockType::Grass: return 0.6f;
    case BlockType::Dirt:  return 0.5f;
    case BlockType::Sand:  return 0.4f;
    case BlockType::Water: return 0.0f; // not breakable
    default: return 1.0f;
    }
}