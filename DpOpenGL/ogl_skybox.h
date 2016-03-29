/********************************************************************
	created:	2016/03/29
	created:	29:3:2016   21:15
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_skybox.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_skybox
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	ogl skybox
*********************************************************************/
#ifndef __OGL_SKYBOX_H__
#define __OGL_SKYBOX_H__

#include "ogl_pipeline.h"
#include "ogl_skybox_technique.h"
#include "ogl_cubemap_texture.h"
#include "ogl_mesh.h"
#include "ogl_camera.h"

#include <string>

namespace ogl
{
	class Skybox
	{
	public:
		Skybox(const Camera* camera, const PersProjInfo& p);
		~Skybox();

		bool Init ( const std::string& posXFileName,
					const std::string& negXFileName,
					const std::string& posYFileName,
					const std::string& negYFileName,
					const std::string& posZFileName,
					const std::string& negZFileName,
					const std::string& modelFileName);

		void Render();
	private:
		SkyboxTechnique* skyboxTechnique_;
		CubemapTexture* cubemapTexture_;
		Mesh* mesh_;
		const Camera* camera_;
		PersProjInfo persProjInfo_;
	};
}

#endif
