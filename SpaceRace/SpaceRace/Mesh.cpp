#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, Material material, const aiScene *scene, aiMesh *mesh)
{
	mVertices = vertices;
	mIndices = indices;
	mGlobalInverseTransform = scene->mRootNode->mTransformation;
	mGlobalInverseTransform.Inverse();
	mScene = scene;
	mMesh = mesh;
	setup();
}

void Mesh::animate(std::string name,double time)
{
	
}

glm::mat4 Mesh::aMat4toGLMMat4(aiMatrix4x4 &matrix)
{
	return glm::mat4
		(
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		matrix.d1,matrix.d2,matrix.d3,matrix.d4);
}

glm::vec3 Mesh::aVec3toGLMVec3(aiVector3D &vector)
{
	return glm::vec3(vector.x,vector.y,vector.z);
}

glm::quat Mesh::aQuattoGLMQuat(aiQuaternion &quat)
{
	glm::quat result;
	result.x = quat.x;
	result.y = quat.y;
	result.z = quat.z;
	result.w = quat.w;
	return result;
}

glm::vec3 Mesh::lerp(float &time, glm::vec3 &start, glm::vec3 &end)
{
	return start*(1.0f-time) + end*time;
}

void Mesh::draw(Shader *shader)
{
#pragma region uniforms
	GLuint hasBones = glGetUniformLocation(shader->mProgram,"hasBones");
	glUniform1i(hasBones,mMaterial.hasBones);

	if(mMaterial.hasBones)
	{
		for(int i = 0 ; i < mBoneInfo.size() ; i++)
		{
			char buffer[80];
			sprintf(buffer,"bones[%d]",i);
			GLuint bones = glGetUniformLocation(shader->mProgram,buffer);

			glm::mat4 transform = aMat4toGLMMat4(mBoneInfo[i].finalTransformation);

			glUniformMatrix4fv(bones,1,GL_FALSE,glm::value_ptr(transform));
		}
	}

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

	if(mMaterial.hasDiffuse)
	{
		GLuint diffuse = glGetUniformLocation(shader->mProgram,"material.diffuse");
		glUniform4fv(diffuse,1,glm::value_ptr(mMaterial.diffuse));
	}

	if(mMaterial.hasAmbient)
	{
		GLuint ambient = glGetUniformLocation(shader->mProgram,"material.ambient");
		glUniform4fv(ambient,1,glm::value_ptr(mMaterial.ambient));
	}

	if(mMaterial.hasSpecular)
	{
		GLuint specular = glGetUniformLocation(shader->mProgram,"material.specular");
		glUniform4fv(specular,1,glm::value_ptr(mMaterial.specular));
	}

	if(mMaterial.hasEmission)
	{
		GLuint emission = glGetUniformLocation(shader->mProgram,"material.emission");
		glUniform4fv(emission,1,glm::value_ptr(mMaterial.emission));
	}

	if(mMaterial.hasShininess)
	{
		GLuint shininess = glGetUniformLocation(shader->mProgram,"material.shininess");
		glUniform1f(shininess,mMaterial.shininess);
	}

	GLuint textureOffset = 0;

	if(mMaterial.hasDiffuseTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		GLuint diffuseTexture = glGetUniformLocation(shader->mProgram,"material.diffuseTexture");
		glUniform1i(diffuseTexture,textureOffset);
		glBindTexture(GL_TEXTURE_2D,mMaterial.diffuseTexture);
		textureOffset++;
	}

	if(mMaterial.hasAmbientTexture)
	{
		glActiveTexture(GL_TEXTURE0+textureOffset);
		GLuint ambientTexture = glGetUniformLocation(shader->mProgram,"material.ambientTexture");
		glUniform1i(ambientTexture,textureOffset);
		glBindTexture(GL_TEXTURE_2D,mMaterial.ambientTexture);
		textureOffset++;
	}

	if(mMaterial.hasSpecularTexture)
	{
		glActiveTexture(GL_TEXTURE0+textureOffset);
		GLuint specularTexture = glGetUniformLocation(shader->mProgram,"material.specularTexture");
		glUniform1i(specularTexture,textureOffset);
		glBindTexture(GL_TEXTURE_2D,mMaterial.specularTexture);
		textureOffset++;
	}

	if(mMaterial.hasEmissionTexture)
	{
		glActiveTexture(GL_TEXTURE0+textureOffset);
		GLuint emissionTexture = glGetUniformLocation(shader->mProgram,"material.emissionTexture");
		glUniform1i(emissionTexture,textureOffset);
		glBindTexture(GL_TEXTURE_2D,mMaterial.emissionTexture);
	}
	glActiveTexture(GL_TEXTURE0);
#pragma endregion uniforms

	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES,mIndices.size(),GL_UNSIGNED_INT,0);
	glBindVertexArray(0);
}

void Mesh::loadBones()
{
	std::map<int,VertexBone> bones;

	for(int i = 0 ; i < mMesh->mNumBones ; i++)
	{
		int boneIndex = 0;
		aiBone *bone = mMesh->mBones[i];
		std::string boneName(bone->mName.data);

		if(mBoneMapping.find(boneName) == mBoneMapping.end())
		{
			BoneInfo info;
			mBoneInfo.push_back(info);

			mBoneInfo[boneIndex].boneOffset = bone->mOffsetMatrix;
			mBoneMapping[boneName] = boneIndex;

			boneIndex++;
		}
		else
		{
			boneIndex = mBoneMapping[boneName];
		}

		//map vertices to bones
		for(int j = 0 ; j < bone->mNumWeights ; j++)
		{
			aiVertexWeight weight = bone->mWeights[j];
			if(bones.find(weight.mVertexId) == bones.end())
			{
				VertexBone b;
				b.boneWeights = glm::vec4(0.0f);
				b.boneIDs = glm::ivec4(0.0f);

				b.boneIDs[0] = boneIndex;
				b.boneWeights[0] = weight.mWeight;
				b.numWeights = 1;
				bones[weight.mVertexId] = b;
			}
			else
			{
				int index = bones[weight.mVertexId].numWeights;
				if(index < 4)
				{
					bones[weight.mVertexId].boneIDs[index] = boneIndex;
					bones[weight.mVertexId].boneWeights[index] = weight.mWeight;
					bones[weight.mVertexId].numWeights++;
				}
			}
		}

		//pump these out to bones vector
		std::map<int,VertexBone>::iterator it;
		for(it = bones.begin() ; it != bones.end() ; it++)
		{
			//dont forget to fill the bones vector to the size of the number of vertices

		}

	}
}

void Mesh::setup()
{
	loadBones();

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

	glGenBuffers(1,&mBBO);
	glBindBuffer(GL_ARRAY_BUFFER,mBBO);
	glBufferData(GL_ARRAY_BUFFER,mBones.size()*sizeof(VertexBone),&mBones[0],GL_STATIC_DRAW);

	//bones
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3,4,GL_INT,sizeof(VertexBone),(GLvoid*)0);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4,4,GL_FLOAT,GL_FALSE,sizeof(VertexBone),(GLvoid*)(sizeof(glm::ivec4)));

	glBindVertexArray(0);
}