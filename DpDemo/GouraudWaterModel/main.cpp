#include "DemoApp.h"

class GouraudWaterModel : public DemoApp
{
public:
	void OnCreate();

	bool MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void Run(float fElapsedTime);

	void Render(float fElapsedTime);
private:
	Camera camera;
	RenderList renderList;

	Object objConstantWater;
	Object objFlatWater;
	Object objGouraudWater;

	float viewAngle;

	bool wireframeMode;
	bool backfaceMode;
	bool lightingMode;
	int zsortMode;

	// light
	unsigned int pointLightId;
	unsigned int spotLightId;

	// angles for light motion
	float plightAngle;
	float slightAngle;

	Matrix44f rotMatrix;
};

void GouraudWaterModel::OnCreate()
{
	camera.InitCamera(Camera::MODEL_EULER, Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 12000.0f, 
		120.0f, (float)clientWidth, (float)clientHeight);

	// load constant shaded water
	Vector4f scale(10.0f, 10.0f, 10.0f, 1.0f);
	Vector4f pos(0.0f, 0.0f, 0.0f, 1.0f);
	Vector4f rot(0.0f, 0.0f, 0.0f, 1.0f);
	LoadObjectFromCOB(objConstantWater, "water_constant_01.cob", scale, pos, rot, VERTEX_FLAGS_SWAP_YZ | VERTEX_FLAGS_TRANSFORM_LOCAL | VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD);

	// load flat shaded water
	LoadObjectFromCOB(objFlatWater, "water_flat_01.cob", scale, pos, rot, VERTEX_FLAGS_SWAP_YZ | VERTEX_FLAGS_TRANSFORM_LOCAL | VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD);

	// load gouraud shaded water
	LoadObjectFromCOB(objGouraudWater, "water_gouraud_01.cob", scale, pos, rot, VERTEX_FLAGS_SWAP_YZ | VERTEX_FLAGS_TRANSFORM_LOCAL | VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD);


	// light
	lightFactory.AddAmbientLight(Color(100, 100, 100));

	lightFactory.AddSunLight(Vector4f(-1.0f, 0.0f, -1.0f, 1.0f), Color(100, 100, 100));

	pointLightId = lightFactory.AddPointLight(Vector4f(0.0, 200.0f, 0.0f, 1.0f), Color(0, 255, 0), 0.0f, 0.001f, 0.0f);

	spotLightId = lightFactory.AddSpotLight(Light::ATTR_SPOTLIGHT2, Vector4f(0.0f, 200.0f, 0.0f, 1.0f), Vector4f(-1.0f, 0.0f, -1.0f, 1.0f), Color(255, 0, 0), 0.0f, 0.001f, 0.0f, 0.0f, 0.0f, 1.0f);

	// others
	viewAngle = 0.0f;
	wireframeMode = true;
	backfaceMode = true;
	lightingMode = true;
	zsortMode = 1;
	plightAngle = 0.0f;
	slightAngle = 0.0f;
}

bool GouraudWaterModel::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CHAR:
	{
		switch (wParam)
		{
		case 'w':
			wireframeMode = !wireframeMode;
			break;
		case 'b':
			backfaceMode = !backfaceMode;
			break;
		case 'l':
			lightingMode = !lightingMode;
			break;
		}
	}
	break;
	}
	return DemoApp::MsgProc(hwnd, msg, wParam, lParam);
}

