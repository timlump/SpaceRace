#include "stdafx.h"
struct ShaderInstance
{
	GLuint id;
	int count;
};

class Shader
{
public:
	Shader(std::string vertShader,std::string fragShader);
	~Shader();
	GLuint mVertexShader, mFragmentShader, mProgram;
private:
	GLuint createVertexShader(std::string vertShader);
	GLuint createFragmentShader(std::string fragShader);
	GLuint createProgram(GLuint vertID,GLuint fragID);
	std::string loadShaderSource(std::string filename);

	static std::map<std::string,ShaderInstance> mVertShaders;
	static std::map<std::string,ShaderInstance> mFragShaders;
	static std::map<std::pair<GLuint,GLuint>,ShaderInstance> mPrograms;

	int *mVertCount,*mFragCount, *mProgramCount;
	const std::string *mVertKey,*mFragKey;
	const std::pair<GLuint,GLuint> *mProgramKey;
};