#include <iostream>
#include <cmath>
#include <map>
#include <unordered_map>
#include <string>
#include <time.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <filesystem>

#include "glad.h"
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

double pi = 3.141592653589793;
float deltaTime = 0.0f, lastTime = 0.0f, change1 = 0.0f, change2 = 0.0f, change3 = 0.0f, window_width, window_height, startingWindow_width, startingWindow_height;
glm::mat4 quadProjection = glm::mat4(1.0f);
glm::mat4 projectionText = glm::mat4(1.0f);
float lastX = 800.0f / 2.0f;
float lastY = 800.0f / 2.0f;

#include "classes.h" //includes inputHandler.cpp

Input input;
Debug debug;

#define debugInputs()\
if (pressed(BUTTON_0))	debug.showWireFrame = !debug.showWireFrame;\
if (pressed(BUTTON_ESCAPE)){\
	debug.fullscreen = !debug.fullscreen;\
	glfwSetWindowAttrib(window, GLFW_DECORATED, !debug.fullscreen);\
	if(debug.fullscreen) {\
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, startingWindow_width, startingWindow_height, 60);\
		glfwSetWindowSize(window, startingWindow_width, startingWindow_height);\
	} else {\
		glfwSetWindowMonitor(window, nullptr, 0, 0, startingWindow_width, startingWindow_height, 60);\
		glfwSetWindowSize(window, static_cast<int>(startingWindow_width * 0.8f), static_cast<int>(startingWindow_height * 0.8f));\
	}\
}\
if (held(BUTTON_LEFT_CTRL) && held(BUTTON_LEFTSHIFT) && pressed(BUTTON_P)) {\
	glfwSetWindowShouldClose(window, GLFW_TRUE);\
	std::cout << "\nWindow force closed\n";\
}\
if (debug.nextFPScounter > debug.FPS) {\
	debug.nextFPScounter = 0;\
	debug.FPS = static_cast<short>(1.0f / deltaTime);\
}\
else {\
	debug.nextFPScounter++;\
}\
if (debug.showWireFrame) {\
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);\
}\
else {\
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);\
}\

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void window_focus_callback(GLFWwindow* window, int focused);
void mouse_button_callback(GLFWwindow* window, int button, int action, int bits);

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};
std::map<char, Character> Characters;
unsigned int textVAO, textVBO;

bool firstMouse = true;

