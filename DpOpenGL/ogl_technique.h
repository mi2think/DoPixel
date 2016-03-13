/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   20:49
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_technique.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_technique
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	ogl technique
*********************************************************************/
#ifndef __OGL_TECHNIQUE_H__
#define __OGL_TECHNIQUE_H__

#include <GL/glew.h>

#include <list>

namespace ogl
{
	class Technique
	{
	public:
		Technique();
		virtual ~Technique();

		virtual bool Init();

		void Enable();
	protected:
		bool AddShader(GLenum shaderType, const char* strShader);

		bool AddShaderFromFile(GLenum shaderType, const char* fileName);

		bool Finalize();

		GLint GetUniformLocation(const char* uniformName);

		GLint GetProgramParam(GLint param);

		GLuint shaderProg_;
	private:
		typedef std::list<GLuint> ShaderObjList;
		ShaderObjList shaderObjList_;
	};
}

#endif
