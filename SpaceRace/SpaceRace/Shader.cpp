#include "stdafx.h"
#include "Shader.h"

Shader::Shader(std::string vertShader,std::string fragShader)
{
	mVertexShader = createVertexShader(vertShader);
	mFragmentShader = createFragmentShader(fragShader);
	mProgram = createProgram(mVertexShader,mFragmentShader);
}

Shader::~Shader()
{

}

GLuint Shader::createVertexShader(std::string vertShader)
{
	auto id = mVertShaders.find(vertShader);
	if(id != mVertShaders.end())
	{
		id->second.count++;
		return id->second.id;
	}
	else
	{
		ShaderInstance inst;
		inst.count = 1;
		inst.id = glCreateShader(GL_VERTEX_SHADER);
		std::string source = loadShaderSource(vertShader);
		glShaderSource(inst.id,1,(const char **)source.c_str(),NULL);
		glCompileShader(inst.id);

		mVertShaders[vertShader] = inst;

		GLint status;
		glGetShaderiv(inst.id,GL_COMPILE_STATUS,&status);
		if(status)
		{
			char buffer[512];
			glGetShaderInfoLog(inst.id,512,NULL,buffer);
		}

		return inst.id;
	}
}

GLuint Shader::createFragmentShader(std::string fragShader)
{
	auto id = mVertShaders.find(fragShader);
	if(id != mVertShaders.end())
	{
		id->second.count++;
		return id->second.id;
	}
	else
	{
		ShaderInstance inst;
		inst.count = 1;
		inst.id = glCreateShader(GL_FRAGMENT_SHADER);
		std::string source = loadShaderSource(fragShader);
		glShaderSource(inst.id,1,(const char **)source.c_str(),NULL);
		glCompileShader(inst.id);

		mFragShaders[fragShader] = inst;

		GLint status;
		glGetShaderiv(inst.id,GL_COMPILE_STATUS,&status);
		if(status)
		{
			char buffer[512];
			glGetShaderInfoLog(inst.id,512,NULL,buffer);
		}

		return inst.id;
	}
}

GLuint Shader::createProgram(GLuint vertID,GLuint fragID)
{
	//see if the program already exists
	bool found = false;
	for(std::map<std::pair<GLuint,GLuint>,ShaderInstance>::iterator it = mPrograms.begin() ; it != mPrograms.end() ; ++it)
	{
		std::pair<GLuint,GLuint> ids = it->first;
		if(ids.first == vertID && ids.second == fragID)
		{
			found = true;
			it->second.count++;
			return it->second.id;
		}
	}

	ShaderInstance inst;
	inst.count = 1;
	inst.id = glCreateProgram();
	glAttachShader(inst.id,vertID);
	glAttachShader(inst.id,fragID);
	glLinkProgram(inst.id);
	glUseProgram(inst.id);
}

std::string Shader::loadShaderSource(std::string filename)
{

}