#include "DemoApp.h"

#define NUM_OBJECTS     16
#define OBJECT_SPACING  250

class TankUVN : public DemoApp
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

	Matrix44f rotation;

	int viewAngle;
};

void TankUVN::OnCreate()
{
	camera.InitCamera(Camera::MODEL_UVNSimple, Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), 50, 8000, 90, (float)clientWidth, (float)clientHeight);
	
	LoadObjectFromPLG(object, "tank1.plg", Vector4f(1,1,1,1), Vector4f(0,0,0,1));

	object.worldPos = Vector4f(0,0,0,1);

	viewAngle = 0;

//	camera.pos = Vector4f(0, 0, -1750, 1);
}

void TankUVN::Run(float fElapsedTime)
{
	MatrixRotationY(rotation, angle2radian(1));

	object.Transform(rotation, TRANSFORM_LOCAL_ONLY, true);

	const int cameraDist = 1750;

	camera.pos.x = cameraDist * cos(angle2radian(viewAngle));
	camera.pos.y = cameraDist* sin(angle2radian(viewAngle));
	camera.pos.z = 2 * cameraDist * sin(angle2radian(viewAngle));

	if (++viewAngle >= 360)
		viewAngle = 0;

	renderList.Reset();

	for (int x = -NUM_OBJECTS / 2; x < NUM_OBJECTS / 2; ++x)
	{
		for (int z = -NUM_OBJECTS / 2; z < NUM_OBJECTS / 2; ++z)
		{
			object.ResetCull();

			Vector4f worldPos;
			worldPos.x = float(x * OBJECT_SPACING + OBJECT_SPACING / 2);
			worldPos.y = 0;
			worldPos.z = float(z * OBJECT_SPACING + OBJECT_SPACING / 2);

			if (!object.Cull(camera, worldPos, Camera::CULL_PLANE_XYZ))
			{
				object.ModelToWorld(object.worldPos);

				renderList.InsertObject(object);
			}
		}
	}

	camera.BuildCameraMatrixUVN(Camera::MODEL_UVNSimple);

	renderList.RemoveBackfaces(camera);

	renderList.WorldToCamera(camera);

	renderList.CameraToPerspective(camera);

	renderList.PerspectiveToScreen(camera);
}

void TankUVN::Render(float fElapsedTime)
{
	renderList.RenderWire(device);
}

bool TankUVN::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
	TankUVN* app = new TankUVN;
	app->CreateFullScreen("tankUVN");
	app->Loop();
	delete app;

	return 0;
}
