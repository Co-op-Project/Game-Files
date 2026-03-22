#include "classes.h"
#include "glad.h"
#include <iostream>

//Static definitions for Quad
unsigned int Quad::VAO = 0;
unsigned int Quad::VBO = 0;
unsigned int Quad::EBO = 0;
std::vector<float> Quad::vertices;
std::vector<unsigned int> Quad::indices;

//Shader implementation
Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        if (geometryPath != nullptr) {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    } catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    unsigned int geometry;
    if (geometryPath != nullptr) {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryPath != nullptr)
        glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr)
        glDeleteShader(geometry);
}
void Shader::use(){ 
    glUseProgram(ID); 
}
void Shader::Delete(){ 
    glDeleteProgram(ID); 
}
void Shader::setBool(const std::string& name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); }
void Shader::setInt(const std::string& name, int value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
void Shader::setFloat(const std::string& name, float value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), value); }
void Shader::setVec2(const std::string& name, const glm::vec2& value) const { glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
void Shader::setVec2(const std::string& name, float x, float y) const { glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); }
void Shader::setVec3(const std::string& name, const glm::vec3& value) const { glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
void Shader::setVec3(const std::string& name, float x, float y, float z) const { glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); }
void Shader::setVec4(const std::string& name, const glm::vec4& value) const { glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) { glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); }
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const { glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const { glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const { glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }

void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

// Texture implimentation
Texture::Texture(const char* texturePaths[], short int textureNumber, GLint internalformat, bool atlas, int size) {
    glGenTextures(1, &ID);
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    if (atlas) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, size - 1);
        for (int i = 0; i < size; i++) {
            int width, height, nrChannels;
            data = stbi_load(texturePaths[i], &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_2D, i, internalformat, width, height, 0, internalformat, GL_UNSIGNED_BYTE, data);
            } else {
                std::cout << "Failed to load texture" << std::endl;
            }
            stbi_image_free(data);
        }
    } else {
        int width, height, nrChannels;
        data = stbi_load(texturePaths[0], &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, internalformat, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
}

//framebuffer implimentation
void frameBuffer::screenUpdate(int newWidth, int newHeight) {
    if (newWidth <= 0 || newHeight <= 0) return;
    if ((unsigned)newWidth == width && (unsigned)newHeight == height) return;

    width = static_cast<unsigned int>(newWidth);
    height = static_cast<unsigned int>(newHeight);

    // Update color texture storage
    glBindTexture(GL_TEXTURE_2D, colorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Update renderbuffer storage
    glBindRenderbuffer(GL_RENDERBUFFER, depthAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    // Keep texture bound as caller needs it; no framebuffer rebind required here
}

void frameBuffer::build(unsigned int width, unsigned int height) {
    glGenFramebuffers(1, &frameBufferPrivate);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferPrivate);

    glGenTextures(1, &colorAttachment);
    glBindTexture(GL_TEXTURE_2D, colorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

    glGenRenderbuffers(1, &depthAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, depthAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAttachment);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    this->width = width;
    this->height = height;
}

frameBuffer::~frameBuffer() {
    glDeleteFramebuffers(1, &frameBufferPrivate);
    glDeleteTextures(1, &colorAttachment);
    glDeleteRenderbuffers(1, &depthAttachment);
}

void frameBuffer::drawTo() {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferPrivate);
    glViewport(0, 0, width > 0 ? width : 1, height > 0 ? height : 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void frameBuffer::readFrom() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorAttachment);
}

// Quad implimentation
Quad::Quad(Quad&& other) noexcept
    : X(other.X), Y(other.Y), width(other.width), height(other.height),
      color(other.color), texture(other.texture), useColor(other.useColor),
      added(other.added), verticesStartIndex(other.verticesStartIndex),
      indicesStartIndex(other.indicesStartIndex), safeToDelete(other.safeToDelete)
{
    other.safeToDelete = false; // Disarm the source so it won't delete GL objects
}

Quad::Quad(float x, float y, float width, float height, glm::vec4 color, int texture) {
    X = x;
    Y = y;
    this->width = width;
    this->height = height;
    this->color = color;
    this->texture = texture;
    if (color.x == -1.0f) useColor = false;
    updateVertices();
}

void Quad::updateVertices() {
    float x1 = X - width * 0.5f;
    float x2 = X + width * 0.5f;
    float y1 = Y - height * 0.5f;
    float y2 = Y + height * 0.5f;

    if (added) {
        vertices[verticesStartIndex] = x1;     vertices[verticesStartIndex + 1] = y1; //x1, y1
        vertices[verticesStartIndex + 2] = x2; vertices[verticesStartIndex + 3] = y1; //x2, y1
        vertices[verticesStartIndex + 4] = x2; vertices[verticesStartIndex + 5] = y2; //x2, y2
        vertices[verticesStartIndex + 6] = x1;     vertices[verticesStartIndex + 7] = y2; //x1, y2
    } else {
        verticesStartIndex = vertices.size();
        vertices.push_back(x1);  vertices.push_back(y1); //x1, y1
        vertices.push_back(x2);  vertices.push_back(y1); //x2, y1
        vertices.push_back(x2);  vertices.push_back(y2); //x2, y2
        vertices.push_back(x1);  vertices.push_back(y2); //x1, y2

        indicesStartIndex = indices.size();
        unsigned int vertexBase = verticesStartIndex / 2; // convert float index → vertex index
        indices.push_back(vertexBase);
        indices.push_back(vertexBase + 1);
        indices.push_back(vertexBase + 2);

        indices.push_back(vertexBase + 2);
        indices.push_back(vertexBase + 3);
        indices.push_back(vertexBase);


         //same index for line blend when changing quad
        
    }
    added = true;
    updateBuffers();
}
void Quad::updateBuffers() {
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Quad::toString(){
    std::cout << '\n' << "verticesStartIndex: " << verticesStartIndex << '\n';
    std::cout << "vertices: " << vertices.size() << " indices: " << indices.size() << '\n';
    for(int i = 0; i < vertices.size();) std::cout << "x: " << vertices[i++] << ", y: " << vertices[i++] << '\n';
    std::cout << "INDICES: \n";
    for(int i = 0; i < indices.size(); i++) std::cout << indices[i] << '\n';
    std::cout << "texture: " << texture << '\n';
    std::cout << "r: " << color.x << ", g: " << color.y << ", b: " << color.z << ", a: " << color.w << '\n';
}

Quad::~Quad(){
    if(safeToDelete){
        glDeleteVertexArrays(1, &this->VAO);
        glDeleteBuffers(1, &this->VBO);
        glDeleteBuffers(1, &this->EBO);
        safeToDelete = false;
    } 
}
