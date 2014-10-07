#include "stdafx.h"
#include "Shader.h"

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
	aiMatrix4x4 boneOffset;
	aiMatrix4x4 worldSpaceTransformation;
	aiMatrix4x4 finalTransformation;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, Material material, const aiScene *scene, aiMesh *mesh);
	void draw(Shader *shader);
	void drawBones(Shader *shader);
	void setup();
	void animate(std::string name,double time);
	static glm::mat4 aMat4toGLMMat4(aiMatrix4x4 &matrix);
	static glm::vec3 aVec3toGLMVec3(aiVector3D &vector);
	static glm::quat aQuattoGLMQuat(aiQuaternion &quat);
	static glm::vec3 lerp(float &time, glm::vec3 &start, glm::vec3 &end);
private:
	void loadBones();
	void readNodes(float time,aiNode *node, aiAnimation *animation, aiMatrix4x4 &parentTransform);

	std::vector<Vertex> mVertices;
	std::vector<GLuint> mIndices;
	std::vector<VertexBone> mBones;
	std::map<std::string,int> mBoneMapping;
	std::map<std::string,int> mAnimations;
	std::vector<BoneInfo> mBoneInfo;

	Material mMaterial;
	GLuint mVAO,mVBO,mEBO,mBBO;

	aiMatrix4x4 mGlobalInverseTransform;
	float mCurrentTime;

	const aiScene *mScene;
	aiMesh *mMesh;
};