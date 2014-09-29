#include "stdafx.h"
#include "Model.h"

std::map<std::string,GLuint> Model::mTextureIDs = std::map<std::string,GLuint>();

Model::Model(std::string filename)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(filename,aiProcessPreset_TargetRealtime_Fast);
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
		Mesh processedMesh = processMesh(mesh,scene);
		mMeshes.push_back(processedMesh);
	}

	for(int i = 0 ; i < node->mNumChildren ; i++)
	{
		processNode(node->mChildren[i],scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	Material material;

	//vertices
	for(int i = 0 ; i < mesh->mNumFaces ; i++)
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
			material.specular.r = diffuse.r;
			material.specular.g = diffuse.g;
			material.specular.b = diffuse.b;
			material.specular.a = diffuse.a;
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

	return Mesh(vertices,indices,material);
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
		std::string filename = mDirectory + "//" + name;
		std::wstring wFilename(filename.begin(),filename.end());

		//see if the texture has already been loaded
		auto id = mTextureIDs.find(filename);
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
			ilLoadImage(wFilename.c_str());
			ilConvertImage(IL_RGBA,IL_UNSIGNED_BYTE);

			GLuint glID;
			glGenTextures(1,&glID);
			glBindTexture(GL_TEXTURE_2D,glID);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT),0,GL_RGBA,GL_UNSIGNED_BYTE,ilGetData());
			ilDeleteImages(1,&devilID);

			mTextureIDs[filename] = glID;

			tex =  glID;
		}
		success = GL_TRUE;
	}

	return tex;
}