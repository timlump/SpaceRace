#include "stdafx.h"
#include "Mesh.h"

//derived from http://learnopengl.com/#!Model-Loading/Model
class Model
{
public:
	Model(std::string filename);
	void wipeModel();
	void draw(Shader *shader);
	std::vector<Mesh> mMeshes;
	void animate(std::string name, double timeStep);
	static std::map<std::string,GLuint> mTextureIDs;
private:
	std::string mDirectory;
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(int index, aiMesh* mesh, const aiScene* scene);
	void processGeometry(aiMesh* mesh, std::vector<Vertex> &vertices, std::vector<GLuint> &indices);
	void processMaterial(aiMesh* mesh, const aiScene* scene, Material &material);
	GLuint loadMaterialTexture(aiMaterial* mat, aiTextureType type,GLboolean &success);

	const aiScene *mScene;
	Assimp::Importer *mImport;
};