#include "stdafx.h"

#define SHADER_H

class Shader
{
public:
	Shader(std::string vertShader,std::string fragShader);
	GLuint mVertexShader, mFragmentShader, mProgram;
	void bind();
	void unbind();

	static std::map<std::string,GLuint> mVertShaders;
	static std::map<std::string,GLuint> mFragShaders;
	static std::map<std::pair<GLuint,GLuint>,GLuint> mPrograms;
private:
	GLuint createVertexShader(std::string vertShader);
	GLuint createFragmentShader(std::string fragShader);
	GLuint createProgram(GLuint vertID,GLuint fragID);
	std::string loadShaderSource(std::string filename);
};