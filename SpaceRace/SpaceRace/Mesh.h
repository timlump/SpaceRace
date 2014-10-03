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
	double duration;
	double ticksPerSecond;
	std::vector<aiVector3D> positions;
	std::vector<aiVector3D> scalings;
	std::vector<aiQuaternion> rotations;
};

struct Bone
{
	glm::mat4 transform;
	std::vector<Bone*> children;
	std::string name;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<VertexBone> bones, std::vector<Bone*> boneHierarchy,std::vector<std::map<std::string,BoneAnimation>> animations, Material material);
	void draw(Shader *shader);
	void setup();
	std::vector<Vertex> mVertices;
	std::vector<GLuint> mIndices;
	std::vector<VertexBone> mBones;
	std::vector<Bone*> mBoneHierarchy;
	std::vector<std::map<std::string,BoneAnimation>> mAnimations;
	Material mMaterial;
private:
	GLuint mVAO,mVBO,mEBO,mBBO;
};