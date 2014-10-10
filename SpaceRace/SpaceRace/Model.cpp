#include "stdafx.h"
#include "Model.h"
#include "dirent.h"

std::map<std::string,GLuint> Model::mTextureIDs = std::map<std::string,GLuint>();
std::map<std::string,Model*> Model::mModels = std::map<std::string,Model*>();

Model::Model(std::string filename)
{
	mImport = new Assimp::Importer();
	mScene = mImport->ReadFile(filename,aiProcessPreset_TargetRealtime_Quality);

	if(mScene->HasMeshes())
	{
		printf("Model has %d mesh(es)\n",mScene->mNumMeshes);
	}

	if(mScene->HasAnimations())
	{
		printf("Model has %d animation(s)\n",mScene->mNumAnimations);
		for(int i = 0 ; i < mScene->mNumAnimations ; i++)
		{
			aiAnimation *anim = mScene->mAnimations[i];
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
	processNode(mScene->mRootNode,mScene);

	mModels[filename] = this;
}

Model* Model::loadModel(std::string filename)
{
	if(mModels.find(filename)!=mModels.end())
	{
		return mModels[filename];
	}
	else
	{
		return new Model(filename);
	}
}

void Model::wipeModel()
{
	for(int i = 0 ; i < mMeshes.size() ; i++)
	{
		mMeshes[i].wipeMesh();
	}
	delete mImport;
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

void Model::animate(std::string name, float &time, bool loop)
{
	for(int i = 0 ; i < mMeshes.size() ; i++)
	{
		mMeshes[i].animate(name,time,loop);
	}
}

Mesh Model::processMesh(int index, aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	Material material;

	processGeometry(mesh,vertices,indices);
	processMaterial(mesh,scene,material);

	return Mesh(vertices,indices,material,scene,mesh);
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
