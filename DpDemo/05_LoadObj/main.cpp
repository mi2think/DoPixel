#include "DemoApp.h"

using namespace dopixel;
using namespace dopixel::math;

class LoadObjApp : public DemoApp
{
public:
	LoadObjApp() {}

	virtual void OnCreate() override;

	virtual void OnUpdate(const Timestep& timestep) override;

	virtual void OnRender(const Timestep& timestep) override;

	virtual bool OnEvent(const Event& event) override;

	bool OnKeyPressEvent(const KeyPressEvent& keyEvent);
private:
	MeshRef mesh_;
	Vector3f cameraPos_;
	Vector3f transPos_;
	ArcBall arcBall_;
	bool update_;
};

void LoadObjApp::OnCreate()
{
	DemoApp::OnCreate();

	mesh_ = new Mesh("box.obj");

	arcBall_.SetWidth(width_);
	arcBall_.SetHeight(height_);

	update_ = false;

	// camera
	cameraPos_ = Vector3f(0, 0, -3);
	cameraController_ = new ModelViewCameraController();
	cameraController_->Attach(camera_);
	cameraController_->SetWindow(width_, height_);
	cameraController_->SetView(cameraPos_, Vector3f(0, 0, 0), Vector3f(0, 1, 0));

	transPos_ = math::Vector3f(0, 0, 0);
	math::Matrix44f mat;
	math::MaxtrixTranslation(mat, transPos_);
	renderer_->SetTransform(Transform::World, mat);	
	renderer_->SetTransform(Transform::View, cameraController_->GetViewMatrix());
	renderer_->SetTransform(Transform::Projection, cameraController_->GetProjMatrix());

	renderer_->SetZBufferType(ZBuffer::INVZBuff);

	renderer_->SetShadeMode(ShadeMode::Flat);
	renderer_->SetCameraController(cameraController_);
}

void LoadObjApp::OnUpdate(const Timestep& timestep)
{
	cameraController_->OnUpdate(timestep);
	renderer_->SetTransform(Transform::View, cameraController_->GetViewMatrix());
}

void LoadObjApp::OnRender(const Timestep& timestep)
{
	mesh_->OnRender(renderer_);

}

bool LoadObjApp::OnEvent(const Event& event)
{
	EventDispatch dispatch(event);
	dispatch.Dispatch(this, &LoadObjApp::OnKeyPressEvent);

	return cameraController_->OnEvent(event);
}

bool LoadObjApp::OnKeyPressEvent(const KeyPressEvent& keyEvent)
{
	int key = keyEvent.GetKey();

	{
		cameraPos_ = cameraController_->GetEyePos();
		bool update = false;
		switch (key)
		{
			// move on axis
			// z
		case KEY_KEY_W:
			cameraPos_.z += 0.05f;
			update = true;
			break;
		case KEY_KEY_S:
			cameraPos_.z -= 0.05f;
			update = true;
			break;
			// x
		case KEY_KEY_A:
			cameraPos_.x -= 0.05f;
			update = true;
			break;
		case KEY_KEY_D:
			cameraPos_.x += 0.05f;
			update = true;
			break;
			// y
		case KEY_KEY_Q:
			cameraPos_.y += 0.05f;
			update = true;
			break;
		case KEY_KEY_E:
			cameraPos_.y -= 0.05f;
			update = true;
			break;

			// render to image
		case KEY_KEY_R:
			{
				ImageRef snapshot(new Image(width_, height_, renderer_->GetPixelFormat()));
				renderer_->CopyTexImage(snapshot);
				snapshot->SaveTGA("fb.tga");
			}
			break;
		}

		if (update)
		{
			cameraController_->SetEyePos(cameraPos_);

			LOG_INFO("eye:%f,%f,%f", cameraPos_.x, cameraPos_.y, cameraPos_.z);
		}
	}

	return true;
}


int main()
{
	Ref<LoadObjApp> app(new LoadObjApp);
	app->Create(512, 512, "Load Obj");
	app->Loop();

	return 0;
}


