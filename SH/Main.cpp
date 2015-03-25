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




double Test(double theta, double phi)
{
	return max(0.0f, 5.0f * cosf(theta) - 4.0f) + max(0.0f, -4.0f * sinf(theta - PI) * cosf(phi - 2.5f) - 3.0f);
}


const int SqrtSHCoefCount = 2;
const int SqrtSamplesCount = 100;
const int SamplesCount = SqrtSamplesCount * SqrtSamplesCount;
const int SHCoefCount = SqrtSHCoefCount * SqrtSHCoefCount;

SHSample Samples[SamplesCount];
double SHCoef[SHCoefCount];

void CalcCoeficents()
{
	sphericalStratifiedSampling(Samples, SqrtSamplesCount, SqrtSHCoefCount);
	SHProjectSphericalFunction(&Test, Samples, SHCoef, SamplesCount, SHCoefCount);
}

double Approxim(double theta, double phi)
{
	double Result = 0;
	int k = 0;
	for (int l = 0; k < SqrtSHCoefCount; l++)
		for (int m = -l; m <= l && k < SqrtSHCoefCount; m++)
		{
			Result += SHCoef[k] * evaluateSH(l, m, theta, phi);
			k++;
		}

	return Result;
}



int Semples = 10;

Vertex VertexAt(int i, int j)
{
	int m = 0;
	int l = 2;

	float theta = (float)i rad;
	float phi = (float)j rad;

	float r = 0;
	//if (l == 1 && m == 0)
	//	r = sqrt(3.0f / (PI * 4.0f)) * cosf(theta);
	r = FevaluateSH(l, m, theta, phi);
	//r = Test(theta, phi);
	//r = Approxim(theta, phi) * 2;
	r = abs(r);
	//r = 1;

	return 
	{
		r * sinf(theta) * cosf(phi),
		r * sinf(theta) * sinf(phi),
		r * cosf(theta),
	};
}

Vertex VertexAt2(int i, int j)
{
	int m = 0;
	int l = 2;

	float theta = (float)i rad;
	float phi = (float)j rad;

	float r = Test(theta, phi);
	r = evaluateSH(l, m, theta, phi);
	r = abs(r);

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
			CalcCoeficents();
			vector<Vertex> Lines;

			if (true)
			{
				//if(false)
				for (int i = 0; i < 360; i += Semples)
					for (int j = 0; j < 360; j += Semples)
					{
						Lines.push_back(VertexAt(i,				j));
						Lines.push_back(VertexAt(i + Semples,	j));
						Lines.push_back(VertexAt(i,				j));
						Lines.push_back(VertexAt(i,	  j + Semples));
					}/**/

				//if (false)
				for (int i = 0; i < 360; i += Semples)
					for (int j = 0; j < 360; j += Semples)
					{
						Lines.push_back(VertexAt2(i, j));
						Lines.push_back(VertexAt2(i + Semples, j));
						Lines.push_back(VertexAt2(i, j));
						Lines.push_back(VertexAt2(i, j + Semples));
					}/**/

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