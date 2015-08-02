#include "DemoApp.h"

class TextureTriangle : public DemoApp
{
public:
	void OnCreate();

	void Run(float fElapsedTime);

	void Render(float fElapsedTime);

	virtual bool MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	Camera camera;
	RenderList renderList;
	RenderList renderList2;
	RenderList renderList3;
	RenderList renderList4;

	PolyFace polyFace;
	PolyFace polyFace2;
	PolyFace polyFace3;

	float angleX;
	float angleY;
	float angleZ;
	bool bX, bY, bZ;
	Matrix44f matrixR;

	Texture texture;
};


void TextureTriangle::OnCreate()
{
	camera.InitCamera(Camera::MODEL_EULER, Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), 1, 1000, 90, (float)clientWidth, (float)clientHeight);

	// polyFace
	polyFace.state = POLY_STATE_ACTIVE;
	polyFace.attr = 0;
	// vlist
	polyFace.vlist[0].color = Color(255, 0, 0);
	polyFace.vlist[1].color = Color(0, 255, 0);
	polyFace.vlist[2].color = Color(0, 0, 255);

	polyFace.vlist[0].p = Vector4f(-1.0f, 0.0f, 2.0f, 1);
	polyFace.vlist[1].p = Vector4f(0.0f, 1.0f, 2.0f, 1);
	polyFace.vlist[2].p = Vector4f(1.0f, 0.0f, 2.0f, 1);

	polyFace.vlist[0].uv0 = Vector2f(0.0f, 1.0f);
	polyFace.vlist[1].uv0 = Vector2f(0.5f, 0.0f);
	polyFace.vlist[2].uv0 = Vector2f(1.0f, 1.0f);

	// tlist
	polyFace.tlist[0] = polyFace.vlist[0];
	polyFace.tlist[1] = polyFace.vlist[1];
	polyFace.tlist[2] = polyFace.vlist[2];

	// polyFace2
	polyFace2 = polyFace;
	polyFace2.vlist[0].p = Vector4f(-2.25f, -1.0f, 2.0f, 1);
	polyFace2.vlist[1].p = Vector4f(-1.25f, 0.0f, 2.0f, 1);
	polyFace2.vlist[2].p = Vector4f(-0.25f, -1.5f, 2.0f, 1);

	// polyFace3
	polyFace3 = polyFace;
	polyFace3.vlist[0].p = Vector4f(0.25f, -1.5f, 2.0f, 1);
	polyFace3.vlist[1].p = Vector4f(1.25f, 0.0f, 2.0f, 1);
	polyFace3.vlist[2].p = Vector4f(2.25f, -1.0f, 2.0f, 1);

	camera.BuildCameraMatrixEuler(Camera::ROTATE_SEQ_ZYX);

	angleX = angleY = angleZ = 0.0f;

	bX = bY = bZ = false;
	matrixR.Identity();

	texture.Load("crate.tga");
	device.SetTexture(&texture);
}

void TextureTriangle::Run(float fElapsedTime)
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

	{
		renderList.Reset();

		renderList.InsertPolyFace(polyFace);

		renderList.Transform(matrixR, TRANSFORM_LOCAL_ONLY);

		renderList.ModelToWorld(Vector4f(-1.25f, 0, 0, 1));

		renderList.WorldToCamera(camera);

		renderList.CameraToPerspective(camera);

		renderList.PerspectiveToScreen(camera);
	}

	{
		renderList2.Reset();

		renderList2.InsertPolyFace(polyFace);

		renderList2.Transform(matrixR, TRANSFORM_LOCAL_ONLY);

		renderList2.ModelToWorld(Vector4f(1.25f, 0, 0, 1));

		renderList2.WorldToCamera(camera);

		renderList2.CameraToPerspective(camera);

		renderList2.PerspectiveToScreen(camera);
	}

	{
		renderList3.Reset();

		renderList3.InsertPolyFace(polyFace2);

		renderList3.Transform(matrixR, TRANSFORM_LOCAL_ONLY);

		renderList3.ModelToWorld(Vector4f(0.0f, 0, 0, 1));

		renderList3.WorldToCamera(camera);

		renderList3.CameraToPerspective(camera);

		renderList3.PerspectiveToScreen(camera);
	}

	{
		renderList4.Reset();

		renderList4.InsertPolyFace(polyFace3);

		renderList4.Transform(matrixR, TRANSFORM_LOCAL_ONLY);

		renderList4.ModelToWorld(Vector4f(0.0f, 0, 0, 1));

		renderList4.WorldToCamera(camera);

		renderList4.CameraToPerspective(camera);

		renderList4.PerspectiveToScreen(camera);
	}
}

void TextureTriangle::Render(float fElapsedTime)
{
	renderList.RenderTexture(device, &texture);

	renderList2.RenderTexture(device, &texture);

	renderList3.RenderTexture(device, &texture);

	renderList4.RenderTexture(device, &texture);
}

bool TextureTriangle::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
	TextureTriangle* app = new TextureTriangle;
	app->Create(640, 480, "Texture Triangle");
	app->Loop();
	delete app;

	return 0;
}
