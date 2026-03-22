#ifndef SHADER_H
#define SHADER_H

#include "glad.h"
#include "stb_image.h"
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    void use();
    void Delete();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
private:
    void checkCompileErrors(GLuint shader, std::string type);
};

class Texture {
public:
    unsigned int ID;
    unsigned char* data;
    Texture(const char* texturePaths[], short int textureNumber, GLint internalformat, bool atlas, int size);
};


class frameBuffer {
public:
    void screenUpdate(int newWidth, int newHeight);
    void build(unsigned int width, unsigned int height);
    ~frameBuffer();
    void drawTo();
    void readFrom();
private:
    unsigned int frameBufferPrivate, colorAttachment, depthAttachment, width, height;
};

class Quad {
public:

Quad(Quad&& other) noexcept;
Quad(const Quad&) = delete;

    static unsigned int VAO, VBO, EBO;
    float X, Y, width, height;
    glm::vec4 color;
    int texture;
    bool useColor = true;
    static std::vector<float> vertices;
    static std::vector<unsigned int> indices;
    bool added = false;
    unsigned int verticesStartIndex;
    unsigned int indicesStartIndex;
    bool safeToDelete = true;
    bool render = true;

    Quad(float x, float y, float width, float height, glm::vec4 color, int texture);
    void updateVertices();
    void updateBuffers();
    void toString();
    ~Quad();
};


#endif