/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   20:50
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_technique.cpp
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_technique
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	ogl technique
*********************************************************************/
#include "ogl_technique.h"

#include "DpFileStream.h"

namespace ogl
{
	Technique::Technique()
		: shaderProg_(0)
	{

	}

	Technique::~Technique()
	{
		for (auto shaderObj : shaderObjList_)
		{
			glDeleteShader(shaderObj);
		}

		if (shaderProg_ != 0)
		{
			glDeleteProgram(shaderProg_);
			shaderProg_ = 0;
		}
	}

	bool Technique::Init()
	{
		shaderProg_ = glCreateProgram();

		if (shaderProg_ == 0)
		{
			fprintf(stderr, "error create shader program\n");
			return false;
		}
		return true;
	}

	bool Technique::AddShader(GLenum shaderType, const char* strShader)
	{
		GLuint shaderObj = glCreateShader(shaderType);
		if (shaderObj == 0)
		{
			fprintf(stderr, "error create shader type:%d\n", shaderType);
			return false;
		}

		shaderObjList_.push_back(shaderObj);

		const GLchar* p[1] = { strShader };
		GLint lengths[1] = { (GLint)strlen(strShader) };

		glShaderSource(shaderObj, 1, p, lengths);
		glCompileShader(shaderObj);
		GLint success;
		glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLchar infoLog[1024];
			glGetShaderInfoLog(shaderObj, sizeof(infoLog), nullptr, infoLog);
			fprintf(stderr, "error compile shader:\n%s\n", infoLog);
			return false;
		}

		glAttachShader(shaderProg_, shaderObj);

		return true;
	}

	bool Technique::AddShaderFromFile(GLenum shaderType, const char* fileName)
	{
		dopixel::FileStream fs(fileName, dopixel::FileStream::TextRead);
		size_t size = (size_t)fs.Size();
		char* fileData = new char[size + 1];
		dopixel::ON_SCOPE_EXIT([&fileData]() { SAFE_DELETEARRAY(fileData); });

		if (fileData)
		{
			size_t readSize = fs.Read(fileData, size);
			fileData[readSize] = 0;
#ifdef _DEBUG
			if (shaderType == GL_VERTEX_SHADER)
				fprintf(stdout, "vs:\n");
			else
				fprintf(stdout, "ps:\n");
			fprintf(stdout, "%s\n", fileData);
#endif // _DEBUG

			return AddShader(shaderType, fileData);
		}
		return false;
	}

	bool Technique::Finalize()
	{
		GLint success = 0;
		GLchar errorLog[1024] = { 0 };

		glLinkProgram(shaderProg_);
		glGetProgramiv(shaderProg_, GL_LINK_STATUS, &success);
		if (success == 0)
		{
			glGetProgramInfoLog(shaderProg_, sizeof(errorLog), nullptr, errorLog);
			fprintf(stderr, "error link shader program:\n%s\n", errorLog);
			return false;
		}

		glValidateProgram(shaderProg_);
		glGetProgramiv(shaderProg_, GL_VALIDATE_STATUS, &success);
		if (success == 0)
		{
			glGetProgramInfoLog(shaderProg_, sizeof(errorLog), nullptr, errorLog);
			fprintf(stderr, "error validate shader program:\n%s\n", errorLog);
			return false;
		}

		// delete shader objects
		for (auto shaderObj : shaderObjList_)
		{
			glDeleteShader(shaderObj);
		}
		shaderObjList_.clear();

		return glGetError() == GL_NO_ERROR;
	}

	void Technique::Enable()
	{
		glUseProgram(shaderProg_);
	}

	GLint Technique::GetUniformLocation(const char* uniformName)
	{
		GLuint location = glGetUniformLocation(shaderProg_, uniformName);
		if (location == 0xffffffff)
		{
			fprintf(stderr, "warning! unable to get location of uniform:'%s'\n", uniformName);
		}
		return location;
	}

	GLint Technique::GetProgramParam(GLint param)
	{
		GLint ret;
		glGetProgramiv(shaderProg_, param, &ret);
		return ret;
	}
}