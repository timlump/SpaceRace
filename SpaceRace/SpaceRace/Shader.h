#include "stdafx.h"

class Shader
{
public:
	Shader(std::string vertShader,std::string fragShader);
	GLuint mVertexShader, mFragmentShader, mProgram;
private:
	GLuint createVertexShader(std::string vertShader);
	GLuint createFragmentShader(std::string fragShader);
	GLuint createProgram(GLuint vertID,GLuint fragID);
	std::string loadShaderSource(std::string filename);

	static std::map<std::string,GLuint> mVertShaders;
	static std::map<std::string,GLuint> mFragShaders;
	static std::map<std::pair<GLuint,GLuint>,GLuint> mPrograms;
};