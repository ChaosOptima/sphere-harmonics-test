#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <time.h>
#include <functional>

struct RenderDesc
{
	int SizeX;
	int SizeY;
};

struct CursorPos
{
	double x = -1, y = -1;
	CursorPos() = default;
	CursorPos(double _x, double _y) : x(_x), y(_y){};
};

class Render
{
public:
	Render(const RenderDesc& _desc);
	~Render();

	void Run();
	void Update();
	void Draw();

	void Shader(GLuint val) { m_Shader = val; }
	void Texture(GLuint val) { m_Texture = val; }

	GLuint Shader() const { return m_Shader; }
	GLuint Texture() const { return m_Texture; }

	std::function<void()> OnUpdate;
	std::function<void()> OnDraw;

	bool KeyPressed(int _key) { return glfwGetKey(m_Window, _key) == GLFW_PRESS; }
	bool MouseButton(int _button) { return glfwGetMouseButton(m_Window, _button) == GLFW_PRESS; }
	CursorPos MousePos() { double x, y; glfwGetCursorPos(m_Window, &x, &y); return{ x, y }; }

	clock_t Time() const { return m_Time; }
	float DeltaTime() const { return m_DeltaTime; }

	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
private:

	GLFWwindow* m_Window = 0;
	GLuint m_Shader = 0;
	GLuint m_Texture = 0;

	clock_t m_Time = clock();
	float m_DeltaTime = 0;
	RenderDesc m_Desc;

};