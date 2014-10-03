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

struct BoneAnimation
{
	std::string name;
	int index;
	double duration;
	double ticksPerSecond;
	std::vector<aiVectorKey> positions;
	std::vector<aiVectorKey> scalings;
	std::vector<aiQuatKey> rotations;
};

struct Bone
{
	glm::mat4 transform;
	std::vector<Bone*> children;
	std::string name;
	int index;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<VertexBone> bones, std::vector<Bone*> boneHierarchy, std::map<std::string,std::map<std::string,BoneAnimation>> animations, std::vector<glm::mat4> boneTransforms, Material material);
	void draw(Shader *shader);
	void setup();
	void animate(std::string name,double time);
	std::vector<Vertex> mVertices;
	std::vector<GLuint> mIndices;
	std::vector<VertexBone> mBones;
	std::vector<Bone*> mBoneHierarchy;
	std::map<std::string,std::map<std::string,BoneAnimation>> mAnimations;
	std::vector<glm::mat4> mBoneTransforms;
	Material mMaterial;
private:
	void traverseTreeApplyTransformations(std::vector<Bone*> bone, std::map<std::string,BoneAnimation> &animation, double time, glm::mat4 &parentTransform);
	GLuint mVAO,mVBO,mEBO,mBBO;
};