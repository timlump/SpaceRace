#include "stdafx.h"
#include "Mesh.h"

//derived from http://learnopengl.com/#!Model-Loading/Model
class Model
{
public:
	Model(std::string filename);
	void draw(Shader *shader);
	std::vector<Mesh> mMeshes;
private:
	std::string mDirectory;
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(int index, aiMesh* mesh, const aiScene* scene);
	GLuint loadMaterialTexture(aiMaterial* mat, aiTextureType type,GLboolean &success);
	static std::map<std::string,GLuint> mTextureIDs;
};