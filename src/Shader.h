#pragma once
#include <string>
#include <unordered_map>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void use() const;
    void setInt(const std::string& name, int value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const float* mat) const;

private:
    unsigned int m_RendererID;
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;

    unsigned int compileShader(unsigned int type, const std::string& source);
    int getUniformLocation(const std::string& name) const;
    std::string readFile(const std::string& path);
};