int main(){
    srand(static_cast<unsigned int>(time(NULL)));
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	window_width = mode->width;
	window_height = mode->height;
	startingWindow_width = window_width;
	startingWindow_height = window_height;

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "openGL window", glfwGetPrimaryMonitor(), NULL);
	glfwMakeContextCurrent(window);
	std::cout << "Context set to current. ALL couts must be after this.\n";
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetWindowFocusCallback(window, window_focus_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");

	glEnable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0xFF);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader text1Shader("../resources/shaders/text.vert", "../resources/shaders/text.frag");
	projectionText = glm::ortho(0.0f, float(mode->width), 0.0f, float(mode->height));
	text1Shader.use();
	glUniformMatrix4fv(glGetUniformLocation(text1Shader.ID, "projectionText"), 1, GL_FALSE, glm::value_ptr(projectionText));

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return -1;
	}

	FT_Face face1;
	if (FT_New_Face(ft, "../resources/fonts/mainFont.ttf", 0, &face1))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return -1;
	}
	FT_Set_Pixel_Sizes(face1, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(face1, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texts;
		glGenTextures(1, &texts);
		glBindTexture(GL_TEXTURE_2D, texts);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face1->glyph->bitmap.width,
			face1->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face1->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texts,
			glm::ivec2(face1->glyph->bitmap.width, face1->glyph->bitmap.rows),
			glm::ivec2(face1->glyph->bitmap_left, face1->glyph->bitmap_top),
			static_cast<unsigned int>(face1->glyph->advance.x)
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}

	FT_Done_Face(face1);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Texture quadTexture("../resources/textures/atlas.png", 1, GL_RGBA, false, 1);
	
	Shader quadShader("../resources/shaders/quad.vert", "../resources/shaders/quad.frag");
	glGenVertexArrays(1, &Quad::VAO);
    glGenBuffers(1, &Quad::VBO);
	std::vector<Quad> quads;
	quads.emplace_back(Quad(window_width * 0.5f, window_height * 0.5f, 128.0f, 128.0f, 992, true, 4.0f, STANDARD));
	quads.emplace_back(Quad(window_width * 0.15f, window_height * 0.75f, 150.0f, 300.0f, 993, false, 8.0f, STANDARD));
	quads.emplace_back(Quad(window_width * 0.35f, window_height * 0.6f, 350.0f, 250.0f, 994, false, 2.0f, STANDARD));

	std::vector<myGUI> guis;
	guis.emplace_back(myGUI(0.33f, 0.4f, 0.755f, 0.5f, true, 4.0f));
	guis[0].textShader = &text1Shader;
	guis[0].addButton(0.33f, 0.75f, 0.3f, "Can't get enough", glm::vec3(1.0f, 0.0f, 1.0f), 997);
	/*guis[0].addButton(0.66f, 0.75f, 0.3f, "Can't get enough", glm::vec3(0.0f, 1.0f, 1.0f));
	guis[0].addButton(0.33f, 0.5f, 0.3f, "Can't get enough", glm::vec3(1.0f, 1.0f, 0.0f));
	guis[0].addButton(0.66f, 0.5f, 0.3f, "Can't get enough", glm::vec3(1.0f, 0.5f, 0.5f));
	guis[0].addButton(0.5f, 0.25f, 0.5f);

	guis.emplace_back(myGUI(0.66f, 0.6f, 0.3f, 0.7f, true, 2.0f));
	guis[1].textShader = &text1Shader;
	guis[1].addButton(0.5f, 0.8f, 0.2f, "Please", glm::vec3(1.0f, 0.0f, 1.0f));
	guis[1].addButton(0.5f, 0.5f, 0.22f, "Pass the", glm::vec3(0.0f, 1.0f, 0.5f));
	guis[1].addButton(0.5f, 0.2f, 0.17f, "Peas!", glm::vec3(0.5f, 0.0f, 1.0f));*/

	quadProjection = glm::ortho(0.0f, float(mode->width), 0.0f, float(mode->height));
	quadShader.use();
	quadShader.setInt("texID", 1);
	quadShader.setMat4("projection", quadProjection);

    while (!glfwWindowShouldClose(window)) {
    	for (int i = 0; i < BUTTON_COUNT; i++) {
    	    input.buttons[i].is_changed = false;
    	}

    	float currentTime = float(glfwGetTime());
    	deltaTime = currentTime - lastTime;
    	lastTime = currentTime;

    	glfwPollEvents();
		glClearColor(0.8f, 0.4f, 0.4f, 1.0f);
    	glClear(GL_COLOR_BUFFER_BIT);

		quadShader.use();
		quadShader.setMat4("projection", quadProjection);

		for(auto& currentQuad : guis){
			currentQuad.updateGUI();
			currentQuad.draw(quadShader);
		}
		
    	debugInputs();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("ImGui window");
		ImGui::Text("FPS: = %i", debug.FPS);
		ImGui::SliderFloat("height", &guis[0].height, 0.0f, 1.0f, "%.3f", 0);
		ImGui::SliderFloat("width", &guis[0].width, 0.0f, 1.0f, "%.3f", 0);
		ImGui::SliderFloat("x", &guis[0].X, 0.0f, 1.0f, "%.3f", 0);
		ImGui::SliderFloat("y", &guis[0].Y, 0.0f, 1.0f, "%.3f", 0);
		ImGui::SliderFloat("size ratio", &guis[0].sizeRatio, 1.0f, 16.0f, "%.f", 0);
		ImGui::SliderFloat("change1", &change1, 0.01f, 2.0f, "%.3f", 0);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    	glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &Quad::VAO);
    glDeleteBuffers(1, &Quad::VBO);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
    glfwTerminate();
	return 0;
}

