#pragma once
#include <vector>
#include "Block.h"
#include "Shader.h"

class World {
public:
    World();
    ~World();

    void draw(const Shader& shader) const;

private:
    std::vector<Block> m_Blocks;
    void generateWorld();
};