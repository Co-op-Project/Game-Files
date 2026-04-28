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
      render(other.render), sizeRatio(other.sizeRatio), type(other.type), angle(other.angle),
      texXmin(other.texXmin), texYmin(other.texYmin), model(other.model), preX(other.preX), preY(other.preY)
{}

Quad::Quad(float x, float y, float width, float height, int texture, bool render, float sizeRatio,int type) {
    updateData(x, y, width, height, texture, render, sizeRatio, type);
}

void Quad::updateData(float x, float y, float width, float height, int texture, bool render, float sizeRatio, int type){
    bool needsUpdate = false;
    X = x * window_height;
    Y = y * window_height;
    this->render = render;
    this->type = type;
    if(width * window_height != this->width || height * window_height != this->height || this->texture != texture || this->sizeRatio != sizeRatio) needsUpdate = true;
    if(needsUpdate){
        this->width = width * window_height;
        this->height = height * window_height;
        this->texture = texture;
        this->sizeRatio = sizeRatio;
        updateVertices();
    }
}

void Quad::updateVertices() {
    float unit = 32.0f/1024.0f;

    float x1 = 0.0f - width * 0.5f;
    float x2 = width * 0.5f;
    float y1 = 0.0f - height * 0.5f;
    float y2 = height * 0.5f;

    float texX1 = ((float(texture % 32) * 32.0f) / 1024.0f);
    float texX2 = ((float((texture + 1) % 32) * 32.0f) / 1024.0f) + (unit * (width * unit * (1.0f/(sizeRatio))) - unit);
    float texY1 = ((float(texture / 32) * 32.0f) / 1024.0f);
    float texY2 = ((float((texture + 1 * 32) / 32) * 32.0f) / 1024.0f) + (unit * (height * unit * (1.0f/(sizeRatio))) - unit);

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
    if(render){
        if(preX != X || preY != Y || preAngle != angle || preWorldX != worldX || preWorldY != worldY){
            preWorldX = worldX;
            preWorldY = worldY;
            preX = X;
            preY = Y;
            preAngle = angle;
            model = glm::mat4(1.0f);
            switch (type){
                case STANDARD :{
                    model = glm::translate(model, glm::vec3(X - preWorldX, Y - preWorldY, 0.0f));
                } break;
                case ENTITY :{
                    model = glm::translate(model, glm::vec3(X - preWorldX, Y - preWorldY, 0.0f));
                    model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
                } break;
                default:
                break;
            }
        }
        s.use();
        s.setMat4("model", this->model);
        s.setVec2("texMins", glm::vec2(texXmin, texYmin));
        glDrawArrays(GL_TRIANGLES, verticesStartIndex / 4, 6);
    }
}

float Quad::relativeX(float x){ return X + width * (-0.5f + x) * (window_height / window_width); }

float Quad::relativeY(float y){ return Y + height * (-0.5f + y); }

float Quad::relativeWH(float sizeRatio) { return (sizeRatio / 64.0f) * (startingWindow_height / window_height); }

bool Quad::mouseOver(float x, float y, float width, float height){
    return (lastX > x - width * 0.5f) && (lastX < x + width * 0.5f) && (window_height - lastY > y - height * 0.5f) && (window_height - lastY < y + height * 0.5f);;
    return false;
}

myGUI::myGUI(float x, float y, float width, float height, bool render, float sizeRatio) {
    X = x;
    Y = y;
    this->width = width;
    this->height = height;
    this->render = render;
    this->sizeRatio = sizeRatio;
    updateGUI();
} 

