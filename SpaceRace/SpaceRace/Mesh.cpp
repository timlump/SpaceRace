#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, Material material)
{
	this->mVertices = vertices;
	this->mIndices = indices;
	this->mTextures = textures;
	this->mMaterial = material;
	this->setup();
}

Mesh::~Mesh()
{
	glDeleteBuffers(1,&mEBO);
	glDeleteBuffers(1,&mVBO);
	glDeleteVertexArrays(1,&mVAO);
}

void Mesh::draw(Shader shader)
{
	//to do texture

	glBindVertexArray(this->mVAO);
	glDrawElements(GL_TRIANGLES,this->mIndices.size(),GL_UNSIGNED_INT,0);
	glBindVertexArray(0);
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