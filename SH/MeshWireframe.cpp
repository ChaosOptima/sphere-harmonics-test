#include "MeshWireframe.h"
#include <iostream>

void MeshWireframe::Config(const MeshDesc& _desc)
{
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, _desc.Count * sizeof(Vertex), _desc.Lines, GL_STATIC_DRAW);

	m_Count = _desc.Count;
	m_ReadyToUse = true;
}

void MeshWireframe::Draw()
{
	if (PreDraw)
		PreDraw();

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glDrawArrays(GL_LINES, 0, m_Count);
	glDisableVertexAttribArray(0);

	if (PostDraw)
		PostDraw();

}
