#include "OpenGLDemoApp.h"

class GouraudTriangle : public OpenGLDemoApp
{
public:
	void OnCreate() {}

	void Run(float fElapsedTime) {}

	void Render(float fElapsedTime);
};

void GouraudTriangle::Render(float fElapsedTime)
{
	glTranslatef(0.0f, 0.0f, 0.0f);

	glBegin(GL_TRIANGLES);

	// triangle 1
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-2.25f, 0.0f, -2.0f);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.25f, 1.0f, -2.0f);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-0.25f, 0.0f, -2.0f);

	// triangle 2
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.25f, 0.0f, -2.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.25f, 1.0f, -2.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(2.25f, 0.0f, -2.0f);

	// triangle 3
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-2.25f, -1.0f, -2.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.25f, 0.0f, -2.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.25f, -1.5f, -2.0f);

	// triangle 3
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.25f, -1.5f, -2.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.25f, 0.0f, -2.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(2.25f, -1.0f, -2.0f);

	glEnd();
}


int main()
{
	GouraudTriangle app;
	app.Create(640, 480, "Gouraud Triangle");
	app.Loop();
	return 0;
}
