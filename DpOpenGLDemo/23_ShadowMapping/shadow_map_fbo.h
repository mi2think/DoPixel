#pragma once

#include <GL/glew.h>

class ShadowMapFBO
{
public:
	ShadowMapFBO();
	~ShadowMapFBO();

	bool Init(int width, int height);
	void BindForWriting();
	void BindForReading(GLenum textureUnit);
private:
	GLuint fbo_;
	GLuint shadowMap_;
};