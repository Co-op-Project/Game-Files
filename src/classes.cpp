#include "classes.h"
#include "glad.h"
#include <iostream>

//Static definitions for Quad
unsigned int Quad::VAO = 0;
unsigned int Quad::VBO = 0;
std::vector<float> Quad::vertices;

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
Texture::Texture(const char* texturePath, short int textureNumber, GLint internalformat, bool atlas, int size) {
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &ID);
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    int width, height, nrChannels;
    data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, internalformat, GL_UNSIGNED_BYTE, data);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
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
      texture(other.texture), added(other.added), verticesStartIndex(other.verticesStartIndex),
      safeToDelete(other.safeToDelete), render(other.render), sizeRatio(other.sizeRatio),
      texXmin(other.texXmin), texYmin(other.texYmin)
{
    other.safeToDelete = false; // Disarm the source so it won't delete GL objects
}

//width and height in pixels of one whole texture unit is sizeRatio * 32
Quad::Quad(float x, float y, float width, float height, int texture, bool render, float sizeRatio) {
    updateData(x, y, width, height, texture, render, sizeRatio);
}

void Quad::updateData(float x, float y, float width, float height, int texture, bool render, float sizeRatio){
    X = x - sizeRatio * 8;
    Y = y - sizeRatio * 8;
    this->width = width;
    this->height = height;
    this->texture = texture;
    this->render = render;
    this->sizeRatio = sizeRatio;
    updateVertices();
}

void Quad::updateVertices() {
    float unit = 32.0f/1024.0f;

    float x1 = X;
    float x2 = X + width * 0.5f;
    float y1 = Y;
    float y2 = Y + height * 0.5f;

    float texX1 = ((float(texture % 32) * 32.0f) / 1024.0f);
    float texX2 = ((float((texture + 1) % 32) * 32.0f) / 1024.0f) + (unit * (width * unit * (1.0f/sizeRatio)) - unit);
    float texY1 = ((float(texture / 32) * 32.0f) / 1024.0f);
    float texY2 = ((float((texture + 1 * 32) / 32) * 32.0f) / 1024.0f) + (unit * (height * unit * (1.0f/sizeRatio)) - unit);

    texXmin = texX1; texYmin = texY1;
    if (added) {
        vertices[verticesStartIndex + 0] = x1;  vertices[verticesStartIndex + 1] = y1;  vertices[verticesStartIndex + 2] = texX1;   vertices[verticesStartIndex + 3] = texY1;   //x1, y1
        vertices[verticesStartIndex + 4] = x2;  vertices[verticesStartIndex + 5] = y1;  vertices[verticesStartIndex + 6] = texX2;   vertices[verticesStartIndex + 7] = texY1;   //x2, y1
        vertices[verticesStartIndex + 8] = x2;  vertices[verticesStartIndex + 9] = y2;  vertices[verticesStartIndex + 10] = texX2;  vertices[verticesStartIndex + 11] = texY2;  //x2, y2
 
        vertices[verticesStartIndex + 12] = x2; vertices[verticesStartIndex + 13] = y2;  vertices[verticesStartIndex + 14] = texX2;  vertices[verticesStartIndex + 15] = texY2; //x2, y2
        vertices[verticesStartIndex + 16] = x1; vertices[verticesStartIndex + 17] = y2;  vertices[verticesStartIndex + 18] = texX1;  vertices[verticesStartIndex + 19] = texY2; //x1, y2
        vertices[verticesStartIndex + 20] = x1; vertices[verticesStartIndex + 21] = y1;  vertices[verticesStartIndex + 22] = texX1;  vertices[verticesStartIndex + 23] = texY1; //x1, y1

    } else {
        verticesStartIndex = vertices.size();
        vertices.push_back(x1);  vertices.push_back(y1);  vertices.push_back(texX1);  vertices.push_back(texY1);//x1, y1
        vertices.push_back(x2);  vertices.push_back(y1);  vertices.push_back(texX2);  vertices.push_back(texY1);//x2, y1
        vertices.push_back(x2);  vertices.push_back(y2);  vertices.push_back(texX2);  vertices.push_back(texY2);//x2, y2

        vertices.push_back(x2);  vertices.push_back(y2);  vertices.push_back(texX2);  vertices.push_back(texY2);//x2, y2
        vertices.push_back(x1);  vertices.push_back(y2);  vertices.push_back(texX1);  vertices.push_back(texY2);//x1, y2     
        vertices.push_back(x1);  vertices.push_back(y1);  vertices.push_back(texX1);  vertices.push_back(texY1);//x1, y1   
    }
    added = true;
    updateBuffers();
}
void Quad::updateBuffers() {
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Quad::toString(){
    std::cout << '\n' << "verticesStartIndex: " << verticesStartIndex << '\n';
    std::cout << "all vertices amount: " << vertices.size() << '\n';
    for(int i = verticesStartIndex; i < verticesStartIndex + 24;) std::cout << "x: " << vertices[i++] << ", y: " << vertices[i++] << ", texX: " << vertices[i++] << ", texY: " << vertices[i++] << '\n';
    std::cout << "texture: " << texture << '\n';
}

Quad::~Quad(){}

Quad::Quad(){}

void Quad::draw(Shader &s){
    s.setVec2("texMins", glm::vec2(texXmin, texYmin));
    if(render) glDrawArrays(GL_TRIANGLES, verticesStartIndex / 4, 6);
}

myGUI::myGUI(float x, float y, float width, float height, bool render, float sizeRatio) {
    X = x - sizeRatio * 8;
    Y = y - sizeRatio * 8;
    this->width = width;
    this->height = height;
    this->render = render;
    this->sizeRatio = sizeRatio; 
    updateGUI();
} 

void myGUI::updateGUI(){
    background.updateData(X, Y, width, height, 996, render, sizeRatio);
    top.updateData(X, Y + height * 0.5f - sizeRatio * 16, width, sizeRatio * 32, 994, render, sizeRatio);
    bottom.updateData(X, Y, width, sizeRatio * 32 - 1, 992, render, sizeRatio);
    left.updateData(X, Y, sizeRatio * 32, height, 993, render, sizeRatio);
    right.updateData(X + width * 0.5f - sizeRatio * 16, Y, sizeRatio * 32, height, 995, render, sizeRatio);
}

void myGUI::draw(Shader &s){
    background.draw(s);
    top.draw(s);
    bottom.draw(s);
    left.draw(s);
    right.draw(s);
}

void myGUI::toString(){
    top.toString();
    bottom.toString();
    left.toString();
    right.toString();
    background.toString();
}