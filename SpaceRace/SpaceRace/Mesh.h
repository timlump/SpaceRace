#include "stdafx.h"
#ifndef SHADER_H
#include "Shader.h"
#endif

#define BONES_PER_VERTEX 4

//derived from http://learnopengl.com/#!Model-Loading/Mesh
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Material
{
	GLboolean hasDiffuse;
	GLboolean hasAmbient;
	GLboolean hasSpecular;
	GLboolean hasEmission;
	GLboolean hasShininess;

	GLboolean hasDiffuseTexture;
	GLboolean hasAmbientTexture;
	GLboolean hasSpecularTexture;
	GLboolean hasEmissionTexture;
	GLboolean hasNormalTexture;

	GLboolean hasBones;

	glm::vec4 diffuse;
	glm::vec4 ambient;
	glm::vec4 specular;
	glm::vec4 emission;
	GLfloat shininess;

	GLuint diffuseTexture;
	GLuint ambientTexture;
	GLuint specularTexture;
	GLuint emissionTexture;
	GLuint normalTexture;
};

struct VertexBone
{
	glm::ivec4 boneIDs;
	glm::vec4 boneWeights;
	int numWeights;
};

struct BoneInfo
{
	glm::mat4 boneOffset;
	glm::mat4 worldSpaceTransformation;
	glm::mat4 finalTransformation;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, Material material, const aiScene *scene, aiMesh *mesh, Shader *shader);
	void wipeMesh();
	void draw(Shader *shader);
	void drawBones(Shader *shader);
	void setup(Shader *shader);
	void animate(std::string name,float &time, bool loop=true);
	static glm::mat4 aMat4toGLMMat4(aiMatrix4x4 &matrix);
private:
	void loadBones();
	void readNodes(float time,aiNode *node, aiAnimation *animation, glm::mat4 &parentTransform);
	aiVector3D interpolatePosition(float time, aiNodeAnim* anim);
	aiVector3D interpolateScale(float time, aiNodeAnim* anim);
	aiQuaternion interpolateRotation(float time, aiNodeAnim* anim);
	
	int findPosition(float time, aiNodeAnim *anim);
	int findScale(float time, aiNodeAnim *anim);
	int findRotation(float time, aiNodeAnim *anim);

	std::vector<Vertex> mVertices;
	std::vector<GLuint> mIndices;
	std::vector<VertexBone> mBones;
	std::map<std::string,int> mBoneMapping;
	std::map<std::string,int> mAnimations;
	std::vector<BoneInfo> mBoneInfo;

	std::vector<GLint> mBoneUniformLocs;

	Material mMaterial;
	GLuint mVAO,mVBO,mEBO,mBBO;

	glm::mat4 mGlobalInverseTransform;
	float mCurrentTime;

	const aiScene *mScene;
	aiMesh *mMesh;
};