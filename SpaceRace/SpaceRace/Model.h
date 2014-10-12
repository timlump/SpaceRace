#include "stdafx.h"
#include "Mesh.h"

//derived from http://learnopengl.com/#!Model-Loading/Model
class Model
{
public:
	Model(std::string filename);
	void wipeModel();
	void draw(Shader *shader);
	void animate(std::string name, float &time, bool loop=true);
	static std::map<std::string,GLuint> mTextureIDs;
	static std::map<std::string,Model*> mModels;
	static Model *loadModel(std::string filename);
	std::vector<Mesh> mMeshes;
	glm::vec3 mHalfExtents;
private:
	std::string mDirectory;
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(int index, aiMesh* mesh, const aiScene* scene);
	void processGeometry(aiMesh* mesh, std::vector<Vertex> &vertices, std::vector<GLuint> &indices);
	void processMaterial(aiMesh* mesh, const aiScene* scene, Material &material);
	GLuint loadMaterialTexture(aiMaterial* mat, aiTextureType type,GLboolean &success);

	float mMaxX,mMaxY,mMaxZ,mMinX,mMinY,mMinZ;
	const aiScene *mScene;
	Assimp::Importer *mImport;
};