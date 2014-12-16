#include "DemoApp.h"

#define TANK_NUM	24
#define TOWER_NUM	96
#define TANK_SPEED	15

#define UNIVERSE_RADIUS	4000

#define POINT_SIZE        200
#define NUM_POINTS_X      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS_Z      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS        (NUM_POINTS_X*NUM_POINTS_Z)

class TankAndTower : public DemoApp
{
public:
	void OnCreate();

	void Run(float fElapsedTime);

	void Render(float fElapsedTime);

	bool MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	Camera camera;
	RenderList renderList;

	Object player;
	Object tank;
	Object tower;
	Object maker;

	float  tankSpeed;

	Vector4f tankPos[TANK_NUM];
	Vector4f towerPos[TOWER_NUM];

	Matrix44f mRot;
	int turning;

	unsigned int lightIndex1;
	unsigned int lightIndex2;
	unsigned int lightIndex3;
	unsigned int lightIndex4;
};

void TankAndTower::OnCreate()
{
	camera.InitCamera(Camera::MODEL_EULER, Vector4f(0, 40, 0, 1), Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), 200, 12000, 120, (float)clientWidth, (float)clientHeight);

	LoadObjectFromPLG(player, "tank3.plg", Vector4f(0.75f, 0.75f, 0.75f, 1), Vector4f(0, 0, 0, 1));
	LoadObjectFromPLG(tank, "tank2.plg", Vector4f(0.75f, 0.75f, 0.75f, 1), Vector4f(0, 0, 0, 1));
	LoadObjectFromPLG(tower, "tower1.plg", Vector4f(1.0f, 2.0f, 1.0f, 1), Vector4f(0, 0, 0, 1));
	LoadObjectFromPLG(maker, "marker1.plg", Vector4f(3.0f, 3.0f, 3.0f, 1), Vector4f(0, 0, 0, 1));

	for (int i = 0; i < TANK_NUM; ++i)
	{
		tankPos[i].x = (float)RandRange(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tankPos[i].y = 0;
		tankPos[i].z = (float)RandRange(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tankPos[i].w = (float)RandRange(0, 360);
	}

	for (int j = 0; j < TOWER_NUM; ++j)
	{
		towerPos[j].x = (float)RandRange(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		towerPos[j].y = 0;
		towerPos[j].z = (float)RandRange(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		towerPos[j].w = 1;
	}

	tankSpeed = 15;

	turning = 0;

	lightIndex1 = lightFactory.AddAmbientLight(Color::gray);

	lightIndex2 = lightFactory.AddPointLight(Vector4f(0, 200, 0, 1), Color::green, 0, 0.001f, 0);
	
	lightIndex3 = lightFactory.AddSunLight(Vector4f(-1, 0, -1, 1), Color::gray);
	
	lightIndex4 = lightFactory.AddSpotLight(Light::ATTR_SPOTLIGHT2, Vector4f(0, 200, 0, 1), Vector4f(-1, 0, -1, 1), Color::red, 0 ,0.001f, 0, 0, 0, 1);
}

void TankAndTower::Run(float fElapsedTime)
{
	// Camera

	// allow user to move camera
	// center heading again
	{
		if (turning > 0)
			turning -= 1;
		else
			if (turning < 0)
				turning += 1;

	} // end else

	// generate camera matrix
	camera.BuildCameraMatrixEuler(Camera::ROTATE_SEQ_ZYX);

	renderList.Reset();

	// Tower

	for (int i = 0; i < TOWER_NUM; ++i)
	{
		tower.ResetCull();

		tower.worldPos = towerPos[i];
		if (!tower.Cull(camera, Camera::CULL_PLANE_XYZ))
		{
			tower.ModelToWorld(tower.worldPos);

			renderList.InsertObject(tower);
		}
	}

	// Tank
	for (int i = 0; i < TANK_NUM; ++i)
	{
		tank.ResetCull();

		tank.worldPos = tankPos[i];
		tank.worldPos.w = 1;

		MatrixRotationY(mRot, angle2radian(tankPos[i].w));

		tank.Transform(mRot, TRANSFORM_LOCAL_TO_TRANS, true);

		if (! tank.Cull(camera, Camera::CULL_PLANE_XYZ))
		{
			// Since local has transformed(rotate) 
			tank.ModelToWorld(tank.worldPos, TRANSFORM_TRANS_ONLY);

			renderList.InsertObject(tank);
		}
	}

	// player

	player.ResetCull();

	// set position of player
	player.worldPos.x = camera.pos.x + 300 * sin(camera.dir.y);
	player.worldPos.y = camera.pos.y - 70;
	player.worldPos.z = camera.pos.z + 300 * cos(camera.dir.y);
	player.worldPos.w = 1;

	MatrixRotationY(mRot, camera.dir.y + angle2radian(turning));
	player.Transform(mRot, TRANSFORM_LOCAL_TO_TRANS, true);
	player.ModelToWorld(player.worldPos, TRANSFORM_TRANS_ONLY);
	renderList.InsertObject(player);

	// ground maker
	srand(13);

	for (int x = 0; x < NUM_POINTS_X; ++x)
	{
		for (int z = 0; z < NUM_POINTS_Z; ++z)
		{
			maker.ResetCull();

			maker.worldPos.x = float(RandRange(-100, 100) - UNIVERSE_RADIUS + x * POINT_SIZE);
			maker.worldPos.y = float(maker.maxRadius);
			maker.worldPos.z = float(RandRange(-100, 100) - UNIVERSE_RADIUS + z * POINT_SIZE);

			if (! maker.Cull(camera, Camera::CULL_PLANE_XYZ))
			{
				maker.ModelToWorld(maker.worldPos);
				renderList.InsertObject(maker);
			}
		}
	}

	renderList.RemoveBackfaces(camera);

	renderList.Lighting(camera, lightFactory.GetLights());

	renderList.WorldToCamera(camera);

	renderList.SortByZ();

	renderList.CameraToPerspective(camera);

	renderList.PerspectiveToScreen(camera);
}

void TankAndTower::Render(float fElapsedTime)
{
//	renderList.RenderWire(device);

	renderList.RenderSolid(device);
}

bool TankAndTower::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE:
			tankSpeed += 5;
			break;
			// forward/backward
		case VK_UP:
			camera.pos.x += tankSpeed * sin(camera.dir.y);
			camera.pos.z += tankSpeed * cos(camera.dir.y);
			break;
		case VK_DOWN:
			camera.pos.x -= tankSpeed * sin(camera.dir.y);
			camera.pos.z -= tankSpeed * cos(camera.dir.y);
			break;
			// rotate
		case VK_LEFT:
			camera.dir.y -= angle2radian(3);

			// add a little turn to object
			if ((turning -= 2) < -15)
				turning = -15;
			break;
		case VK_RIGHT:
			camera.dir.y += angle2radian(3);

			// add a little turn to object
			if ((turning += 2) > 15)
				turning = 15;
			break;
		}
		break;
	}
	return DemoApp::MsgProc(hwnd, msg, wParam, lParam);
}


int main()
{
	TankAndTower* app = new TankAndTower;
	app->Create(1440, 900, "tank and tower");
	app->Loop();
	delete app;

	return 0;
}