void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color) {
 	//example: RenderText(text1Shader, "FPS: " + std::to_string(debug.FPS), 10.0f, window_height + 10.0f, 0.7f, glm::vec3(1.0f, 0.0f, 0.0f));
	// activate corresponding render state	
	x *= window_width;
	y *= window_height;
	scale *= window_height / startingWindow_height;
	s.use();
	s.setMat4("projectionText", projectionText);
	s.setVec3("textColor", glm::vec3(color.x, color.y, color.z));
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	window_width = width;
	window_height = height;
	quadProjection = glm::ortho(0.0f, float(window_width), 0.0f, float(window_height));
	projectionText = glm::ortho(0.0f, float(window_width), 0.0f, float(window_height));
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == 2) action = 1;

#define process_button(b, keyCode)\
case keyCode: {\
input.buttons[b].is_changed = (bool(action) != input.buttons[b].is_down);\
input.buttons[b].is_down = bool(action);\
} break;\

	switch (key) {
		process_button(BUTTON_SPACE, 32);
		process_button(BUTTON_F3, 292);
		process_button(BUTTON_0, 48);
		process_button(BUTTON_1, 49);
		process_button(BUTTON_2, 50);
		process_button(BUTTON_3, 51);
		process_button(BUTTON_4, 52);
		process_button(BUTTON_5, 53);
		process_button(BUTTON_6, 54);
		process_button(BUTTON_7, 55);
		process_button(BUTTON_8, 56);
		process_button(BUTTON_9, 57);
		process_button(BUTTON_F11, 300);
		process_button(BUTTON_W, 87);
		process_button(BUTTON_A, 65);
		process_button(BUTTON_S, 83);
		process_button(BUTTON_D, 68);
		process_button(BUTTON_LEFTSHIFT, 340);
		process_button(BUTTON_ESCAPE, 256);
		process_button(BUTTON_F4, 293);
		process_button(BUTTON_B, 66);
		process_button(BUTTON_C, 67);
		process_button(BUTTON_E, 69);
		process_button(BUTTON_F, 70);
		process_button(BUTTON_G, 71);
		process_button(BUTTON_H, 72);
		process_button(BUTTON_I, 73);
		process_button(BUTTON_J, 74);
		process_button(BUTTON_K, 75);
		process_button(BUTTON_L, 76);
		process_button(BUTTON_M, 77);
		process_button(BUTTON_N, 78);
		process_button(BUTTON_O, 79);
		process_button(BUTTON_P, 80);
		process_button(BUTTON_Q, 81);
		process_button(BUTTON_R, 82);
		process_button(BUTTON_T, 84);
		process_button(BUTTON_U, 85);
		process_button(BUTTON_V, 86);
		process_button(BUTTON_X, 88);
		process_button(BUTTON_Y, 89);
		process_button(BUTTON_Z, 90);
		process_button(BUTTON_LEFT_CTRL, 341);
		process_button(BUTTON_LEFT, 263);
		process_button(BUTTON_RIGHT, 262);
		process_button(BUTTON_UP, 265);
		process_button(BUTTON_DOWN, 264);
	default:
		break;
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	if (debug.mouseLocked) {
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (debug.mouseLocked) {

	}
}

void window_focus_callback(GLFWwindow* window, int focused) {
	for (int i = 0; i < BUTTON_COUNT; i++) {
        input.buttons[i].is_down = false;
        input.buttons[i].is_changed = false;
    }
	bool leftButtonPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool rightButtonPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    input.buttons[MOUSE_LEFT].is_down = leftButtonPressed;
    input.buttons[MOUSE_RIGHT].is_down = rightButtonPressed;
    if (focused) {
        // Explicitly query current cursor position and update tracking
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastX = static_cast<float>(xpos); 
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }else{
		firstMouse = true;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

#define process_button(b, keyCode)\
case keyCode: {\
input.buttons[b].is_changed = (bool(action) != input.buttons[b].is_down);\
input.buttons[b].is_down = bool(action);\
} break;\

	switch (button){
		process_button(MOUSE_LEFT, 0);
		process_button(MOUSE_RIGHT, 1);
		process_button(MOUSE_SCROLL_WHEEL, 2);
		process_button(MOUSE_LEFT_BACK, 3);
		process_button(MOUSE_LEFT_FRONT, 4);
	default:
		break;
	}
}

