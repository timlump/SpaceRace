#include "stdafx.h"
#include "Shader.h"

std::map<std::string,GLuint> Shader::mVertShaders = std::map<std::string,GLuint>();
std::map<std::string,GLuint> Shader::mFragShaders = std::map<std::string,GLuint>();
std::map<std::pair<GLuint,GLuint>,GLuint> Shader::mPrograms = std::map<std::pair<GLuint,GLuint>,GLuint>();

Shader::Shader(std::string vertShader,std::string fragShader)
{
	mVertexShader = createVertexShader(vertShader);
	mFragmentShader = createFragmentShader(fragShader);
	mProgram = createProgram(mVertexShader,mFragmentShader);
}

GLuint Shader::createVertexShader(std::string vertShader)
{
	auto id = mVertShaders.find(vertShader);
	if(id != mVertShaders.end())
	{
		return id->second;
	}
	else
	{
		GLuint sdr = glCreateShader(GL_VERTEX_SHADER);
		std::string source = loadShaderSource(vertShader);
		const GLchar * tmp = static_cast<const GLchar *>(source.c_str());

		glShaderSource(sdr,1,&tmp,NULL);
		glCompileShader(sdr);

		mVertShaders[vertShader] = sdr;

		GLint status;
		glGetShaderiv(sdr,GL_COMPILE_STATUS,&status);
		if(status == GL_FALSE)
		{
			char buffer[512];
			glGetShaderInfoLog(sdr,512,NULL,buffer);
			printf("%s\n",buffer);
		}

		return sdr;
	}
}

GLuint Shader::createFragmentShader(std::string fragShader)
{
	auto id = mFragShaders.find(fragShader);
	if(id != mFragShaders.end())
	{
		return id->second;
	}
	else
	{
		GLuint sdr = glCreateShader(GL_FRAGMENT_SHADER);
		std::string source = loadShaderSource(fragShader);
		const GLchar * tmp = static_cast<const GLchar *>(source.c_str());

		glShaderSource(sdr,1,&tmp,NULL);
		glCompileShader(sdr);

		mFragShaders[fragShader] = sdr;

		GLint status;
		glGetShaderiv(sdr,GL_COMPILE_STATUS,&status);
		if(status == GL_FALSE)
		{
			char buffer[512];
			glGetShaderInfoLog(sdr,512,NULL,buffer);
			printf("%s\n",buffer);
		}

		return sdr;
	}
}

GLuint Shader::createProgram(GLuint vertID,GLuint fragID)
{
	//see if the program already exists
	bool found = false;
	for(std::map<std::pair<GLuint,GLuint>,GLuint>::iterator it = mPrograms.begin() ; it != mPrograms.end() ; ++it)
	{
		std::pair<GLuint,GLuint> ids = it->first;
		if(ids.first == vertID && ids.second == fragID)
		{
			found = true;
			return it->second;
		}
	}

	GLuint sdr = glCreateProgram();
	glAttachShader(sdr,vertID);
	glAttachShader(sdr,fragID);
	glLinkProgram(sdr);
	glUseProgram(sdr);

	std::pair<GLuint,GLuint> key = std::pair<GLuint,GLuint>(vertID,fragID);
	mPrograms[key] = sdr;

	return sdr;
}

std::string Shader::loadShaderSource(std::string filename)
{
	std::ifstream file(filename);
	std::stringstream stream;
	stream << file.rdbuf();
	file.close();

	return stream.str();
}