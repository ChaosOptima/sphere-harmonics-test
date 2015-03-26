#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/compatibility.hpp"

#include <iostream>
#include <memory>
#include <thread>
#include <mutex>

#include "Render.h"
#include "Macro.h"
#include "MeshWireframe.h"
#include <vector>
#include <math.h>
#include "SH.h"
#include "glm/gtx/transform.hpp"


using namespace std;
using namespace glm;

void UpdateProjection()
{
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 5, 100);

	// select modelview matrix and clear it out
	glMatrixMode(GL_MODELVIEW);
}

int m = 4;
int l = 5;
int Semples = 2;

Vertex VertexAt(int i, int j, int l, int m)
{
	static auto PI = 3.14159265;

	float theta = (float)j rad;
	float phi = (float)i rad;
	float r = abs(evaluateSH(l, m, theta, phi ));

	return 
	{
		r * sinf(theta) * cosf(phi),
		r * sinf(theta) * sinf(phi),
		r * cosf(theta),
	};
}

void AddPoints(vector<Vertex> &Lines, int l, int m)
{
	float lv_Step = 0.01;
	float lv_Scale = 10;
	for (float i = -1; i < 1; i += lv_Step)
	{
		Lines.push_back({ i * lv_Scale, ALPStd(i, l, m) * lv_Scale, -15 });
		Lines.push_back({ (i + lv_Step) * lv_Scale, ALPStd(i + lv_Step, l, m) * lv_Scale, -15 });
	}
}

void InitUpdate(Render& _Render)
{
	glViewport(0, 0, 800, 600);
	auto View = lookAt(float3{ 0, 0, 2 }, float3{ 0, 0, 0 }, float3{ 0, 1, 0 });
	auto Proj = perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	float angle = 0;

	
	MeshWireframe Mesh;
	Render* RenderPtr = &_Render;
	GLuint Shader;



	_Render.OnDraw = [=]()mutable
	{
		if (!Mesh.ReadyToUse())
		{
			vector<Vertex> Lines;

			if (true)
			{
				for (int i = -180; i < 180; i += Semples)
					for (int j = -180; j < 180; j += Semples)
					{
						Lines.push_back(VertexAt(i,				j, l, m));
						Lines.push_back(VertexAt(i + Semples,	j, l, m));
						Lines.push_back(VertexAt(i,				j, l, m));
						Lines.push_back(VertexAt(i,	  j + Semples, l, m));
					}
			}


			if (false)
			{
				AddPoints(Lines, 0, 0);

				AddPoints(Lines, 1, 0);
				AddPoints(Lines, 1, 1);

				AddPoints(Lines, 2, 0);
				AddPoints(Lines, 2, 1);
				AddPoints(Lines, 2, 2);
				
			}


			MeshDesc lv_Desc;
			lv_Desc.Count = Lines.size();
			lv_Desc.Lines = &Lines[0];

			Mesh.Config(lv_Desc);


			Shader = RenderPtr->LoadShaders("Shaders/VertexShader.txt", "Shaders/PixelShader.txt");
			auto ViewLoc = glGetUniformLocation(Shader, "View");
			auto ProjLoc = glGetUniformLocation(Shader, "Proj");
			auto WorldLoc = glGetUniformLocation(Shader, "World");


			Mesh.PreDraw = [Shader, ViewLoc, View, ProjLoc, Proj, WorldLoc, angle]() mutable
			{
				auto World = rotate(angle rad, float3{ 0, 1, 0 });

				angle += 1.0f / 3;

				glUseProgram(Shader);			
				glUniformMatrix4fv(ViewLoc, 1, false, &View[0][0]);
				glUniformMatrix4fv(ProjLoc, 1, false, &Proj[0][0]);
				glUniformMatrix4fv(WorldLoc, 1, false, &World[0][0]);

			};

			Mesh.PostDraw = []()
			{
				glUseProgram(0);
			};
		}
		
		Mesh.Draw();		
	};
}

void main()
{

	RenderDesc lv_Desc;
	lv_Desc.SizeX = 800;
	lv_Desc.SizeY = 600;

	Render lv_Render(lv_Desc);
	UpdateProjection();
	InitUpdate(lv_Render);
	lv_Render.Run();

}