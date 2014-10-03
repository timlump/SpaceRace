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
	void processGeometry(aiMesh* mesh, std::vector<Vertex> &vertices, std::vector<GLuint> &indices);
	void processMaterial(aiMesh* mesh, const aiScene* scene, Material &material);
	void generateVertexBoneMapping(aiMesh* mesh, std::map<GLuint,VertexBone> &boneMapping);
	void generateBoneNameToIndexMapping(aiMesh* mesh, std::map<std::string,int> &boneNameToIndex);
	void traverseAndGenerateBoneHierarchy(aiNode *node, aiMesh *mesh, std::map<std::string,int> &boneIndices, std::vector<Bone*> &bones);
	glm::mat4 aMat4toGLMMat4(aiMatrix4x4 &matrix);
	GLuint loadMaterialTexture(aiMaterial* mat, aiTextureType type,GLboolean &success);
	static std::map<std::string,GLuint> mTextureIDs;
};