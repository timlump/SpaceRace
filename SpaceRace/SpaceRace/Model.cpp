#include "stdafx.h"
#include "Model.h"
#include "dirent.h"

std::map<std::string,GLuint> Model::mTextureIDs = std::map<std::string,GLuint>();

Model::Model(std::string filename)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(filename,aiProcessPreset_TargetRealtime_Quality|aiProcess_FlipUVs);

	if(scene->HasMeshes())
	{
		printf("Model has %d mesh(es)\n",scene->mNumMeshes);
	}

	if(scene->HasAnimations())
	{
		printf("Model has %d animation(s)\n",scene->mNumAnimations);
		for(int i = 0 ; i < scene->mNumAnimations ; i++)
		{
			aiAnimation *anim = scene->mAnimations[i];
			std::string name(anim->mName.C_Str());
			if(name.empty())
			{
				name = "No Name";
			}
			double duration = anim->mDuration;
			double ticks = anim->mTicksPerSecond;
			printf("Name: %s Duration: %.2f TicksPerSecond: %.2f\n",name.c_str(),duration,ticks);
		}
	}

	mDirectory = filename.substr(0,filename.find_last_of('/'));
	processNode(scene->mRootNode,scene);
}

void Model::draw(Shader *shader)
{
	for(int i = 0 ; i < mMeshes.size() ; i++)
	{
		mMeshes[i].draw(shader);
	}
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for(int i = 0 ; i < node->mNumMeshes ; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

		if(mesh->HasBones())
		{
			printf("Mesh: %d has %d bone(s)\n",i,mesh->mNumBones);
		}

		Mesh processedMesh = processMesh(i,mesh,scene);
		mMeshes.push_back(processedMesh);
	}

	for(int i = 0 ; i < node->mNumChildren ; i++)
	{
		processNode(node->mChildren[i],scene);
	}
}

