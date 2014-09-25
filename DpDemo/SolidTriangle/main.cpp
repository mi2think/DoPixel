#include "DemoApp.h"

class SolidTriangle : public DemoApp
{
public:
	void OnCreate();

	void Run(float fElapsedTime);

	void Render(float fElapsedTime);

	bool MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	Camera camera;
	RenderList renderList;
	PolyFace polyFace;
	Vector4f polyPos;

	float angleX;
	float angleY;
	float angleZ;
	bool bX, bY, bZ;
	Matrix44f matrixR;
};

void SolidTriangle::OnCreate()
{
	camera.InitCamera(Camera::MODEL_EULER, Vector4f(0, 0, -100, 0), Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 0), 50, 500, 90, (float)clientWidth, (float)clientHeight);
	
	polyFace.state = POLY_STATE_ACTIVE;
	polyFace.attr = 0;
	polyFace.color = Color(255,0,0);
	polyFace.shadeColor = polyFace.color;

	polyFace.vlist[0] = Vector4f(0, 50, 0, 1);
	polyFace.vlist[1] = Vector4f(50, -50, 0, 1);
	polyFace.vlist[2] = Vector4f(-50, -50, 0, 1);

	polyPos = Vector4f(0, 0, 100, 1);

	angleX = angleY = angleZ = 0.0f;

	bX = bY = bZ = false;

	camera.BuildCameraMatrixEuler(Camera::ROTATE_SEQ_ZYX);

	matrixR.Identity();
}

void SolidTriangle::Run(float fElapsedTime)
{
	if (bX)
		MatrixRotationX(matrixR, angle2radian(angleX));
	else if (bY)
		MatrixRotationY(matrixR, angle2radian(angleY));
	else if (bZ)
		MatrixRotationZ(matrixR, angle2radian(angleZ));

	if ((angleX += 0.5f) >= 360.0f)
		angleX = 0.0f;
	if ((angleY += 0.5f) >= 360.0f)
		angleY = 0.0f;
	if ((angleZ += 0.5f) >= 360.0f)
		angleZ = 0.0f;

	renderList.Reset();

	renderList.InsertPolyFace(polyFace);

	renderList.Transform(matrixR, TRANSFORM_LOCAL_ONLY);

	renderList.ModelToWorld(polyPos);

	renderList.WorldToCamera(camera);

	renderList.CameraToPerspective(camera);

	renderList.PerspectiveToScreen(camera);
}

void SolidTriangle::Render(float fElapsedTime)
{
	renderList.RenderSolid(device);
}

bool SolidTriangle::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CHAR:
		switch (wParam)
		{
		case 'x':
		case 'y':
		case 'z':
			bX = (wParam == 'x');
			bY = (wParam == 'y');
			bZ = (wParam == 'z');
			break;
		}
		break;
	}
	return DemoApp::MsgProc(hwnd, msg, wParam, lParam);
}


int main()
{
	SolidTriangle* app = new SolidTriangle;
	app->Create(400, 400, "Solid Triangle");
	app->Loop();
	delete app;

	return 0;
}
