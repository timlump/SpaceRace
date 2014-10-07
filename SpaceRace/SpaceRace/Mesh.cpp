#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, Material material, const aiScene *scene, aiMesh *mesh)
{
	mVertices = vertices;
	mIndices = indices;
	mGlobalInverseTransform = aMat4toGLMMat4(scene->mRootNode->mTransformation);
	mGlobalInverseTransform = glm::inverse(mGlobalInverseTransform);
	mScene = scene;
	mMesh = mesh;
	mCurrentTime = 0.0f;
	mMaterial = material;
	setup();
}

void Mesh::animate(std::string name,double time)
{
	auto animation = mAnimations.find(name);
	if(animation != mAnimations.end())
	{
		aiAnimation *anim = mScene->mAnimations[animation->second];

		mCurrentTime += time*anim->mTicksPerSecond;
		if(mCurrentTime>anim->mDuration)
		{
			mCurrentTime = 0.0f;
		}
		readNodes(mCurrentTime,mScene->mRootNode,anim,glm::mat4(1.0f));
	}
}

void Mesh::readNodes(float time,aiNode *node, aiAnimation *animation, glm::mat4 &parentTransform)
{
	std::string nodeName(node->mName.data);
	glm::mat4 nodeTransform = aMat4toGLMMat4(node->mTransformation);

	aiNodeAnim *nodeAnim = NULL;
	for(int i = 0 ; i < animation->mNumChannels ; i++)
	{
		aiNodeAnim *a = animation->mChannels[i];
		std::string n(a->mNodeName.data);
		if(n.compare(nodeName)==0)
		{
			nodeAnim = a;
			break;
		}
	}

	if(nodeAnim)
	{
		aiVector3D scaling = interpolateScale(time,nodeAnim);
		glm::mat4 scaleMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::scale(scaleMatrix,glm::vec3(scaling.x,scaling.y,scaling.z));

		aiQuaternion rotation = interpolateRotation(time,nodeAnim);
		glm::quat rotQuat;
		rotQuat.w = rotation.w;
		rotQuat.x = rotation.x;
		rotQuat.y = rotation.y;
		rotQuat.z = rotation.z;
		glm::mat4 rotMatrix = glm::mat4_cast(rotQuat);


		aiVector3D translation = interpolatePosition(time,nodeAnim);
		glm::mat4 transMatrix = glm::mat4(1.0f);
		transMatrix = glm::translate(transMatrix,glm::vec3(translation.x,translation.y,translation.z));

		nodeTransform = transMatrix*rotMatrix*scaleMatrix;
	}

	glm::mat4 globalTransformation = parentTransform*nodeTransform;

	if(mBoneMapping.find(nodeName) != mBoneMapping.end())
	{
		int index = mBoneMapping[nodeName];
		mBoneInfo[index].worldSpaceTransformation = mGlobalInverseTransform*globalTransformation;
		mBoneInfo[index].finalTransformation = mGlobalInverseTransform*globalTransformation*mBoneInfo[index].boneOffset;
	}

	for(int i = 0 ; i < node->mNumChildren ; i++)
	{
		readNodes(time,node->mChildren[i],animation,globalTransformation);
	}
}

glm::mat4 Mesh::aMat4toGLMMat4(aiMatrix4x4 &matrix)
{
	glm::mat4 result;
	aiMatrix4x4 matT = matrix;
	matT = matT.Transpose();
	for(int i = 0 ; i < 4 ; i++)
	{
		for(int j = 0 ; j < 4 ; j++)
		{
			result[i][j] = matT[i][j];
		}
	}
	return result;
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

			glm::mat4 transform = mBoneInfo[i].finalTransformation;

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
	for(int i = 0 ; i < mMesh->mNumBones ; i++)
	{
		int boneIndex = 0;
		aiBone *bone = mMesh->mBones[i];
		std::string boneName(bone->mName.data);

		if(mBoneMapping.find(boneName) == mBoneMapping.end())
		{
			boneIndex = mBoneInfo.size();
			BoneInfo info;
			mBoneInfo.push_back(info);

			mBoneInfo[boneIndex].boneOffset = aMat4toGLMMat4(bone->mOffsetMatrix);
			mBoneMapping[boneName] = boneIndex;
		}
		else
		{
			boneIndex = mBoneMapping[boneName];
		}

		//map vertices to bones
		//pump these out to bones vector
		for(int j = 0 ; j < mMesh->mNumVertices ; j++)
		{
			VertexBone b;
			b.boneWeights = glm::vec4(0.0f);
			b.boneIDs = glm::ivec4(0.0f);
			b.numWeights = 0;
			mBones.push_back(b);
		}

		for(int j = 0 ; j < bone->mNumWeights ; j++)
		{
			aiVertexWeight weight = bone->mWeights[j];
			
			int index = weight.mVertexId;
			int wI = mBones[index].numWeights;

			if(wI < 4)
			{
				mBones[index].boneIDs[wI] = boneIndex;
				mBones[index].boneWeights[wI] = weight.mWeight;
				mBones[index].numWeights++;
			}
		}

	}
}