void Model::processGeometry(aiMesh* mesh, std::vector<Vertex> &vertices, std::vector<GLuint> &indices)
{
	//vertices
	for(int i = 0 ; i < mesh->mNumVertices ; i++)
	{
		Vertex vertex;
		//positions
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		//normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		//uvs
		if(mesh->mTextureCoords[0])
		{
			glm::vec2 uv;
			uv.x = mesh->mTextureCoords[0][i].x;
			uv.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = uv;
		}
		else
		{
			vertex.uv = glm::vec2(0.0f,0.0f);
		}

		vertices.push_back(vertex);
	}

	//indices
	for(int i = 0 ; i < mesh->mNumFaces ; i++)
	{
		aiFace face = mesh->mFaces[i];
		for(int j = 0 ; j < face.mNumIndices ; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
}

void Model::processMaterial(aiMesh* mesh, const aiScene* scene, Material &material)
{
	if(mesh->HasBones())
	{
		material.hasBones = GL_TRUE;
	}
	else
	{
		material.hasBones = GL_FALSE;
	}
	//materials
	if(mesh->mMaterialIndex >= 0)
	{
		aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
		//material properties
		//diffuse
		aiColor4D diffuse;
		if(AI_SUCCESS == aiGetMaterialColor(mat,AI_MATKEY_COLOR_DIFFUSE,&diffuse))
		{
			material.hasDiffuse = GL_TRUE;
			material.diffuse.r = diffuse.r;
			material.diffuse.g = diffuse.g;
			material.diffuse.b = diffuse.b;
			material.diffuse.a = diffuse.a;
		}
		else
		{
			material.hasDiffuse = GL_FALSE;
			material.diffuse = glm::vec4(0.0f);
		}
		//specular
		aiColor4D specular;
		if(AI_SUCCESS == aiGetMaterialColor(mat,AI_MATKEY_COLOR_SPECULAR,&specular))
		{
			material.hasSpecular = GL_TRUE;
			material.specular.r = specular.r;
			material.specular.g = specular.g;
			material.specular.b = specular.b;
			material.specular.a = specular.a;
		}
		else
		{
			material.hasSpecular = GL_FALSE;
			material.specular = glm::vec4(0.0f);
		}
		//ambient
		aiColor4D ambient;
		if(AI_SUCCESS == aiGetMaterialColor(mat,AI_MATKEY_COLOR_AMBIENT,&ambient))
		{
			material.hasAmbient = GL_TRUE;
			material.ambient.r = ambient.r;
			material.ambient.g = ambient.g;
			material.ambient.b = ambient.b;
			material.ambient.a = ambient.a;
		}
		else
		{
			material.hasAmbient = GL_FALSE;
			material.ambient = glm::vec4(0.0f);
		}
		//emission
		aiColor4D emission;
		if(AI_SUCCESS == aiGetMaterialColor(mat,AI_MATKEY_COLOR_EMISSIVE,&emission))
		{
			material.hasEmission = GL_TRUE;
			material.emission.r = emission.r;
			material.emission.g = emission.g;
			material.emission.b = emission.b;
			material.emission.a = emission.a;
		}
		else
		{
			material.hasEmission = GL_FALSE;
			material.emission = glm::vec4(0.0f);
		}
		//shininess
		GLfloat shininess;
		GLuint max;
		if(AI_SUCCESS == aiGetMaterialFloatArray(mat,AI_MATKEY_SHININESS,&shininess,&max))
		{
			material.hasShininess = GL_TRUE;
			material.shininess = shininess;
		}
		else
		{
			material.hasShininess = GL_FALSE;
			material.shininess = 0.0f;
		}
		//textures
		GLboolean success;
		material.diffuseTexture = loadMaterialTexture(mat,aiTextureType_DIFFUSE,success);
		material.hasDiffuseTexture = success;

		material.specularTexture = loadMaterialTexture(mat,aiTextureType_SPECULAR,success);
		material.hasSpecularTexture = success;

		material.ambientTexture = loadMaterialTexture(mat,aiTextureType_AMBIENT,success);
		material.hasAmbientTexture = success;

		material.emissionTexture = loadMaterialTexture(mat,aiTextureType_EMISSIVE,success);
		material.hasEmissionTexture = success;

		material.normalTexture = loadMaterialTexture(mat,aiTextureType_NORMALS,success);
		material.hasNormalTexture = success;

	}
	else
	{
		material.hasDiffuse = GL_FALSE;
		material.hasSpecular = GL_FALSE;
		material.hasAmbient = GL_FALSE;
		material.hasEmission = GL_FALSE;
		material.hasShininess = GL_FALSE;

		material.hasDiffuseTexture = GL_FALSE;
		material.hasSpecularTexture = GL_FALSE;
		material.hasAmbientTexture = GL_FALSE;
		material.hasEmissionTexture = GL_FALSE;

		material.diffuse = glm::vec4(0.0f);
		material.specular = glm::vec4(0.0f);
		material.ambient = glm::vec4(0.0f);
		material.emission = glm::vec4(0.0f);
		material.shininess = 0.0f;
	}
}

void Model::generateVertexBoneMapping(aiMesh* mesh, std::map<GLuint,VertexBone> &boneMapping)
{
	//fill mapping so that every vertex is present
	for(int i = 0 ; i < mesh->mNumVertices ; i++)
	{
		VertexBone entry;
		entry.numWeights = 0;
		entry.boneWeights = glm::vec4(0.0f);
		entry.boneIDs = glm::ivec4(0);
		boneMapping[i] = entry;
	}

	for(int i = 0 ; i < mesh->mNumBones ; i++)
	{
		aiBone *bone = mesh->mBones[i];
		std::string boneName(bone->mName.data);

		for(int j = 0 ; j < bone->mNumWeights ; j++)
		{
			aiVertexWeight *weight = &bone->mWeights[j];
			GLuint vertexID = weight->mVertexId;
			//see if the vertex doesn't exist
			auto boneEntry = boneMapping.find(vertexID);
			if(boneEntry->second.numWeights+1 < BONES_PER_VERTEX)
			{
				int index = boneEntry->second.numWeights;
				boneEntry->second.numWeights++;
				boneEntry->second.boneIDs[index] = i;
				boneEntry->second.boneWeights[index] = weight->mWeight;
			}
		}
	}
}

void Model::generateBoneNameToIndexMapping(aiMesh* mesh, std::map<std::string,int> &boneNameToIndex)
{
	for(int i = 0 ; i < mesh->mNumBones ; i++)
	{
		aiBone *bone = mesh->mBones[i];

		std::string boneName(bone->mName.data);
		boneNameToIndex[boneName] = i;
	}
}

//inspired by http://pastebin.com/YaTamEct
void Model::traverseAndGenerateBoneHierarchy(aiNode *node, aiMesh *mesh, std::map<std::string,int> &boneIndices, std::vector<Bone*> &bones)
{
	//check if current node is a bone
	std::string name(node->mName.data);
	auto boneEntry = boneIndices.find(name);
	Bone *bone = new Bone();
	if(boneEntry!=boneIndices.end())
	{
		aiBone *b = mesh->mBones[boneEntry->second];

		bone->name = name;
		bone->index = boneEntry->second;
		bone->offset = Mesh::aMat4toGLMMat4(b->mOffsetMatrix);
		bone->transform = glm::mat4(1.0);
		bones.push_back(bone);

		printf("Bone: %d Name: %s\n",bone->index,bone->name.c_str());
	}
	else
	{
		bone->name = name;
		bone->index = -1;
		bone->offset = glm::mat4(1.0);
		bone->transform = Mesh::aMat4toGLMMat4(node->mTransformation);
		bones.push_back(bone);
	}
	for(int i = 0 ; i < node->mNumChildren ; i++)
	{
		traverseAndGenerateBoneHierarchy(node->mChildren[i],mesh,boneIndices,bone->children);
	}
}

void Model::processAnimations(const aiScene* scene, std::map<std::string,int> &boneIndices, std::map<std::string,std::map<int,BoneAnimation>> &animations)
{
	for(int i = 0 ; i < scene->mNumAnimations ; i++)
	{
		aiAnimation *anim = scene->mAnimations[i];
		std::string animName(anim->mName.data);
		std::map<int,BoneAnimation> animationMap;
		for(int j = 0 ; j < anim->mNumChannels ; j++)
		{
			aiNodeAnim *node = anim->mChannels[j];
			//check to see if it applies to a bone
			std::string name(node->mNodeName.data);
			auto boneEntry = boneIndices.find(name);
			if(boneEntry != boneIndices.end())
			{
				BoneAnimation animation;
				animation.name = name;
				animation.index = boneEntry->second;
				animation.duration = anim->mDuration;
				animation.ticksPerSecond = anim->mTicksPerSecond;
				animation.currentTick = 0.0;
				for(int k = 0 ; k < node->mNumPositionKeys ; k++)
				{
					aiVectorKey key = node->mPositionKeys[k];
					animation.positions.push_back(key);
				}
				for(int k = 0 ; k < node->mNumRotationKeys ; k++)
				{
					aiQuatKey key = node->mRotationKeys[k];
					animation.rotations.push_back(key);
				}
				for(int k = 0 ; k < node->mNumScalingKeys ; k++)
				{
					aiVectorKey key = node->mScalingKeys[k];
					animation.scalings.push_back(key);
				}
				animationMap[animation.index] = animation;
			}
		}
		animations[animName] = animationMap;
	}
}

void Model::animate(std::string name, double timeStep)
{
	for(int i = 0 ; i < mMeshes.size() ; i++)
	{
		mMeshes[i].animate(name,timeStep);
	}
}

Mesh Model::processMesh(int index, aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<VertexBone> bones;
	std::vector<Bone*> boneHierarchy;
	std::map<GLuint,VertexBone> boneMapping;
	std::map<std::string,int> boneNameToIndex;
	std::vector<glm::mat4> boneTransforms;
	std::vector<glm::mat4> boneOffsets;

	Material material;
	std::map<std::string,std::map<int,BoneAnimation>> animations; 

	processGeometry(mesh,vertices,indices);
	processMaterial(mesh,scene,material);
	generateVertexBoneMapping(mesh,boneMapping);
	generateBoneNameToIndexMapping(mesh,boneNameToIndex);

	//convert mapping to vector - taking advantage of the fact that a map is implicitly sorted
	std::map<GLuint,VertexBone>::iterator it;
	for(it = boneMapping.begin(); it != boneMapping.end() ; ++it)
	{
		bones.push_back(it->second);
	}

	for(int i = 0 ; i < mesh->mNumBones ; i++)
	{
		boneTransforms.push_back(glm::mat4(1.0f));
		boneOffsets.push_back(Mesh::aMat4toGLMMat4(mesh->mBones[i]->mOffsetMatrix));
	}

	traverseAndGenerateBoneHierarchy(scene->mRootNode,mesh,boneNameToIndex,boneHierarchy);
	processAnimations(scene,boneNameToIndex,animations);

	glm::mat4 globalInverseTransform = Mesh::aMat4toGLMMat4(scene->mRootNode->mTransformation);
	globalInverseTransform = glm::inverse(globalInverseTransform);

	return Mesh(vertices,indices,bones,boneOffsets,boneHierarchy,animations,boneTransforms,material,globalInverseTransform);
}

GLuint Model::loadMaterialTexture(aiMaterial* mat, aiTextureType type, GLboolean &success)
{
	success = GL_FALSE;
	GLuint tex = -1;
	if( mat->GetTextureCount(type) > 0)
	{
		aiString aName;
		mat->GetTexture(type,0,&aName);
		std::string name(aName.C_Str());

		//see if the texture has already been loaded
		auto id = mTextureIDs.find(name);
		if(id != mTextureIDs.end())
		{
			tex = id->second;
		}
		else
		{
			//we need to load the texture
			ILuint devilID;
			ilGenImages(1,&devilID);
			ilBindImage(devilID);
			std::string fileName = mDirectory + "/" + name;
			ILboolean success = ilLoadImage((const ILstring)fileName.c_str());

			if(success == IL_FALSE)
			{
				printf("Failed to load texture: %s\n", name.c_str());
			}

			ilConvertImage(IL_RGBA,IL_UNSIGNED_BYTE);

			GLuint glID;
			glGenTextures(1,&glID);
			glBindTexture(GL_TEXTURE_2D,glID);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

			ILubyte *data = ilGetData();
			ILuint width = ilGetInteger(IL_IMAGE_WIDTH);
			ILuint height = ilGetInteger(IL_IMAGE_HEIGHT);

			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
			ilDeleteImages(1,&devilID);

			mTextureIDs[name] = glID;

			tex =  glID;
		}
		success = GL_TRUE;
	}

	return tex;
}
