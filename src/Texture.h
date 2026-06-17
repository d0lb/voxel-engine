#pragma once
#include <string>

class Texture {
public:
    Texture(const std::string& path);
    ~Texture();

    void bind(unsigned int slot = 0) const;
    void unbind() const;
    unsigned int getID() const { return m_RendererID; }

private:
    unsigned int m_RendererID;
    int m_Width, m_Height, m_Channels;
};