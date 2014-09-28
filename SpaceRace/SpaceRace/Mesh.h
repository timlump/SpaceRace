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
	GLboolean hasDiffuse;
	GLboolean hasAmbient;
	GLboolean hasSpecular;
	GLboolean hasEmission;
	GLboolean hasShininess;
	GLboolean hasTextures;
	glm::vec4 diffuse;
	glm::vec4 ambient;
	glm::vec4 specular;
	glm::vec4 emission;
	GLfloat shininess;
	std::vector<Texture> textures;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, Material material);
	void draw(Shader *shader);
	void setup();
	std::vector<Vertex> mVertices;
	std::vector<GLuint> mIndices;
	Material mMaterial;
private:
	GLuint mVAO,mVBO,mEBO;
};