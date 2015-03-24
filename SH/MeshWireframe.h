#pragma once
#include <functional>
#include "GL\glew.h"

struct Vertex
{
	float x, y, z;
};

struct MeshDesc
{
	Vertex* Lines;
	int Count;
};

class MeshWireframe
{
public:
	
	void Config(const MeshDesc&);
	void Draw();

	std::function<void()> PreDraw;
	std::function<void()> PostDraw;

	bool ReadyToUse() const { return m_ReadyToUse; }
private:
	GLuint m_VBO = 0;
	int m_Count = 0;
	bool m_ReadyToUse = false;
};