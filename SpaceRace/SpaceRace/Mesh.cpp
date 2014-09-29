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
	GLuint hasDiffuse = glGetUniformLocation(shader->mProgram,"material.hasDiffuse");
	glUniform1i(hasDiffuse,mMaterial.hasDiffuse);

	GLuint hasAmbient = glGetUniformLocation(shader->mProgram,"material.hasAmbient");
	glUniform1i(hasAmbient,mMaterial.hasAmbient);

	GLuint hasSpecular = glGetUniformLocation(shader->mProgram,"material.hasSpecular");
	glUniform1i(hasSpecular,mMaterial.hasSpecular);

	GLuint hasEmission = glGetUniformLocation(shader->mProgram,"material.hasEmission");
	glUniform1i(hasEmission,mMaterial.hasEmission);

	GLuint hasShininess = glGetUniformLocation(shader->mProgram,"material.hasShininess");
	glUniform1i(hasShininess,mMaterial.hasShininess);

	GLuint hasDiffuseTexture = glGetUniformLocation(shader->mProgram,"material.hasDiffuseTexture");
	glUniform1i(hasDiffuseTexture,mMaterial.hasDiffuseTexture);

	GLuint hasAmbientTexture = glGetUniformLocation(shader->mProgram,"material.hasAmbientTexture");
	glUniform1i(hasAmbientTexture,mMaterial.hasAmbientTexture);

	GLuint hasSpecularTexture = glGetUniformLocation(shader->mProgram,"material.hasSpecularTexture");
	glUniform1i(hasSpecularTexture,mMaterial.hasSpecularTexture);

	GLuint hasEmissionTexture = glGetUniformLocation(shader->mProgram,"material.hasEmissionTexture");
	glUniform1i(hasEmissionTexture,mMaterial.hasEmissionTexture);

	GLuint diffuse = glGetUniformLocation(shader->mProgram,"material.diffuse");
	glUniform4fv(diffuse,1,glm::value_ptr(mMaterial.diffuse));

	GLuint ambient = glGetUniformLocation(shader->mProgram,"material.ambient");
	glUniform4fv(ambient,1,glm::value_ptr(mMaterial.ambient));

	GLuint specular = glGetUniformLocation(shader->mProgram,"material.specular");
	glUniform4fv(specular,1,glm::value_ptr(mMaterial.specular));

	GLuint emission = glGetUniformLocation(shader->mProgram,"material.emission");
	glUniform4fv(emission,1,glm::value_ptr(mMaterial.emission));

	GLuint shininess = glGetUniformLocation(shader->mProgram,"material.shininess");
	glUniform1f(shininess,mMaterial.shininess);

	GLuint diffuseTexture = glGetUniformLocation(shader->mProgram,"material.diffuseTexture");
	glUniform1i(diffuseTexture,mMaterial.diffuseTexture);

	GLuint ambientTexture = glGetUniformLocation(shader->mProgram,"material.ambientTexture");
	glUniform1i(ambientTexture,mMaterial.ambientTexture);

	GLuint specularTexture = glGetUniformLocation(shader->mProgram,"material.specularTexture");
	glUniform1i(specularTexture,mMaterial.specularTexture);

	GLuint emissionTexture = glGetUniformLocation(shader->mProgram,"material.emissionTexture");
	glUniform1i(emissionTexture,mMaterial.emissionTexture);

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