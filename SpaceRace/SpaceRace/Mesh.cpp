#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<VertexBone> bones, std::vector<glm::mat4> boneOffsets, std::vector<Bone*> boneHierarchy, std::map<std::string,std::map<int,BoneAnimation>> animations, std::vector<glm::mat4> boneTransforms, Material material)
{
	mVertices = vertices;
	mIndices = indices;
	mBones = bones;
	mBoneHierarchy = boneHierarchy;
	mBoneTransforms = boneTransforms;
	mAnimations = animations;
	mMaterial = material;
	mBoneOffsets = boneOffsets;
	setup();
}

void Mesh::animate(std::string name,double time)
{
	auto animEntry = mAnimations.find(name);
	if(animEntry != mAnimations.end())
	{
		for(int i = 0 ; i < mBoneHierarchy.size() ; i++)
		{
			Bone *bone = mBoneHierarchy[i];
			traverseTreeApplyTransformations(bone,animEntry->second,time,glm::mat4(1.0f));
		}
	}
}

glm::mat4 Mesh::aMat4toGLMMat4(aiMatrix4x4 &matrix)
{
	glm::mat4 result;
	for(int i = 0 ; i < 4 ; i++)
	{
		for(int j = 0 ; j < 4 ; j++)
		{
			result[i][j] = matrix[j][i];
		}
	}
	return result;
}

glm::vec3 Mesh::aVec3toGLMVec3(aiVector3D &vector)
{
	return glm::vec3(vector.x,vector.y,vector.z);
}

glm::quat Mesh::aQuattoGLMQuat(aiQuaternion &quat)
{
	return glm::quat(quat.w,quat.x,quat.y,quat.z);
}

glm::vec3 Mesh::lerp(float &time, glm::vec3 &start, glm::vec3 &end)
{
	return start*(1.0f-time) + end*time;
}

glm::mat4 Mesh::calculatePosition(BoneAnimation *anim)
{
	glm::mat4 result(1.0f);
	//if only one key
	if(anim->positions.size() == 1)
	{
		glm::vec3 translate = aVec3toGLMVec3(anim->positions[0].mValue);
		result = glm::translate(result,translate);
	}
	//if many keys
	else
	{
		//find current range
		for(int i = 0 ; i < anim->positions.size()-1 ; i++)
		{
			aiVectorKey before = anim->positions[i];
			aiVectorKey after = anim->positions[i+1];
			//check if range is correct
			if(anim->currentTick >= before.mTime && anim->currentTick < after.mTime)
			{
				glm::vec3 start = aVec3toGLMVec3(before.mValue);
				glm::vec3 end = aVec3toGLMVec3(after.mValue);

				float time = (anim->currentTick-before.mTime)/(after.mTime-before.mTime);
				result = glm::translate(result,lerp(time,start,end));
				break;
			}
		}
	}
	return result;
}

glm::mat4 Mesh::calculateScale(BoneAnimation *anim)
{
	glm::mat4 result(1.0f);
	//if only one key
	if(anim->scalings.size() == 1)
	{
		glm::vec3 scale = aVec3toGLMVec3(anim->scalings[0].mValue);
		result = glm::scale(result,scale);
	}
	//if many keys
	else
	{
		//find current range
		for(int i = 0 ; i < anim->scalings.size()-1 ; i++)
		{
			aiVectorKey before = anim->scalings[i];
			aiVectorKey after = anim->scalings[i+1];
			//check if range is correct
			if(anim->currentTick >= before.mTime && anim->currentTick < after.mTime)
			{
				glm::vec3 start = aVec3toGLMVec3(before.mValue);
				glm::vec3 end = aVec3toGLMVec3(after.mValue);

				float time = (anim->currentTick-before.mTime)/(after.mTime-before.mTime);
				result = glm::scale(result,lerp(time,start,end));
				break;
			}
		}
	}
	return result;
}

glm::mat4 Mesh::calculateRotation(BoneAnimation *anim)
{
	glm::mat4 result(1.0f);
	//if only one key
	if(anim->rotations.size() == 1)
	{
		glm::quat rot = aQuattoGLMQuat(anim->rotations[0].mValue);
		result = glm::mat4_cast(rot);

	}
	//if many keys
	else
	{
		//find current range
		for(int i = 0 ; i < anim->rotations.size()-1 ; i++)
		{
			aiQuatKey before = anim->rotations[i];
			aiQuatKey after = anim->rotations[i+1];
			//check if range is correct
			if(anim->currentTick >= before.mTime && anim->currentTick < after.mTime)
			{
				glm::quat start = aQuattoGLMQuat(before.mValue);
				glm::quat end = aQuattoGLMQuat(after.mValue);

				float time = (anim->currentTick-before.mTime)/(after.mTime-before.mTime);

				glm::quat rot = glm::slerp(start,end,time);

				result = glm::mat4_cast(rot);
				break;
			}
		}
	}
	return result;
}

void Mesh::traverseTreeApplyTransformations(Bone *bone, std::map<int,BoneAnimation> &animation, double timeStep, glm::mat4 &parentTransform)
{
	Bone* b = bone;
	int index = b->index;

	if(index != -1)
	{
		//calculate current time
		BoneAnimation *anim = &animation[index];
		anim->currentTick += timeStep*anim->ticksPerSecond;
		if(anim->currentTick>anim->duration)
		{
			anim->currentTick = 0.0;
		}

		//calculate local transform

		//find current position
		glm::mat4 translation = calculatePosition(anim);

		//find current scale
		glm::mat4 scale = calculateScale(anim);

		//find current rotation
		glm::mat4 rotation = calculateRotation(anim);

		//calculate bone transform
		glm::mat4 localTransform = translation*rotation*scale;

		//calculate global transform
		glm::mat4 offset = mBoneOffsets[index];
		glm::mat4 invOffset = glm::inverse(offset);
		glm::mat4 globalTransform = parentTransform*invOffset*localTransform*offset;

		//apply transform
		b->transform = globalTransform;
		mBoneTransforms[index] = b->transform;
	}

	//pass it on
	for (int i = 0 ; i < b->children.size() ; i++)
	{
		Bone *child = b->children[i];
		traverseTreeApplyTransformations(child,animation,timeStep,b->transform);
	}
}

void Mesh::draw(Shader *shader)
{
#pragma region uniforms
	GLuint hasBones = glGetUniformLocation(shader->mProgram,"hasBones");
	glUniform1i(hasBones,mMaterial.hasBones);

	if(mMaterial.hasBones)
	{
		for(int i = 0 ; i < mBoneTransforms.size() ; i++)
		{
			char buffer[80];
			sprintf(buffer,"bones[%d]",i);
			GLuint bones = glGetUniformLocation(shader->mProgram,buffer);
			glUniformMatrix4fv(bones,1,GL_FALSE,glm::value_ptr(mBoneTransforms[i]));
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