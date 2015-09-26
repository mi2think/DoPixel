#include <GL/freeglut.h>

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("tutorial 01");

	glutDisplayFunc(RenderScene);
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	glutMainLoop();
	return 0;
}