void myGUI::updateGUI(){
    bool tempRender = width != 0.0f && height != 0.0f && render;
    float tempSizeRatio = sizeRatio * (window_height / startingWindow_height);
    background.updateData(X, Y, width, height, 996, tempRender, tempSizeRatio, STANDARD); 
    top.updateData(X, relativeY(1.0f), width, relativeWH(tempSizeRatio), 994, tempRender, tempSizeRatio, STANDARD); 
    bottom.updateData(X, relativeY(0.0f), width, relativeWH(tempSizeRatio), 992, tempRender, tempSizeRatio, STANDARD);
    left.updateData(relativeX(0.0f), Y, relativeWH(tempSizeRatio), height, 993, tempRender, tempSizeRatio, STANDARD); 
    right.updateData(relativeX(1.0f), Y, relativeWH(tempSizeRatio), height, 995, tempRender, tempSizeRatio, STANDARD);

    for(int i = 0; i < buttons.size(); i++){
        buttons[i].sizeRatio = sizeRatio;
        buttons[i].background.updateData(relativeX(buttons[i].X), relativeY(buttons[i].Y), width * buttons[i].width, relativeWH(tempSizeRatio), buttons[i].texture, tempRender, tempSizeRatio * 0.375f, STANDARD);
        buttons[i].left.updateData(relativeX(buttons[i].X - buttons[i].width * 0.5f), relativeY(buttons[i].Y), relativeWH(tempSizeRatio), relativeWH(tempSizeRatio), 998, tempRender, tempSizeRatio * 0.375f, STANDARD);
        buttons[i].right.updateData(relativeX(buttons[i].X + buttons[i].width * 0.5f), relativeY(buttons[i].Y), relativeWH(tempSizeRatio), relativeWH(tempSizeRatio), 999, tempRender, tempSizeRatio * 0.375f, STANDARD);
    }

}

void myGUI::draw(Shader &s){
    glBindVertexArray(Quad::VAO);
    background.draw(s);
    top.draw(s);
    bottom.draw(s);
    left.draw(s);
    right.draw(s);
    for(int i = 0; i < buttons.size(); i++){
        buttons[i].background.draw(s);
        buttons[i].left.draw(s);
        buttons[i].right.draw(s);
    }
    for(int i = 0; i < buttons.size(); i++){
        float textSizeRatio = sizeRatio * (window_height / startingWindow_height) * 0.125f;
        if(buttons[i].over() && !held(MOUSE_LEFT)) buttons[i].status = 1;
        if(buttons[i].status == 1){
            buttons[i].counter += deltaTime;
            if(buttons[i].counter > 0.01f){
                buttons[i].counter = 0.0f;
                if(buttons[i].over() && !held(MOUSE_LEFT)){
                    buttons[i].status = 1;
                }else{
                    buttons[i].status = 0;
                }
            }
        }
        if(buttons[i].status == 1 && held(MOUSE_LEFT) && buttons[i].over()){
            switch(buttons[i].callbackID){
                case(NOARGS): buttons[i].callBack(); break;
                case(INT): buttons[i].callBackInt(1); break;
                case(GUI): buttons[i].callBackGUI(this); break;

            default: break;
            }
            buttons[i].status = 2;
        }

        if(render && buttons[i].text != "") RenderText(*textShader, buttons[i].text, relativeX(buttons[i].X - buttons[i].width * 0.5f) + relativeWH(textSizeRatio * 1.25f) * (startingWindow_width / window_width) * (window_height / startingWindow_height), relativeY(buttons[i].Y) - relativeWH(textSizeRatio * 1.5f), textSizeRatio * 1.2f * (startingWindow_height / window_height), buttons[i].color);
    }
}
    

void myGUI::toString(){
    top.toString();
    bottom.toString();
    left.toString();
    right.toString();
    background.toString();
}

void myGUI::addButton(float x, float y, float width, short int texture) { 
    buttons.emplace_back(buttonContainer(x, y, width));
    buttons[buttons.size() - 1].texture = texture;
}

void myGUI::addButton(float x, float y, float width, std::string text, glm::vec3 color, short int texture) { 
    buttons.emplace_back(buttonContainer(x, y, width));
    buttons[buttons.size() - 1].text = text;
    buttons[buttons.size() - 1].color = color;
    buttons[buttons.size() - 1].texture = texture;
}

buttonContainer::buttonContainer(float x, float y, float width){
    X=x;
    Y=y;
    this->width = width;
}

