#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, Material material)
{

}

Mesh::~Mesh()
{

}

void Mesh::draw(Shader shader)
{

}

void Mesh::setup()
{
	glGenVertexArrays(1,&this->mVAO);
	glGenBuffers(1,&this->mVBO);
	glGenBuffers(1,&this->mEBO);

	glBindVertexArray(this->mVAO);
	glBindBuffer(GL_ARRAY_BUFFER,this->mVBO);
	glBufferData(GL_ARRAY_BUFFER,this->mVertices.size()*sizeof(Vertex),&this->mVertices[0],GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,this->mIndices.size()*sizeof(GLuint),&this->mIndices[0],GL_STATIC_DRAW);

	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)0);

	//normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)sizeof(glm::vec3));

	//texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)(sizeof(glm::vec3)*2));

}