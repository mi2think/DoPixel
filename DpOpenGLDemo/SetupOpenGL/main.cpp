#include "OpenGLDemoApp.h"

class SetupOpenGL : public OpenGLDemoApp
{
public:
	void OnCreate() {}

	void Run(float fElapsedTime) {}

	void Render(float fElapsedTime);
};

void SetupOpenGL::Render(float fElapsedTime)
{
	glTranslatef(0.0f, 0.0f, -6.0f);

	glBegin(GL_TRIANGLES);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	glEnd();
}


int main()
{
	SetupOpenGL app;
	app.Create(640, 480, "Setup OpenGL");
	app.Loop();
	return 0;
}
