#ifndef SHADER_H
#define SHADER_H

#include "glad.h"
#include "stb_image.h"
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include "inputHandler.cpp"

extern float window_width, window_height, startingWindow_width, startingWindow_height, change1, change2, change3, deltaTime, lastX, lastY, zoom;
extern Input input;
extern Debug debug;

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

void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color);

class Texture {
public:
    unsigned int ID;
    unsigned char* data;
    Texture(const char* texturePath, short int textureNumber, GLint internalformat, bool atlas, int size);
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

enum quadType {
    STANDARD,
    ENTITY
};

class Quad {
public:

Quad(Quad&& other) noexcept;
Quad(const Quad&) = delete;

    static unsigned int VAO, VBO, EBO;
    float X = 0.0f, Y = 0.0f, width, height, sizeRatio, texXmin, texYmin, preX, preY, tileSize = 0.0f, angle = 0.0f, preAngle = 0.0f;
    int texture;
    static std::vector<float> vertices;
    bool added = false;
    unsigned int verticesStartIndex;
    bool render = true;
    glm::mat4 model = glm::mat4(1.0f);
    int type = 0;

    Quad(float x, float y, float width, float height, int texture, bool render, float sizeRatio, int type);
    Quad();
    void updateVertices();
    void updateData(float x, float y, float width, float height, int texture, bool render, float sizeRatio, int type);
    void updateBuffers();
    void toString();
    void draw(Shader &s);
    float relativeX(float x);
    float relativeY(float y);
    float relativeWH(float sizeRatio);
    bool mouseOver(float x, float y, float width, float height);
    ~Quad();

private:
};

/*Future child classes  must have: class "child" : public "parent class"
                              and: using "parent"::"parent"; */
enum callbacks {
    NOARGS,
    INT,
    GUI
};

class myGUI;

class buttonContainer : public Quad{
public:
    using Quad::Quad;

    Quad left;
    Quad background;
    Quad right;
    std::string text = "";
    glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);
    short int texture;
    short int status = 0;
    float counter = 0.0f;
    short int callbackID = NOARGS;
    
    std::function<void()> callBack = [](){};
    std::function<void(int x)> callBackInt = [](int x){};
    std::function<void(myGUI* gui)> callBackGUI = [](myGUI* gui){};
    
    buttonContainer(float x, float y, float width);
    void setCallback(std::function<void()> func);
    void setCallback(std::function<void(int x)> func);
    void setCallback(std::function<void(myGUI* gui)> func);
    bool over();

};

class myGUI : public Quad {
public:
    using Quad::Quad;

    Quad bottom;
    Quad top;
    Quad left;
    Quad right;
    Quad background;
    std::vector<buttonContainer> buttons;
    Shader *textShader;

    myGUI(float x, float y, float width, float height, bool render, float sizeRatio);
    void updateGUI();
    void draw(Shader &s);
    void toString();
    void addButton(float x, float y, float width, short int texture);
    void addButton(float x, float y, float width, std::string text, glm::vec3 color, short int texture);
};

class Entity : public Quad {
public:
    using Quad::Quad;
    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;

    float Xp = 0.5f, Yp = 0.5f, scale = 1.0f, mass = 1.0f, maxSelfGivenV = 1.0f;
    glm::vec2 V = glm::vec2(0.0f), A = glm::vec2(0.0f), friction = glm::vec2(0.0f);
;
    std::vector<glm::vec2> As;

    Entity(float x, float y, short int texture, float scale, bool render);
    void update();
    void move();
    float pointTowards(float x, float y, bool percentage);
    glm::vec2 dirScalar(float mag, float dir);
};

class Hitbox : public Quad {
public:
    using Quad::Quad;

    bool AABB = true;
    float hitboxWidth = 1.0f, hitboxHeight = 1.0f;
    glm::vec2 normals[4];
    glm::vec2 SATvertices[4];
    Hitbox();
    void updateData(float x, float y, float width, float height, int texture, bool render, float sizeRatio, int type);
    void updateSAT();
};

class Projectile : public Entity {
public:
    Projectile(Projectile&&) noexcept = default;
    Projectile& operator=(Projectile&&) noexcept = default;

    Hitbox hitbox;

    float elasticity = 1.0f;

    Projectile(float x, float y, short int texture, float scale, bool render, bool AABB);
    void update();
    void draw(Shader &s);
    bool colliding(Projectile &object1, Projectile &object2);
};

#endif