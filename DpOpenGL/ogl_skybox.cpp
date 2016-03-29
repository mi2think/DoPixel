/********************************************************************
	created:	2016/03/29
	created:	29:3:2016   22:12
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_skybox.cpp
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_skybox
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	ogl skybox
*********************************************************************/
#include "ogl_skybox.h"

namespace ogl
{
	Skybox::Skybox(const Camera* camera, const PersProjInfo& p)
		: camera_(camera)
		, persProjInfo_(p)
	{

	}

	Skybox::~Skybox()
	{

	}

	bool Skybox::Init(
		const std::string& posXFileName,
		const std::string& negXFileName,
		const std::string& posYFileName,
		const std::string& negYFileName,
		const std::string& posZFileName,
		const std::string& negZFileName,
		const std::string& modelFileName)
	{
		skyboxTechnique_ = new SkyboxTechnique();
		if (!skyboxTechnique_->Init("skybox.vs", "skybox.ps", "gWVP", "gCubemapTexture"))
		{
			fprintf(stderr, "error init skybox technique\n");
			return false;
		}
		skyboxTechnique_->Enable();
		skyboxTechnique_->SetTextureUnit(0);

		cubemapTexture_ = new CubemapTexture(posXFileName, negXFileName, posYFileName, negYFileName, posZFileName, negZFileName);
		if (cubemapTexture_->Load())
			return false;

		mesh_ = new Mesh();
		return mesh_->LoadMesh(modelFileName);
	}

	void Skybox::Render()
	{
		skyboxTechnique_->Enable();

		GLint oldCullFaceMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		GLint oldDepthFuncMode;
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);

		Pipeline p;
		p.Scale(20.0f, 20.0f, 20.0f);
		p.Rotate(0.0f, 0.0f, 0.0f);
		p.WorldPos(camera_->GetPosition());
		p.SetCamera(*camera_);
		p.SetPerspectiveProj(persProjInfo_);

		skyboxTechnique_->SetWVP(p.GetOGLWorldViewProjTrans());
		cubemapTexture_->Bind(GL_TEXTURE0);
		mesh_->Render();

		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
	}
}