#pragma once
#include <glad/glad.h>

class Cube {
public:
    Cube();
    ~Cube();

    void draw() const;

private:
    unsigned int m_VAO, m_VBO;
    unsigned int m_VertexCount;
    void setupMesh();
};