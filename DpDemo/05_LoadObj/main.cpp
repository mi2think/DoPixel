#include "DemoApp.h"

using namespace dopixel;
using namespace dopixel::math;

class LoadObjApp : public DemoApp
{
public:
	LoadObjApp() {}

	virtual void OnCreate() override;

	virtual void OnRender(const Timestep& timestep) override;

	virtual bool OnEvent(const Event& event) override;

	bool OnKeyPressEvent(const KeyPressEvent& keyEvent);
private:
	MeshRef mesh_;
	Vector3f cameraPos_;
};

void LoadObjApp::OnCreate()
{
	DemoApp::OnCreate();

	mesh_ = new Mesh("box.obj");

	// camera
	cameraPos_ = Vector3f(0, 2, -1);
	cameraNode_->SetPosition(cameraPos_);
	math::Matrix44f mat;
	mat.Identity();
	mat.SetTranslation(math::Vector3f(0, 0, 1));
	renderer_->SetTransform(Transform::World, mat);
	renderer_->SetTransform(Transform::View, cameraNode_->GetViewMatrix());
	renderer_->SetTransform(Transform::Projection, cameraNode_->GetProjectionMatrix());

	renderer_->SetZBufferType(ZBuffer::INVZBuff);

	renderer_->SetShadeMode(ShadeMode::Flat);
	renderer_->SetCameraNode(cameraNode_);
}

void LoadObjApp::OnRender(const Timestep& timestep)
{
	mesh_->OnRender(renderer_);

	renderer_->SetShadeMode(ShadeMode::Wireframe);
	mesh_->OnRender(renderer_);
	renderer_->SetShadeMode(ShadeMode::Flat);
}

bool LoadObjApp::OnEvent(const Event& event)
{
	EventDispatch dispatch(event);
	dispatch.Dispatch(this, &LoadObjApp::OnKeyPressEvent);
	return dispatch.GetResult();
}

bool LoadObjApp::OnKeyPressEvent(const KeyPressEvent& keyEvent)
{
	int key = keyEvent.GetKey();

	{
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
			cameraNode_->SetPosition(cameraPos_);
			renderer_->SetTransform(Transform::View, cameraNode_->GetViewMatrix());
			renderer_->SetTransform(Transform::Projection, cameraNode_->GetProjectionMatrix());
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