bool buttonContainer::over(){
    return mouseOver(background.X, background.Y, background.width, window_height * relativeWH(sizeRatio * (window_height / startingWindow_height)));
}

void buttonContainer::setCallback(std::function<void()> func){
    this->callBack = func;
    callbackID = NOARGS;
}

void buttonContainer::setCallback(std::function<void(int x)> func){
    this->callBackInt = func;
    callbackID = INT;
}

void buttonContainer::setCallback(std::function<void(myGUI* gui)> func){
    this->callBackGUI = func;
    callbackID = GUI;
}

Entity::Entity(float x, float y, short int texture, float scale, bool render){
    Xp=x;
    Yp=y;
    this->render = render;
    this->texture = texture;
    this->scale = scale;
    type = ENTITY;
}

void Entity::update(){
    float tempSizeRatio = 4.0f * scale * (window_height / startingWindow_height);
    updateData(Xp, Yp, relativeWH(tempSizeRatio), relativeWH(tempSizeRatio), texture, render, tempSizeRatio / 2.67f, ENTITY);
}

void Entity::move(){
    Xp += V.x * deltaTime;
    Yp += V.y * deltaTime;
    A = glm::vec2(0.0f);
    for(glm::vec2 &a : As) A +=a;

    if(A.x != 0.0f){
        if(A.x < 0.0f){
            A.x += friction.x * mass * deltaTime;
            if(A.x > 0.0f) A.x = 0.0f;
        }else{
            A.x -= friction.x * mass * deltaTime;
            if(A.x < 0.0f) A.x = 0.0f;
        }
    }
    if(A.y != 0.0f){
        if(A.y < 0.0f){
            A.y += friction.y * mass * deltaTime;
            if(A.y > 0.0f) A.y = 0.0f;
        }else{
            A.y -= friction.y * mass * deltaTime;
            if(A.y < 0.0f) A.y = 0.0f;
        }
    }

    V += A * deltaTime;
    if(V.x != 0.0f) V.x *= 1.0f - (friction.x * mass / abs(V.x)) * deltaTime;
    if(abs(V.x) < 0.01f) V.x = 0.0f;
    if(V.y != 0.0f) V.y *= 1.0f - (friction.y * mass / abs(V.y)) * deltaTime;
    if(abs(V.y) < 0.01f) V.y = 0.0f;
}

float Entity::pointTowards(float x, float y){
    float targetX, targetY;
    float left = window_width * (zoom - 2.0f) * -0.5f;
    float right = window_width * zoom * 0.5f;
    float bottom = window_height * (zoom - 2.0f) * -0.5f;
    float top = window_height * zoom * 0.5f;
    targetX = left + (right - left) * (x / window_width) + worldX;
    targetY = bottom + (top - bottom) * (y / window_height) + worldY;
    
    float deltaX = targetX - X;
    float deltaY = targetY - Y;
    angle = glm::atan(deltaY, deltaX);
    return angle;
}

glm::vec2 Entity::dirScalar(float mag, float dir){
    return glm::vec2(mag * glm::cos(dir), mag * glm::sin(dir));
}

Projectile::Projectile(){}

Projectile::Projectile(float x, float y, short int texture, float scale, bool render, bool AABB){
    Xp=x;
    Yp=y;
    this->render = render;
    this->texture = texture;
    this->scale = scale;
    hitbox.hitboxWidth = 1.0f;
    hitbox.hitboxHeight = 1.0f;
    hitbox.AABB = AABB;
    type = ENTITY;
    hitbox.type = STANDARD;
    this->update();
    hitbox.updateSAT();
}

void Projectile::update(){
    this->Entity::update();
    hitbox.AABB ? hitbox.angle = 0 : hitbox.angle = angle;
    hitbox.updateData(Xp, Yp, (width / window_height) * hitbox.hitboxWidth, (height / window_height) * hitbox.hitboxHeight, 1001, 1, hitbox.AABB ? STANDARD : ENTITY);
}

void Projectile::draw(Shader &s){
    this->Quad::draw(s);
    if(debug.showHitboxes) hitbox.draw(s);
}

