#include "DemoApp.h"

class GouraudTriangle : public DemoApp
{
public:
	void OnCreate();

	void Run(float fElapsedTime);

	void Render(float fElapsedTime);

private:
	Camera camera;
	RenderList renderList;
	PolyFace polyFace;
	Vector4f polyPos;
};


void GouraudTriangle::OnCreate()
{
	camera.InitCamera(Camera::MODEL_EULER, Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), 50, 500, 90, (float)clientWidth, (float)clientHeight);

	polyFace.state = POLY_STATE_ACTIVE;
	polyFace.attr = 0;
	polyFace.color = Color(255, 0, 0);
	polyFace.litColor[0] = polyFace.color;

	polyFace.vlist[0].v = Vector4f(-1.0f, 0.0f, 2.0f, 1);
	polyFace.vlist[1].v = Vector4f( 0.0f, 1.0f, 2.0f, 1);
	polyFace.vlist[2].v = Vector4f( 1.0f, 0.0f, 2.0f, 1);

	polyPos = Vector4f(0, 0, 0, 1);

	camera.BuildCameraMatrixEuler(Camera::ROTATE_SEQ_ZYX);
}

void GouraudTriangle::Run(float fElapsedTime)
{
	renderList.Reset();

	renderList.InsertPolyFace(polyFace);

	renderList.ModelToWorld(polyPos);

	renderList.WorldToCamera(camera);

	renderList.CameraToPerspective(camera);

	renderList.PerspectiveToScreen(camera);
}

void GouraudTriangle::Render(float fElapsedTime)
{
	renderList.RenderWire(device);
}


int main()
{
	GouraudTriangle* app = new GouraudTriangle;
	app->Create(640, 480, "Gouraud Triangle");
	app->Loop();
	delete app;

	return 0;
}
