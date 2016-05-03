#include "DemoApp.h"

using namespace dopixel;
using namespace dopixel::math;

class LightingCubeApp : public DemoApp
{
public:
	LightingCubeApp() {}

	virtual void OnCreate() override;

	virtual void OnUpdate(const Timestep& timestep) override;

	virtual void OnRender(const Timestep& timestep) override;

	virtual bool OnEvent(const Event& event) override;

	bool OnKeyPressEvent(const KeyPressEvent& keyEvent);
private:
	MeshRef cube_;
	Vector3f cameraPos_;
};

void LightingCubeApp::OnCreate()
{
	DemoApp::OnCreate();

	cube_ = new Mesh("cube/cube.obj");

	// camera
	cameraPos_ = Vector3f(0, 0, -5);
	cameraController_ = new ModelViewCameraController();
	cameraController_->Attach(camera_);
	cameraController_->SetWindow(width_, height_);
	cameraController_->SetView(cameraPos_, Vector3f(0, 0, 0), Vector3f(0, 1, 0));
	cameraController_->SetPerspective(45, (float)width_ / height_, 1, 5000);
	renderer_->SetCameraController(cameraController_);
	renderer_->SetTransform(Transform::View, cameraController_->GetViewMatrix());
	renderer_->SetTransform(Transform::Projection, cameraController_->GetProjMatrix());
	renderer_->SetZBufferType(ZBuffer::INVZBuff);
	renderer_->SetShadeMode(ShadeMode::Gouraud);

	// light
	LightRef directionalLight(new DirectionalLight(Vector3f(1, -1, 1), Vector3f(0.2, 0.2, 0.2), Vector3f(1, 1, 1), Vector3f(1, 1, 1)));
	renderer_->SetLight(0, directionalLight);
	renderer_->EnableLight(0, true);
}

void LightingCubeApp::OnUpdate(const Timestep& timestep)
{
	cameraController_->OnUpdate(timestep);
	renderer_->SetTransform(Transform::View, cameraController_->GetViewMatrix());
}

void LightingCubeApp::OnRender(const Timestep& timestep)
{
	cube_->OnRender(renderer_);
}

bool LightingCubeApp::OnEvent(const Event& event)
{
	EventDispatch dispatch(event);
	dispatch.Dispatch(this, &LightingCubeApp::OnKeyPressEvent);

	return cameraController_->OnEvent(event);
}

bool LightingCubeApp::OnKeyPressEvent(const KeyPressEvent& keyEvent)
{
	int key = keyEvent.GetKey();
	bool enbale = false;
	switch (key)
	{
	case KEY_KEY_0:
		enbale = renderer_->GetLightEnable(0);
		renderer_->EnableLight(0, !enbale);
		break;
	}
	return true;
}

int main()
{
	Ref<LightingCubeApp> app(new LightingCubeApp);
	app->Create(512, 512, "Load Obj");
	app->Loop();

	return 0;
}