void Mesh::setup()
{
	loadBones();

	for(int i = 0 ; i < mScene->mNumAnimations ; i++)
	{
		aiAnimation *anim = mScene->mAnimations[i];
		std::string animName(anim->mName.data);
		mAnimations[animName] = i;
	}

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

aiVector3D Mesh::interpolatePosition(float time, aiNodeAnim* anim)
{
	if(anim->mNumPositionKeys == 1)
	{
		return anim->mPositionKeys[0].mValue;
	}

	int index = findPosition(time,anim);
	int nextIndex = index+1;

	 float delta = (float)(anim->mPositionKeys[nextIndex].mTime - anim->mPositionKeys[index].mTime);
	 float factor = (time - (float)anim->mPositionKeys[index].mTime) / delta;
	 aiVector3D start = anim->mPositionKeys[index].mValue;
	 aiVector3D end = anim->mPositionKeys[nextIndex].mValue;
	 return start + factor*(end-start);
}

aiVector3D Mesh::interpolateScale(float time, aiNodeAnim* anim)
{
	if(anim->mNumScalingKeys == 1)
	{
		return anim->mScalingKeys[0].mValue;
	}

	int index = findScale(time,anim);
	int nextIndex = index+1;

	 float delta = (float)(anim->mScalingKeys[nextIndex].mTime - anim->mScalingKeys[index].mTime);
	 float factor = (time - (float)anim->mScalingKeys[index].mTime) / delta;
	 aiVector3D start = anim->mScalingKeys[index].mValue;
	 aiVector3D end = anim->mScalingKeys[nextIndex].mValue;
	 return start + factor*(end-start);
}

aiQuaternion Mesh::interpolateRotation(float time, aiNodeAnim* anim)
{
	if(anim->mNumRotationKeys == 1)
	{
		return anim->mRotationKeys[0].mValue;
	}

	int index = findRotation(time,anim);
	int nextIndex = index+1;

	 float delta = (float)(anim->mRotationKeys[nextIndex].mTime - anim->mRotationKeys[index].mTime);
	 float factor = (time - (float)anim->mRotationKeys[index].mTime) / delta;
	 aiQuaternion start = anim->mRotationKeys[index].mValue;
	 aiQuaternion end = anim->mRotationKeys[nextIndex].mValue;
	 aiQuaternion output;
	 aiQuaternion::Interpolate(output,start,end,factor);
	 return output.Normalize();
}
	
int Mesh::findPosition(float time, aiNodeAnim *anim)
{
	for(int i = 0 ; anim->mNumPositionKeys-1 ; i++)
	{
		if(time < (float)anim->mPositionKeys[i+1].mTime)
		{
			return i;
		}
	}
	return 0;
}
int Mesh::findScale(float time, aiNodeAnim *anim)
{
	for(int i = 0 ; anim->mNumScalingKeys-1 ; i++)
	{
		if(time < (float)anim->mScalingKeys[i+1].mTime)
		{
			return i;
		}
	}
	return 0;
}
int Mesh::findRotation(float time, aiNodeAnim *anim)
{
	for(int i = 0 ; anim->mNumRotationKeys-1 ; i++)
	{
		if(time < (float)anim->mRotationKeys[i+1].mTime)
		{
			return i;
		}
	}
	return 0;
}