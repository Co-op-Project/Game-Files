#ifndef SHADER_H
#define SHADER_H

#include "stb_image.h"
#include <fstream>
#include <sstream>

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
    {
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            if (geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if (geometryPath != nullptr)
        {
            const char* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryPath != nullptr)
            glAttachShader(ID, geometry);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != nullptr)
            glDeleteShader(geometry);
    }

    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    void Delete()
    {
        glDeleteProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

class Texture {
public:
    unsigned int ID;
    unsigned char* data;
    Texture(const char* texturePaths[], short int textureNumber, GLint internalformat, bool atlas, int size) {
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
                }
                else {
                    std::cout << "Failed to load texture" << std::endl;
                }
                stbi_image_free(data);
            }

        }
        else {
            int width, height, nrChannels;
            data = stbi_load(texturePaths[0], &width, &height, &nrChannels, 0);

            if (data) {
                glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, internalformat, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else {
                std::cout << "Failed to load texture" << std::endl;
            }
            stbi_image_free(data);
        }

    };

};

class frameBuffer {
public:

    void screenUpdate(int newWidth, int newHeight) {
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

    void build(unsigned int width, unsigned int height) {
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

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        this->width = width;
        this->height = height;
    }

    ~frameBuffer() {
        glDeleteFramebuffers(1, &frameBufferPrivate);
        glDeleteTextures(1, &colorAttachment);
        glDeleteRenderbuffers(1, &depthAttachment);
    }

    void drawTo() {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferPrivate);
        glViewport(0, 0, width > 0 ? width : 1, height > 0 ? height : 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void readFrom() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorAttachment);
    }
private:
    unsigned int frameBufferPrivate, colorAttachment, depthAttachment, width, height;
};

class GUI {    
public:

    GUI(const GUI&) = delete;
    GUI& operator=(const GUI&) = delete;
    GUI(GUI&&) = delete;
    GUI& operator=(GUI&&) = delete;

    float X, Y, width, height, borderWidth = 0.0f;
    glm::vec3 backgroundColor;
    unsigned int VAO, VBO = 0;
    float vertices[12]; //12 + 1 for smooth transition between geometry
    float* vertsForBuffer;
    std::vector<float> totalVertices;
    int type = 0;
    int place = -1;
    int additionalElements = 0;
    bool vertsAdded = false;

    GUI() : VAO(0), VBO(0) {}

    GUI(float x, float y, float width, float height, glm::vec3 backgroundColor, int type){
        mainSetup(x, y, width, height, backgroundColor, type);
    }

    void printAllVerticies(){
        for(int i = 0; i < totalVertices.size(); i++){
            std::cout << "index " << i << ": " << totalVertices[i] << '\n';
        }
    }

    void mainUpdateVertices(){
        float x1 = X - width / 2.0f;
        float y1 = Y - height / 2.0f;
        float x2 = X + width / 2.0f;
        float y2 = Y + height / 2.0f;

        vertices[0] = x1; vertices[1] = y1; // bottom-left
        vertices[2] = x2; vertices[3] = y1; // bottom-right
        vertices[4] = x2; vertices[5] = y2; // top-right

        vertices[6] = x2; vertices[7] = y2; // top-right
        vertices[8] = x1; vertices[9] = y2; // top-left
        vertices[10] = x1; vertices[11] = y1; // bottom-left

        if(!vertsAdded){
            for(int i = 0; i < 12; i++){
                totalVertices.push_back(vertices[i]);
            }
            totalVertices.push_back(vertices[10]);
            totalVertices.push_back(vertices[11]);
            place = totalVertices.size() - 14;
        }else{
            for(int i = place; i < place + 12; i++){
                totalVertices[i] = vertices[0];
            }
            totalVertices[place + 12] = vertices[10];
            totalVertices[place + 13] = vertices[11];
        }
        if(type == 1){
            if(vertsAdded) delete[] vertsForBuffer;
            vertsAdded = true;
            vertsForBuffer = new float[totalVertices.size()];
            for(int i = 0; i < totalVertices.size(); i++) {
                vertsForBuffer[i] = totalVertices[i];
                std::cout << "index " << i << ": " << vertsForBuffer[i] << '\n';
            }
            updateBuffers();
        }
    }

    virtual ~GUI(){
        if(type == 1){
            if (VAO != 0) glDeleteVertexArrays(1, &this->VAO);
            if (VBO != 0) glDeleteBuffers(1, &this->VBO);
            delete[] vertsForBuffer;
        }
    }


    void updateBuffers(){
        glBindVertexArray(this->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, totalVertices.size() * sizeof(float), vertsForBuffer, GL_DYNAMIC_DRAW);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesAmount * sizeof(unsigned int), this->indices, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    }

    void mainSetup(float x, float y, float width, float height, glm::vec3 backgroundColor, int type){
        X = x;
        Y = y;
        this->width = width * 2.0f;
        this->height = height * 2.0f;
        this->backgroundColor = backgroundColor;
        this->type = type;
        if(type == 1){
            glGenVertexArrays(1, &this->VAO);
            glGenBuffers(1, &this->VBO);
        }
        mainUpdateVertices();
    }

private:
    
};

class Button : public GUI {
public:
GUI &parent;

    Button(const Button&) = delete;
    Button& operator=(const Button&) = delete;
    Button(Button&&) = delete;
    Button& operator=(Button&&) = delete;

    Button(int parentIndex, float x, float y, float width, float height, glm::vec3 backgroundColor, int type, GUI *parentGUI) : parent(*parentGUI) {
        this->place = parent.place + 13;
        setup(x, y, width, height, backgroundColor, type);

    }

    ~Button(){}

private:
    void setup(float x, float y, float width, float height, glm::vec3 backgroundColor, int type){
        X = x;
        Y = y;
        this->width = width * 2.0f;
        this->height = height * 2.0f;
        this->backgroundColor = backgroundColor;
        this->type = type;
        if(type == 1){
            this->type = 2;
            std::cout << "BUTTON ATTEMPTED TO BE TYPE 1\n";
        }else{
            this->type = type;
        }
        updateVertices();
        parent.mainUpdateVertices();
    }

    void updateVertices(){
        float x1 = X - width / 2.0f;
        float y1 = Y - height / 2.0f;
        float x2 = X + width / 2.0f;
        float y2 = Y + height / 2.0f;

        vertices[0] = x1; vertices[1] = y1; // bottom-left
        vertices[2] = x2; vertices[3] = y1; // bottom-right
        vertices[4] = x2; vertices[5] = y2; // top-right

        vertices[6] = x2; vertices[7] = y2; // top-right
        vertices[8] = x1; vertices[9] = y2; // top-left
        vertices[10] = x1; vertices[11] = y1; // bottom-left

        if(!vertsAdded){
            vertsAdded = true;
            for(int i = 0; i < 12; i++){
                parent.totalVertices.push_back(vertices[i]);
            }
            parent.totalVertices.push_back(vertices[10]);
            parent.totalVertices.push_back(vertices[11]);
            place = parent.totalVertices.size() - 14;
        }else{
            for(int i = place; i < place + 12; i++){
                parent.totalVertices[i] = vertices[0];
            }
            parent.totalVertices[place + 12] = vertices[10];
            parent.totalVertices[place + 13] = vertices[11];
        }
        parent.updateBuffers();
    }
};

#endif
