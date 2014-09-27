#include "stdafx.h"
#include "Shader.h"

//derived from http://learnopengl.com/#!Model-Loading/Mesh

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Texture
{
	GLuint id;
	std::string type;
};

struct Material
{
	bool hasDiffuse;
	bool hasAmbient;
	bool hasSpecular;
	bool hasEmission;
	bool hasShininess;
	glm::vec4 diffuse;
	glm::vec4 ambient;
	glm::vec4 specular;
	glm::vec4 emission;
	float shininess;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, Material material);
	~Mesh();
	void draw(Shader shader);
	std::vector<Vertex> mVertices;
	std::vector<GLuint> mIndices;
	std::vector<Texture> mTextures;
	Material mMaterial;
private:
	void setup();
	GLuint mVAO,mVBO,mEBO;
};