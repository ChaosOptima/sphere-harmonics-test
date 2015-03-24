#include "Render.h"
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

using namespace std;

Render::Render(const RenderDesc& _desc)
	: m_Desc(_desc)
{
	if (!glfwInit())
	{
		cout << "glfwInit error\n";
		return;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	m_Window = glfwCreateWindow(m_Desc.SizeX, m_Desc.SizeY, "Kick Ass", 0, 0);
	if (!m_Window)
	{
		cout << "glfwCreateWindow error\n";
		return;
	}

	glfwMakeContextCurrent(m_Window);
	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		cout << "glewInit error\n";
		return;
	}

}


Render::~Render()
{}


void Render::Run()
{
	if (!m_Window)
		return;

	glfwSetInputMode(m_Window, GLFW_STICKY_KEYS, GL_TRUE);
	do
	{
		auto lv_Time = clock();
		m_DeltaTime = (float)(lv_Time - m_Time) / CLOCKS_PER_SEC;
		m_Time = lv_Time;


		Update();		
		Draw();

		glFlush();
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	} while (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(m_Window) == 0);
}

void Render::Update()
{
	if (OnUpdate)
		OnUpdate();
}

void Render::Draw()
{
	glClearColor(0, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(OnDraw)
		OnDraw();

}

GLuint Render::LoadShaders(const char * vertex_shader_path, const char * pixel_shader_path)
{
	GLuint lv_VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint lv_PixelShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	string lv_VertexCode;
	string lv_PixelCode;
	{
		string lv_Line = "";

		ifstream lv_VertexCodeStream(vertex_shader_path, ios::in);
		if (lv_VertexCodeStream.is_open())
		{
			while (getline(lv_VertexCodeStream, lv_Line))
				lv_VertexCode += "\n" + lv_Line;
			lv_VertexCodeStream.close();
		}
		else 
			printf("Vertex shader file cannot be open : %s\n", vertex_shader_path);


		ifstream lv_PixelCodeStream(pixel_shader_path, ios::in);
		if (lv_PixelCodeStream.is_open())
		{
			while (getline(lv_PixelCodeStream, lv_Line))
				lv_PixelCode += "\n" + lv_Line;
			lv_PixelCodeStream.close();
		}
		else
			printf("Pixel shader file cannot be open : %s\n", pixel_shader_path);
	}

	GLint lv_Result = GL_FALSE;
	int lv_LogLength;

	printf("Compiling vertex shader : %s\n", vertex_shader_path);
	{
		char const * lv_VertexSource = lv_VertexCode.c_str();
		glShaderSource(lv_VertexShaderID, 1, &lv_VertexSource, NULL);
		glCompileShader(lv_VertexShaderID);
		glGetShaderiv(lv_VertexShaderID, GL_COMPILE_STATUS, &lv_Result);
		glGetShaderiv(lv_VertexShaderID, GL_INFO_LOG_LENGTH, &lv_LogLength);
		if (lv_LogLength > 0)
		{
			char* lv_ErrorMessage = new char[lv_LogLength];
			glGetShaderInfoLog(lv_VertexShaderID, lv_LogLength, NULL, lv_ErrorMessage);
			cout << lv_ErrorMessage << '\n';
			delete[] lv_ErrorMessage;
		}
	}

	printf("Compiling pixel shader : %s\n", pixel_shader_path);
	{
		char const * lv_PixelSource = lv_PixelCode.c_str();
		glShaderSource(lv_PixelShaderID, 1, &lv_PixelSource, NULL);
		glCompileShader(lv_PixelShaderID);
		glGetShaderiv(lv_PixelShaderID, GL_COMPILE_STATUS, &lv_Result);
		glGetShaderiv(lv_PixelShaderID, GL_INFO_LOG_LENGTH, &lv_LogLength);
		if (lv_LogLength > 0)
		{
			char* lv_ErrorMessage = new char[lv_LogLength];
			glGetShaderInfoLog(lv_PixelShaderID, lv_LogLength, NULL, lv_ErrorMessage);
			cout << lv_ErrorMessage << '\n';
			delete[] lv_ErrorMessage;
		}
	}


	fprintf(stdout, "Linking program\n");
	GLuint lv_ProgramID = glCreateProgram();
	glAttachShader(lv_ProgramID, lv_VertexShaderID);
	glAttachShader(lv_ProgramID, lv_PixelShaderID);
	glLinkProgram(lv_ProgramID);
	glGetProgramiv(lv_ProgramID, GL_LINK_STATUS, &lv_Result);
	glGetProgramiv(lv_ProgramID, GL_INFO_LOG_LENGTH, &lv_LogLength);
	if (lv_LogLength > 0)
	{
		char* lv_ErrorMessage = new char[lv_LogLength];
		glGetProgramInfoLog(lv_ProgramID, lv_LogLength, NULL, lv_ErrorMessage);
		cout << lv_ErrorMessage << '\n';
		delete[] lv_ErrorMessage;
	}

	glDeleteShader(lv_VertexShaderID);
	glDeleteShader(lv_PixelShaderID);

	return lv_ProgramID;
}