void GouraudWaterModel::Run(float fElapsedTime)
{
	// move point light source in ellipse around game world
	Light* pointLight = lightFactory.GetLight(pointLightId);
	pointLight->pos.x = 1000 * cos(angle2radian(plightAngle));
	pointLight->pos.y = 100;
	pointLight->pos.z = 1000 * sin(angle2radian(plightAngle));

	if ((plightAngle += 3) > 360)
		plightAngle = 0.0f;

	// move spot light source in ellipse around game world
	Light* spotLight = lightFactory.GetLight(spotLightId);
	spotLight->pos.x = 1000 * cos(angle2radian(slightAngle));
	spotLight->pos.y = 200;
	spotLight->pos.z = 1000 * sin(angle2radian(slightAngle));

	if ((slightAngle -= 5) < 0)
		slightAngle = 0.0f;

	// generate camera matrix
	camera.BuildCameraMatrixEuler(Camera::ROTATE_SEQ_ZYX);

	// use these to rotate objects
	static float x_ang = 0, y_ang = 0, z_ang = 0;

	// generate rotation matrix around y axis
	MatrixRotationXYZ(rotMatrix, angle2radian(x_ang), angle2radian(y_ang), angle2radian(z_ang));

	renderList.Reset();

	//////////////////////////////////////////////////////////////////////////
	// constant shaded water

	// reset the object (this only matters for back face and object removal)
	objConstantWater.ResetCull();

	// rotate
	objConstantWater.Transform(rotMatrix, TRANSFORM_LOCAL_TO_TRANS, 1, true);

	// model to world
	Vector4f worldPos = Vector4f(-50.0f, 0.0f, 120.0f, 1.0f);

	if (!objConstantWater.Cull(camera, worldPos, CULL_PLANE_XYZ))
	{
		// Since local has transformed(rotate) 
		objConstantWater.ModelToWorld(worldPos, TRANSFORM_TRANS_ONLY, true);

		renderList.InsertObject(objConstantWater);
	}

	//////////////////////////////////////////////////////////////////////////
	// flat shaded water

	// reset the object (this only matters for back face and object removal)
	objFlatWater.ResetCull();

	// rotate
	objFlatWater.Transform(rotMatrix, TRANSFORM_LOCAL_TO_TRANS, 1, true);

	// model to world
	worldPos = Vector4f(0.0f, 0.0f, 120.0f, 1.0f);

	if (!objFlatWater.Cull(camera, worldPos, CULL_PLANE_XYZ))
	{
		// Since local has transformed(rotate) 
		objFlatWater.ModelToWorld(worldPos, TRANSFORM_TRANS_ONLY, true);

		renderList.InsertObject(objFlatWater);
	}

	//////////////////////////////////////////////////////////////////////////
	// gouraud shaded water

	// reset the object (this only matters for back face and object removal)
	objGouraudWater.ResetCull();

	// rotate
	objGouraudWater.Transform(rotMatrix, TRANSFORM_LOCAL_TO_TRANS, 1, true);

	// model to world
	worldPos = Vector4f(50.0f, 0.0f, 120.0f, 1.0f);

	if (!objGouraudWater.Cull(camera, worldPos, CULL_PLANE_XYZ))
	{
		// Since local has transformed(rotate) 
		objGouraudWater.ModelToWorld(worldPos, TRANSFORM_TRANS_ONLY, true);

		renderList.InsertObject(objGouraudWater);
	}

	//////////////////////////////////////////////////////////////////////////
	// update rotation angles
	if ((x_ang += 1) > 360) x_ang = 0;
	if ((y_ang += 2) > 360) y_ang = 0;
	if ((z_ang += 3) > 360) z_ang = 0;

	// remove backfaces
	if (backfaceMode)
		renderList.RemoveBackfaces(camera);

	// lit scene
	if (lightingMode)
		renderList.Lighting(camera, lightFactory.GetLights());

	// world to camera
	renderList.WorldToCamera(camera);

	// sort by z
	if (zsortMode == 1)
		renderList.SortByZ();

	// camera to perspective
	renderList.CameraToPerspective(camera);

	// perspective to screen
	renderList.PerspectiveToScreen(camera);
}

void GouraudWaterModel::Render(float fElapsedTime)
{
	if (wireframeMode)
		renderList.RenderWire(device);
	else
		renderList.RenderSolid(device);
}

int main()
{
	GouraudWaterModel* app = new GouraudWaterModel();
	app->Create(800, 600, "Gouraud water model");
	app->Loop();
	delete app;
	return 0;
}