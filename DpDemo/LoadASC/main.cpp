#include "DemoApp.h"

class LoadASC : public DemoApp
{
public:
	void OnCreate();

	void Run(float fElapsedTime);

	void Render(float fElapsedTime);

	bool MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	Camera camera;
	Object object;
	RenderList renderList;

	float angleY;
	Matrix44f matrixR;

	bool wireMode;
};

void LoadASC::OnCreate()
{
//	camera.InitCamera(Camera::MODEL_EULER, Vector4f(0, 0, -50, 1), Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), 10, 1200, 120, (float)clientWidth, (float)clientHeight);
	camera.InitCamera(Camera::MODEL_UVNSimple, Vector4f(0, 0, -50, 1), Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), 10, 1200, 90, (float)clientWidth, (float)clientHeight);
	angleY = 0;
	wireMode = false;

	LoadObjectFrom3DSASC(object, "car01.asc", Vector4f(5, 5, 5, 1), Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), VERTEX_FLAGS_SWAP_YZ | VERTEX_FLAGS_INVERT_WINDING_ORDER);
}

void LoadASC::Run(float fElapsedTime)
{
	MatrixRotationY(matrixR, angle2radian(angleY));

	if (++angleY >= 360.0f)
		angleY = 0.0f;

	renderList.Reset();

//	camera.BuildCameraMatrixEuler(Camera::ROTATE_SEQ_ZYX);
	camera.BuildCameraMatrixUVN(Camera::MODEL_UVNSimple);

	// Try to cull object
	object.ResetCull();

	if (!object.Cull(camera, Vector4f(0, 0, 0, 1), Camera::CULL_PLANE_XYZ))
	{
		object.ModelToWorld(Vector4f(0, 0, 0, 1));
		renderList.InsertObject(object);
	}

//	renderList.RemoveBackfaces(camera);

	renderList.WorldToCamera(camera);

	renderList.SortByZ();

	renderList.CameraToPerspective(camera);

	renderList.PerspectiveToScreen(camera);
}


void LoadASC::Render(float fElapsedTime)
{
	if (wireMode)
		renderList.RenderWire(device);
	else
		renderList.RenderSolid(device);
}

bool LoadASC::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CHAR:
		switch (wParam)
		{
		case 'w':
			camera.pos.z += 5;
			break;
		case 's':
			camera.pos.z -= 5;
			break;
		case 'a':
			camera.pos.x -= 5;
			break;
		case 'd':
			camera.pos.x += 5;
			break;
		case 'b':
			wireMode = !wireMode;
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			camera.pos.y += 5;
			break;
		case VK_DOWN:
			camera.pos.y -= 5;
			break;
		}
		break;
	}
	return DemoApp::MsgProc(hwnd, msg, wParam, lParam);
}


int main()
{
	LoadASC* app = new LoadASC;
	app->Create(1440, 900, "car");
	app->Loop();
	delete app;

	return 0;
}