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
	double currentTick;
	std::vector<aiVectorKey> positions;
	std::vector<aiVectorKey> scalings;
	std::vector<aiQuatKey> rotations;
};

struct Bone
{
	glm::mat4 transform;
	glm::mat4 offset;
	std::vector<Bone*> children;
	std::string name;
	int index;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<VertexBone> bones, std::vector<glm::mat4> boneOffsets ,std::vector<Bone*> boneHierarchy, std::map<std::string,std::map<int,BoneAnimation>> animations, std::vector<glm::mat4> boneTransforms, Material material);
	void draw(Shader *shader);
	void setup();
	void animate(std::string name,double time);
	static glm::mat4 aMat4toGLMMat4(aiMatrix4x4 &matrix);
	static glm::vec3 aVec3toGLMVec3(aiVector3D &vector);
	static glm::quat aQuattoGLMQuat(aiQuaternion &quat);
	static glm::vec3 lerp(float &time, glm::vec3 &start, glm::vec3 &end);
private:
	glm::mat4 calculatePosition(BoneAnimation *anim);
	glm::mat4 calculateScale(BoneAnimation *anim);
	glm::mat4 calculateRotation(BoneAnimation *anim);
	void traverseTreeApplyTransformations(Bone* bone, std::map<int,BoneAnimation> &animation, double timeStep, glm::mat4 &parentTransform);

	std::vector<Vertex> mVertices;
	std::vector<GLuint> mIndices;
	std::vector<VertexBone> mBones;
	std::vector<glm::mat4> mBoneOffsets;
	std::vector<Bone*> mBoneHierarchy;
	std::map<std::string,std::map<int,BoneAnimation>> mAnimations;
	std::vector<glm::mat4> mBoneTransforms;
	Material mMaterial;
	GLuint mVAO,mVBO,mEBO,mBBO;
};