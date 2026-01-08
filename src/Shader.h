#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

/**
 * Shader class - loads, compiles, and manages OpenGL shaders
 */
class Shader
{
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    
    void use() const;
    
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& vec) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
    
    unsigned int getID() const { return m_programID; }
    
private:
    unsigned int m_programID;
    
    std::string readFile(const std::string& path);
    void checkCompileErrors(unsigned int shader, const std::string& type);
};