Hitbox::Hitbox(){}

void Hitbox::updateSAT(){
    float cosA = cos(angle);
    float sinA = sin(angle);
    glm::mat2 rot2D = glm::mat2(cosA, sinA, -sinA, cosA);
    glm::vec2 center(X, Y);
    glm::vec2 corners[4] = {
        {-this->width * 0.5f, -this->height * 0.5f},
        { this->width * 0.5f, -this->height * 0.5f},
        { this->width * 0.5f,  this->height * 0.5f},
        {-this->width * 0.5f,  this->height * 0.5f}
    };
    for(int i = 0; i < 4; i++){
        SATvertices[i] = center + rot2D * corners[i];
    }
    glm::vec2 edges[4];
    for(int i = 0; i < 4; i++){
        glm::vec2 e = SATvertices[(i+1) % 4] - SATvertices[i];
        float mag = glm::length(e);
        normals[i] = glm::vec2(-e.y / mag, e.x / mag);
    }
}

void Hitbox::updateData(float x, float y, float width, float height, int texture, float sizeRatio, int type){
    Quad::updateData(x, y, width, height, texture, true, sizeRatio, type);
}

struct Projection {
    float min, max;
};

Projection projectShape(glm::vec2 vertices[], glm::vec2 axis) {
    float min = glm::dot(axis, vertices[0]);
    float max = min;

    for (int i = 0; i < 4; i++) {
        float p = glm::dot(axis, vertices[i]);
        if (p < min) min = p;
        else if (p > max) max = p;
    }
    return {min, max};
}

