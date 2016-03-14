#include <cstdio>

#include "shadow_map_fbo.h"

ShadowMapFBO::ShadowMapFBO()
	: fbo_(0)
	, shadowMap_(0)
{

}

ShadowMapFBO::~ShadowMapFBO()
{
	if (fbo_ != 0)
	{
		glDeleteFramebuffers(1, &fbo_);
	}
	if (shadowMap_ != 0)
	{
		glDeleteTextures(1, &shadowMap_);
	}
}

bool ShadowMapFBO::Init(int width, int height)
{
	// create the fbo
	glGenFramebuffers(1, &fbo_);

	// create the depth buffer
	glGenTextures(1, &shadowMap_);
	glBindTexture(GL_TEXTURE_2D, shadowMap_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap_, 0);

	// disable writes to the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "FB error, status:0x%x\n", status);
		return false;
	}

	return true;
}

void ShadowMapFBO::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
}

void ShadowMapFBO::BindForReading(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, shadowMap_);
}
