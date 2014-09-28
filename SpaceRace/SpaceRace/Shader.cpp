#include "stdafx.h"
#include "Shader.h"

std::map<std::string,ShaderInstance> Shader::mVertShaders = std::map<std::string,ShaderInstance>();
std::map<std::string,ShaderInstance> Shader::mFragShaders = std::map<std::string,ShaderInstance>();
std::map<std::pair<GLuint,GLuint>,ShaderInstance> Shader::mPrograms = std::map<std::pair<GLuint,GLuint>,ShaderInstance>();

Shader::Shader(std::string vertShader,std::string fragShader)
{
	mVertexShader = createVertexShader(vertShader);
	mFragmentShader = createFragmentShader(fragShader);
	mProgram = createProgram(mVertexShader,mFragmentShader);
}

Shader::~Shader()
{
	*mVertCount--;
	*mFragCount--;
	*mProgramCount--;

	if(*mProgramCount == 0)
	{
		glDeleteProgram(mProgram);
		std::map<std::pair<GLuint,GLuint>,ShaderInstance>::iterator it = mPrograms.find(*mProgramKey);
		mPrograms.erase(it);
	}

	if(*mVertCount == 0)
	{
		glDeleteShader(mVertexShader);
		std::map<std::string,ShaderInstance>::iterator it = mVertShaders.find(*mVertKey);
		mVertShaders.erase(it);
	}

	if(*mFragCount == 0)
	{
		glDeleteShader(mFragmentShader);
		std::map<std::string,ShaderInstance>::iterator it = mFragShaders.find(*mFragKey);
		mFragShaders.erase(it);
	}
}

GLuint Shader::createVertexShader(std::string vertShader)
{
	auto id = mVertShaders.find(vertShader);
	if(id != mVertShaders.end())
	{
		id->second.count++;
		mVertCount = &id->second.count;
		mVertKey = &id->first;
		return id->second.id;
	}
	else
	{
		ShaderInstance inst;
		inst.count = 1;
		mVertCount = &inst.count;
		inst.id = glCreateShader(GL_VERTEX_SHADER);
		std::string source = loadShaderSource(vertShader);
		glShaderSource(inst.id,1,(const char **)source.c_str(),NULL);
		glCompileShader(inst.id);

		mVertShaders[vertShader] = inst;
		mVertKey = &vertShader;

		GLint status;
		glGetShaderiv(inst.id,GL_COMPILE_STATUS,&status);
		if(status)
		{
			char buffer[512];
			glGetShaderInfoLog(inst.id,512,NULL,buffer);
			printf("%s\n",buffer);
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
		mFragCount = &id->second.count;
		mFragKey = &id->first;
		return id->second.id;
	}
	else
	{
		ShaderInstance inst;
		inst.count = 1;
		mFragCount = &inst.count;
		inst.id = glCreateShader(GL_FRAGMENT_SHADER);
		std::string source = loadShaderSource(fragShader);
		glShaderSource(inst.id,1,(const char **)source.c_str(),NULL);
		glCompileShader(inst.id);

		mFragShaders[fragShader] = inst;
		mVertKey = &fragShader;

		GLint status;
		glGetShaderiv(inst.id,GL_COMPILE_STATUS,&status);
		if(status)
		{
			char buffer[512];
			glGetShaderInfoLog(inst.id,512,NULL,buffer);
			printf("%s\n",buffer);
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
			mProgramCount = &it->second.count;
			mProgramKey = &it->first;
			return it->second.id;
		}
	}

	ShaderInstance inst;
	inst.count = 1;
	mProgramCount = &inst.count;
	inst.id = glCreateProgram();
	glAttachShader(inst.id,vertID);
	glAttachShader(inst.id,fragID);
	glLinkProgram(inst.id);
	glUseProgram(inst.id);

	std::pair<GLuint,GLuint> key = std::pair<GLuint,GLuint>(vertID,fragID);
	mPrograms[key] = inst;
	mProgramKey = &key;

	return inst.id;
}

std::string Shader::loadShaderSource(std::string filename)
{
	std::ifstream file(filename);
	std::stringstream stream;
	stream << file.rdbuf();
	file.close();

	return stream.str();
}