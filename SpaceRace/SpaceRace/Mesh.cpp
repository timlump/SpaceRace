#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, Material material)
{
	mVertices = vertices;
	mIndices = indices;
	mMaterial = material;
	setup();
}

void Mesh::draw(Shader *shader)
{
	//set material properties
	GLuint diffuse = glGetUniformLocation(shader->mProgram,"diffuseColor");
	GLuint ambient = glGetUniformLocation(shader->mProgram,"ambientColor");

	glUniformMatrix4fv(diffuse,1,GL_FALSE,glm::value_ptr(mMaterial.diffuse));
	//glUniformMatrix4fv(ambient,1,GL_FALSE,glm::value_ptr(mMaterial.ambient));

	//set textures

	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES,mIndices.size(),GL_UNSIGNED_INT,0);
	glBindVertexArray(0);
}

void Mesh::setup()
{
	glGenVertexArrays(1,&mVAO);

	glBindVertexArray(mVAO);

	glGenBuffers(1,&mVBO);
	glBindBuffer(GL_ARRAY_BUFFER,mVBO);
	glBufferData(GL_ARRAY_BUFFER,mVertices.size()*sizeof(Vertex),&mVertices[0],GL_STATIC_DRAW);

	glGenBuffers(1,&mEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,mIndices.size()*sizeof(GLuint),&mIndices[0],GL_STATIC_DRAW);

	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)0);

	////normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)sizeof(glm::vec3));

	////texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)(sizeof(glm::vec3)*2));

	glBindVertexArray(0);
}