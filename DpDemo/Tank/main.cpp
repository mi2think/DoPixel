#include "DemoApp.h"

#define NUM_OBJECTS     2       // number of objects on a row
#define OBJECT_SPACING  250     // spacing between objects

class Tank : public DemoApp
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
};

void Tank::OnCreate()
{
	camera.InitCamera(Camera::MODEL_EULER, Vector4f(0,200,0,1), Vector4f(0,0,0,1), Vector4f(0,0,0,0), 50, 1000, 90, (float)clientWidth, (float)clientHeight);

	LoadObjectFromPLG(object, "tank1.plg", Vector4f(1,1,1,1), Vector4f(0,0,0,1));

	object.worldPos = Vector4f(0, 0, 400, 1);
}

void Tank::Run(float fElapsedTime)
{
	MatrixRotationY(rotation, angle2radian(1));

	object.Transform(rotation, TRANSFORM_LOCAL_ONLY, true);

	renderList.Reset();

	for (int x = - NUM_OBJECTS / 2; x < NUM_OBJECTS / 2; ++x)
	{
		for (int z = -NUM_OBJECTS / 2; z < NUM_OBJECTS / 2; ++z)
		{
			object.ResetCull();

			object.worldPos.x = float(x * OBJECT_SPACING + OBJECT_SPACING / 2);
			object.worldPos.y = 0;
			object.worldPos.z = float(500 + z * OBJECT_SPACING + OBJECT_SPACING / 2);

			// Try to cull object
			if (! object.Cull(camera, Camera::CULL_PLANE_XYZ))
			{
				object.ModelToWorld(object.worldPos);
				
				renderList.InsertObject(object);
			}
		}
	}

	camera.BuildCameraMatrixEuler(Camera::ROTATE_SEQ_ZYX);
	
	renderList.RemoveBackfaces(camera);

	renderList.WorldToCamera(camera);

	renderList.CameraToPerspective(camera);

	renderList.PerspectiveToScreen(camera);
}

void Tank::Render(float fElapsedTime)
{
	renderList.RenderWire(device);
}

bool Tank::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_DOWN:
			camera.dir.x += angle2radian(1);
			break;
		case VK_UP:
			camera.dir.x -= angle2radian(1);
			break;
		case VK_RIGHT:
			camera.dir.y -= angle2radian(1);
			break;
		case VK_LEFT:
			camera.dir.y += angle2radian(1);
			break;
		}
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case 'z':
			camera.dir.z -= angle2radian(1);
			break;
		case 'x':
			camera.dir.z += angle2radian(1);
			break;
		}
		break;
	}
	return DemoApp::MsgProc(hwnd, msg, wParam, lParam);
}

int main()
{
	Tank* app = new Tank;
	app->Create(400, 400, "tank");
	app->Loop();
	delete app;

	return 0;
}
