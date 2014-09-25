#include "DemoApp.h"

class SingleTriangle : public DemoApp
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

	float angleY;
	Matrix44f matrixR;
};


void SingleTriangle::OnCreate()
{
	camera.InitCamera(Camera::MODEL_EULER, Vector4f(0,0,-100,1), Vector4f(0,0,0,1), Vector4f(0,0,0,0), 50, 500, 90, (float)clientWidth, (float)clientHeight);

	polyFace.state = POLY_STATE_ACTIVE;
	polyFace.attr = 0;
	polyFace.color = Color(255,0,0);
	polyFace.shadeColor = polyFace.color;

	polyFace.vlist[0] = Vector4f(0, 50, 0, 1);
	polyFace.vlist[1] = Vector4f(50, -50, 0, 1);
	polyFace.vlist[2] = Vector4f(-50, -50, 0, 1);

	polyPos = Vector4f(0,0,100,1);

	angleY = 0.0f;

	camera.BuildCameraMatrixEuler(Camera::ROTATE_SEQ_ZYX);
}

void SingleTriangle::Run(float fElapsedTime)
{
	MatrixRotationY(matrixR, angle2radian(angleY));

	if (++angleY >= 360.0f)
		angleY = 0.0f;

	renderList.Reset();

	renderList.InsertPolyFace(polyFace);

	renderList.Transform(matrixR, TRANSFORM_LOCAL_ONLY);

	renderList.ModelToWorld(polyPos);

	renderList.WorldToCamera(camera);

	renderList.CameraToPerspective(camera);

	renderList.PerspectiveToScreen(camera);
}

void SingleTriangle::Render(float fElapsedTime)
{
	renderList.RenderWire(device);
}


int main()
{
	SingleTriangle* app = new SingleTriangle;
	app->Create(400, 400, "Single Triangle");
	app->Loop();
	delete app;

	return 0;
}
