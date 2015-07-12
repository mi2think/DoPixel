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
	RenderList renderList2;
	RenderList renderList3;
	RenderList renderList4;

	PolyFace polyFace;
	PolyFace polyFace2;
	PolyFace polyFace3;
};


void GouraudTriangle::OnCreate()
{
	camera.InitCamera(Camera::MODEL_EULER, Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), 1, 1000, 90, (float)clientWidth, (float)clientHeight);

	polyFace.state = POLY_STATE_ACTIVE;
	polyFace.attr = 0;
	polyFace.color = Color(255, 0, 0);
	polyFace.litColor[0] = Color(255, 0, 0);
	polyFace.litColor[1] = Color(0, 255, 0);
	polyFace.litColor[2] = Color(0, 0, 255);

	polyFace.vlist[0].v = Vector4f(-1.0f, 0.0f, 2.0f, 1);
	polyFace.vlist[1].v = Vector4f( 0.0f, 1.0f, 2.0f, 1);
	polyFace.vlist[2].v = Vector4f( 1.0f, 0.0f, 2.0f, 1);

//	polyFace2 = polyFace;
	polyFace2.state = POLY_STATE_ACTIVE;
	polyFace2.attr = 0;
	polyFace2.color = Color(255, 0, 0);
	polyFace2.litColor[0] = Color(255, 0, 0);
	polyFace2.litColor[1] = Color(0, 255, 0);
	polyFace2.litColor[2] = Color(0, 0, 255);

	polyFace2.vlist[0].v = Vector4f(-2.25f, -1.0f, 2.0f, 1);
	polyFace2.vlist[1].v = Vector4f(-1.25f, 0.0f, 2.0f, 1);
	polyFace2.vlist[2].v = Vector4f(-0.25f, -1.5f, 2.0f, 1);

	//polyFace3 = polyFace;
	polyFace3.state = POLY_STATE_ACTIVE;
	polyFace3.attr = 0;
	polyFace3.color = Color(255, 0, 0);
	polyFace3.litColor[0] = Color(255, 0, 0);
	polyFace3.litColor[1] = Color(0, 255, 0);
	polyFace3.litColor[2] = Color(0, 0, 255);

	polyFace3.vlist[0].v = Vector4f(0.25f, -1.5f, 2.0f, 1);
	polyFace3.vlist[1].v = Vector4f(1.25f, 0.0f, 2.0f, 1);
	polyFace3.vlist[2].v = Vector4f(2.25f, -1.0f, 2.0f, 1);

	camera.BuildCameraMatrixEuler(Camera::ROTATE_SEQ_ZYX);
}

void GouraudTriangle::Run(float fElapsedTime)
{
	{
		renderList.Reset();

		renderList.InsertPolyFace(polyFace);

		renderList.ModelToWorld(Vector4f(-1.25f, 0, 0, 1));

		renderList.WorldToCamera(camera);

		renderList.CameraToPerspective(camera);

		renderList.PerspectiveToScreen(camera);
	}

	{
		renderList2.Reset();

		renderList2.InsertPolyFace(polyFace);

		renderList2.ModelToWorld(Vector4f(1.25f, 0, 0, 1));

		renderList2.WorldToCamera(camera);

		renderList2.CameraToPerspective(camera);

		renderList2.PerspectiveToScreen(camera);
	}

	{
		renderList3.Reset();

		renderList3.InsertPolyFace(polyFace2);

		renderList3.ModelToWorld(Vector4f(0.0f, 0, 0, 1));

		renderList3.WorldToCamera(camera);

		renderList3.CameraToPerspective(camera);

		renderList3.PerspectiveToScreen(camera);
	}

	{
		renderList4.Reset();

		renderList4.InsertPolyFace(polyFace3);

		renderList4.ModelToWorld(Vector4f(0.0f, 0, 0, 1));

		renderList4.WorldToCamera(camera);

		renderList4.CameraToPerspective(camera);

		renderList4.PerspectiveToScreen(camera);
	}
}

void GouraudTriangle::Render(float fElapsedTime)
{
	renderList.RenderSolid(device);

	renderList2.RenderGouraud(device);

	renderList3.RenderGouraud(device);

	renderList4.RenderGouraud(device);
}


int main()
{
	GouraudTriangle* app = new GouraudTriangle;
	app->Create(640, 480, "Gouraud Triangle");
	app->Loop();
	delete app;

	return 0;
}
