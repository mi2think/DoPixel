#include "DemoApp.h"
#include "DpLoadPLG.h"

class CWireCube : public DemoApp
{
public:
	void OnCreate();

	void Run(float fElapsedTime);

	void Render(float fElapsedTime);

	bool MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	Camera camera;
	Object object;

	int xangle;
	int yangle;
	int zangle;
	Matrix44f rotation;

	Vector4f worldPos;
};

void CWireCube::OnCreate()
{
	camera.InitCamera(Camera::MODEL_EULER, Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 0), 50, 500, 90.0f, (float)clientWidth, (float)clientHeight);
	camera.BuildCameraMatrixEuler(Camera::ROTATE_SEQ_ZYX);

	LoadObjectFromPLG(object, "cube2.plg", Vector4f(5.0, 5.0, 5.0, 1.0), Vector4f(0,0,0,1));

	xangle = 0;
	yangle = 0;
	zangle = 0;
	worldPos = Vector4f(0,0,100,1);
}

void CWireCube::Run(float fElapsedTime)
{
	object.ResetCull();

	Matrix44f mx, my, mz;
	MatrixRotationX(mx, angle2radian(xangle));
	MatrixRotationY(my, angle2radian(yangle));
	MatrixRotationZ(mz, angle2radian(zangle));

	Matrix44f m;
	MatrixMultiply(m, mz, my);
	MatrixMultiply(rotation, m, mx);

	object.ResetCull();
	object.Transform(rotation, TRANSFORM_LOCAL_TO_TRANS, false);
	object.ModelToWorld(worldPos, TRANSFORM_TRANS_ONLY);
	object.RemoveBackfaces(camera);
	object.WorldToCamera(camera);
	object.CameraToPerspective(camera);
	object.PerspectiveToScreen(camera);
}

void CWireCube::Render(float fElapsedTime)
{
	object.RenderWire(device);
}

bool CWireCube::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CHAR:
		{
			switch (wParam)
			{
			case 'x':
				xangle += 1;
				break;
			case 'y':
				yangle += 1;
				break;
			case 'z':
				zangle += 1;
				break;
			case 'c':
				xangle = 0;
				yangle = 0;
				zangle = 0;
				break;
			case 'w':
				worldPos.z += 5;
				break;
			case 's':
				worldPos.z -= 5;
				break;
			}
		}
		break;
	case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_LEFT:
				worldPos.x += 5;
				break;
			case VK_RIGHT:
				worldPos.x -= 5;
				break;
			case VK_UP:
				worldPos.y += 5;
				break;
			case VK_DOWN:
				worldPos.y -= 5;
				break;
			}
		}
		break;
	}
	return DemoApp::MsgProc(hwnd, msg, wParam, lParam);
}

int main()
{
	CWireCube* app = new CWireCube;
	app->Create(400, 400, "Wire Cube");
	app->Loop();
	delete app;

	return 0;
}