bool Projectile::colliding(Projectile &object1, Projectile &object2){
    if (object1.hitbox.AABB && object2.hitbox.AABB) {
        float overlapX = std::min(object1.hitbox.X + object1.hitbox.width  * 0.5f, object2.hitbox.X + object2.hitbox.width  * 0.5f)
                       - std::max(object1.hitbox.X - object1.hitbox.width  * 0.5f, object2.hitbox.X - object2.hitbox.width  * 0.5f);
        float overlapY = std::min(object1.hitbox.Y + object1.hitbox.height * 0.5f, object2.hitbox.Y + object2.hitbox.height * 0.5f)
                       - std::max(object1.hitbox.Y - object1.hitbox.height * 0.5f, object2.hitbox.Y - object2.hitbox.height * 0.5f);

        if(overlapX <= 0.0f || overlapY <= 0.0f) return false;

        if(overlapX < overlapY){
            float speed1 = std::abs(object1.V.x);
            float speed2 = std::abs(object2.V.x);
            float totalSpeed = speed1 + speed2;
            float share1 = (totalSpeed > 0.0f) ? speed1 / totalSpeed : 0.1f;
            float share2 = (totalSpeed > 0.0f) ? speed2 / totalSpeed : 0.1f;
            float overlapXn = overlapX / window_width;
            if (overlapXn < 0.001f) overlapXn = 0.001f;
            if(object1.Xp < object2.Xp){
                object1.immobile ? object2.Xp += overlapXn : object1.Xp -= share1 * overlapXn;
                object2.immobile ? object1.Xp -= overlapXn : object2.Xp += share2 * overlapXn;
            }else{ 
                object1.immobile ? object2.Xp -= overlapXn : object1.Xp += share1 * overlapXn;
                object2.immobile ? object1.Xp += overlapXn : object2.Xp -= share2 * overlapXn;
            }
        }else if(overlapY < overlapX){
            float speed1 = std::abs(object1.V.y);
            float speed2 = std::abs(object2.V.y);
            float totalSpeed = speed1 + speed2;
            float share1 = (totalSpeed > 0.0f) ? speed1 / totalSpeed : 0.5f;
            float share2 = (totalSpeed > 0.0f) ? speed2 / totalSpeed : 0.5f;
            float overlapYn = overlapY / window_height;
            if (overlapYn < 0.001f) overlapYn = 0.001f;
            if(object1.Yp < object2.Yp){
                object1.immobile ? object2.Yp += overlapYn: object1.Yp -= share1 * overlapYn;
                object2.immobile ? object1.Yp -= overlapYn: object2.Yp += share2 * overlapYn;
            }else{
                object1.immobile ? object2.Yp -= overlapYn: object1.Yp += share1 * overlapYn;
                object2.immobile ? object1.Yp += overlapYn: object2.Yp -= share2 * overlapYn;
            }
        }else{
            int iterations = 1;
            while(iterations < 100){
                if(object1.hitbox.X + object1.hitbox.width * 0.5f - (object1.hitbox.width * iterations) < object2.hitbox.X - object2.hitbox.width * 0.5f){
                    float displaceX = overlapX / window_width * iterations;
                    if (displaceX < 0.001f) displaceX = 0.001f;
                    object1.Xp -= displaceX;
                    overlapX = 1.f;
                    break;
                }
                if(object1.hitbox.X - object1.hitbox.width * 0.5f - (object1.hitbox.width * iterations) > object2.hitbox.X + object2.hitbox.width * 0.5f){
                    float displaceX = overlapX / window_width  * iterations;
                    if (displaceX < 0.001f) displaceX = 0.001f;
                    object1.Xp += displaceX;
                    overlapX = 1.f;
                    break;
                }
                if(object1.hitbox.Y + object1.hitbox.height * 0.5f - (object1.hitbox.height * iterations) < object2.hitbox.Y - object2.hitbox.height * 0.5f){
                    float displaceY = overlapY / window_height * iterations;
                    if (displaceY < 0.001f) displaceY = 0.001f;
                    object1.Yp -= displaceY;
                    overlapY = 1.f;
                    break;
                }
                if(object1.hitbox.Y - object1.hitbox.height * 0.5f - (object1.hitbox.height * iterations) > object2.hitbox.Y + object2.hitbox.height * 0.5f){
                    float displaceY = overlapY / window_height * iterations;
                    if (displaceY < 0.001f) displaceY = 0.001f;
                    object1.Yp += displaceY;
                    overlapY = 1.f;
                    break;
                }
                iterations++;
            }
        }
        
        float totalElasticity = (object1.elasticity + object2.elasticity) * 0.5f;
        if(overlapX < overlapY){
            if(object1.immobile || object2.immobile){
                object1.V.x = 0.0f;
                object2.V.x = 0.0f;
            }else{
                float tempVx = object1.V.x * object1.mass;
                object1.V.x = object1.V.x * (1.0f - totalElasticity) + (object2.V.x * object2.mass) * totalElasticity;
                object2.V.x = object2.V.x * (1.0f - totalElasticity) + tempVx * totalElasticity;
            }
        }else{
            if(object1.immobile || object2.immobile){
                object1.V.y = 0.0f;
                object2.V.y = 0.0f;
            }else{
                float tempVy = object1.V.y * object1.mass;
                object1.V.y = object1.V.y * (1.0f - totalElasticity) + (object2.V.y * object2.mass) * totalElasticity;
                object2.V.y = object2.V.y * (1.0f - totalElasticity) + tempVy * totalElasticity;
            }
        }
        
        return true;
    }else{ 
        object1.hitbox.updateSAT();
        object2.hitbox.updateSAT();
        float minOverlap = std::numeric_limits<float>::max();
        glm::vec2 mtv(0.0f);
 
        for (int i = 0; i < 2; i++) {
            glm::vec2 axis = object1.hitbox.normals[i];
            Projection p1 = projectShape(object1.hitbox.SATvertices, axis);
            Projection p2 = projectShape(object2.hitbox.SATvertices, axis);
            float overlap = std::min(p1.max, p2.max) - std::max(p1.min, p2.min);
            if (overlap <= 0.0f) return false;
            if (overlap < minOverlap) {
                minOverlap = overlap;
                glm::vec2 dir(object1.hitbox.X - object2.hitbox.X, object1.hitbox.Y - object2.hitbox.Y);
                mtv = (glm::dot(dir, axis) < 0.0f ? -axis : axis) * overlap;
            }
        }
 
        for (int i = 0; i < 2; i++) {
            glm::vec2 axis = object2.hitbox.normals[i];
            Projection p1 = projectShape(object1.hitbox.SATvertices, axis);
            Projection p2 = projectShape(object2.hitbox.SATvertices, axis);
            float overlap = std::min(p1.max, p2.max) - std::max(p1.min, p2.min);
            if (overlap <= 0.0f) return false;
            if (overlap < minOverlap) {
                minOverlap = overlap;
                glm::vec2 dir(object1.hitbox.X - object2.hitbox.X, object1.hitbox.Y - object2.hitbox.Y);
                mtv = (glm::dot(dir, axis) < 0.0f ? -axis : axis) * overlap;
            }
        }

        if (object1.immobile) {
            object2.Xp -= mtv.x / window_width;
            object2.Yp -= mtv.y / window_height;
        } else if (object2.immobile) {
            object1.Xp += mtv.x / window_width;
            object1.Yp += mtv.y / window_height;
        } else {
            float speed1 = glm::length(object1.V);
            float speed2 = glm::length(object2.V);
            float totalSpeed = speed1 + speed2;
            float share1 = (totalSpeed > 0.0f) ? speed1 / totalSpeed : 0.5f;
            float share2 = (totalSpeed > 0.0f) ? speed2 / totalSpeed : 0.5f;
            object1.Xp += mtv.x / window_width  * share1;
            object1.Yp += mtv.y / window_height * share1;
            object2.Xp -= mtv.x / window_width  * share2;
            object2.Yp -= mtv.y / window_height * share2;
        }
 
        glm::vec2 mtvXpYp(mtv.x / window_width, mtv.y / window_height);
        float mtvLen = glm::length(mtvXpYp);
        if(mtvLen > 0.0001f){
            glm::vec2 mtvDir = mtvXpYp / mtvLen;
 
            float totalElasticity = (object1.elasticity + object2.elasticity) * 0.5f;
 
            if(object1.immobile || object2.immobile){
                Projectile& mobile = object1.immobile ? object2 : object1;
                glm::vec2 awayDir = object1.immobile ? -mtvDir : mtvDir;
                float vDotN = glm::dot(mobile.V, awayDir);
                if(vDotN < 0.0f) mobile.V -= vDotN * awayDir;
            } else {
                float v1n = glm::dot(object1.V, mtvDir);
                float v2n = glm::dot(object2.V, mtvDir);
                float new_v1n = v1n * (1.0f - totalElasticity) + (v2n * object2.mass) * totalElasticity;
                float new_v2n = v2n * (1.0f - totalElasticity) + (v1n * object1.mass) * totalElasticity;
                object1.V += (new_v1n - v1n) * mtvDir;
                object2.V += (new_v2n - v2n) * mtvDir;
            }
        }
        return true;
    }
}

Solid::Solid(float x, float y, float width, float height, short int texture, bool render, bool AABB){
    Xp=x;
    Yp=y;
    this->render = render;
    this->texture = texture;
    hitbox.hitboxWidth = 1.0f;
    hitbox.hitboxHeight = 1.0f;
    solidWidth = width;
    solidHeight = height;
    hitbox.AABB = AABB;
    type = ENTITY;
    hitbox.type = STANDARD;
    this->update();
    hitbox.updateSAT();
    immobile = true;
    elasticity = 1.0f;
}

void Solid::update(){
    float tempSizeRatio = 16.0f * (window_height / startingWindow_height);
    V *= 0.0f;
    A *= 0.0f;
    updateData(Xp, Yp, relativeWH(solidWidth * tempSizeRatio), relativeWH(solidHeight * tempSizeRatio), texture, render, tempSizeRatio / 2.67f, ENTITY);
    hitbox.AABB ? hitbox.angle = 0 : hitbox.angle = angle;
    hitbox.updateData(Xp, Yp, (width / window_height) * hitbox.hitboxWidth, (height / window_height) * hitbox.hitboxHeight, 1001, 1, hitbox.AABB ? STANDARD : ENTITY);
}