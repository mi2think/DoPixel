#include "DemoApp.h"

using namespace dopixel;
using namespace dopixel::math;

class GouraudTriangle : public DemoApp
{
public:
	virtual void OnCreate() override;

	virtual void OnUpdate(const Timestep& timestep) override;

	virtual void OnRender(const Timestep& timestep) override;

	virtual bool OnEvent(const Event& event) override;

	bool OnKeyPressEvent(const KeyPressEvent& keyEvent);
private:
	VertexBufferRef vb1_;
	VertexBufferRef vb2_;
	VertexBufferRef vb3_;

	Matrix44f transMatrix_;

	Matrix44f rotateMatrix_;
	float angleX_;
	float angleY_;
	float angleZ_;
	bool rotateX_;
	bool rotateY_;
	bool rotateZ_;
};

void GouraudTriangle::OnCreate()
{
	DemoApp::OnCreate();

	// vb1
	{
		vb1_ = new VertexBuffer;

		// position
		Ref<VertexArray3f> positions(new VertexArray3f(3));
		Vector3f* pos = positions->DataAs<Vector3f>();
		pos[0] = Vector3f(-1.0f, 0.0f, 2.0f);
		pos[1] = Vector3f(0.0f, 1.0f, 2.0f);
		pos[2] = Vector3f(1.0f, 0.0f, 2.0f);
		vb1_->SetPositions(positions);

		// color
		Ref<VertexArray3f> colors(new VertexArray3f(3));
		Vector3f* color = colors->DataAs<Vector3f>();
		color[0] = Vector3f(1, 0, 0);
		color[1] = Vector3f(0, 1, 0);
		color[2] = Vector3f(0, 0, 1);
		vb1_->SetColors(colors);
	}

	// vb2
	{
		vb2_ = new VertexBuffer;

		// position
		Ref<VertexArray3f> positions(new VertexArray3f(3));
		Vector3f* pos = positions->DataAs<Vector3f>();
		pos[0] = Vector3f(-2.25f, -1.0f, 2.0f);
		pos[1] = Vector3f(-1.25f, 0.0f, 2.0f);
		pos[2] = Vector3f(-0.25f, -1.5f, 2.0f);
		vb2_->SetPositions(positions);

		// color
		Ref<VertexArray3f> colors(new VertexArray3f(3));
		Vector3f* color = colors->DataAs<Vector3f>();
		color[0] = Vector3f(1, 0, 0);
		color[1] = Vector3f(0, 1, 0);
		color[2] = Vector3f(0, 0, 1);
		vb2_->SetColors(colors);
	}

	// vb3
	{
		vb3_ = new VertexBuffer;

		// position
		Ref<VertexArray3f> positions(new VertexArray3f(3));
		Vector3f* pos = positions->DataAs<Vector3f>();
		pos[0] = Vector3f(0.25f, -1.5f, 2.0f);
		pos[1] = Vector3f(1.25f, 0.0f, 2.0f);
		pos[2] = Vector3f(2.25f, -1.0f, 2.0f);
		vb3_->SetPositions(positions);

		// color
		Ref<VertexArray3f> colors(new VertexArray3f(3));
		Vector3f* color = colors->DataAs<Vector3f>();
		color[0] = Vector3f(1, 0, 0);
		color[1] = Vector3f(0, 1, 0);
		color[2] = Vector3f(0, 0, 1);
		vb3_->SetColors(colors);
	}


	// camera
	cameraNode_->SetPosition(Vector3f(0, 0, 0));
	renderer_->SetCameraNode(cameraNode_);

	angleX_ = 0.0f;
	angleY_ = 0.0f;
	angleZ_ = 0.0f;
	rotateX_ = false;
	rotateY_ = false;
	rotateZ_ = false;

	transMatrix_.Identity();
	rotateMatrix_.Identity();

	renderer_->SetTransform(Transform::World, transMatrix_);
	renderer_->SetTransform(Transform::View, cameraNode_->GetViewMatrix());
	renderer_->SetTransform(Transform::Projection, cameraNode_->GetProjectionMatrix());

	renderer_->SetCullMode(CullMode::None);
}

void GouraudTriangle::OnUpdate(const Timestep& timestep)
{
	if (rotateX_)
	{
		if (++angleX_ >= 360.0f)
			angleX_ = 0.0f;
		MatrixRotationX(rotateMatrix_, angle2radian(angleX_));
	}
	else if (rotateY_)
	{
		if (++angleY_ >= 360.0f)
			angleY_ = 0.0f;
		MatrixRotationY(rotateMatrix_, angle2radian(angleY_));
	}
	else if (rotateZ_)
	{
		if (++angleZ_ >= 360.0f)
			angleZ_ = 0.0f;
		MatrixRotationZ(rotateMatrix_, angle2radian(angleZ_));
	}
}

void GouraudTriangle::OnRender(const Timestep& timestep)
{
	Matrix44f worldMatrix;
	// draw triangle 1
	{
		MaxtrixTranslation(transMatrix_, Vector3f(-1.25f, 0, 0));
		MatrixMultiply(worldMatrix, rotateMatrix_, transMatrix_);

		renderer_->SetTransform(Transform::World, worldMatrix);
		renderer_->SetShadeMode(ShadeMode::Flat);
		renderer_->DrawPrimitive(vb1_);
	}
	// draw triangle 2
	{
		MaxtrixTranslation(transMatrix_, Vector3f(1.25f, 0, 0));
		MatrixMultiply(worldMatrix, rotateMatrix_, transMatrix_);

		renderer_->SetTransform(Transform::World, worldMatrix);
		renderer_->SetShadeMode(ShadeMode::Gouraud);
		renderer_->DrawPrimitive(vb1_);
	}
	// draw triangle 3 & 4
	{
		MaxtrixTranslation(transMatrix_, Vector3f(0, 0, 0));
		MatrixMultiply(worldMatrix, rotateMatrix_, transMatrix_);

		renderer_->SetTransform(Transform::World, worldMatrix);
		renderer_->SetShadeMode(ShadeMode::Gouraud);
		renderer_->DrawPrimitive(vb2_);

		renderer_->DrawPrimitive(vb3_);
	}
}

bool GouraudTriangle::OnEvent(const Event& event)
{
	EventDispatch dispatch(event);
	dispatch.Dispatch(this, &GouraudTriangle::OnKeyPressEvent);
	return dispatch.GetResult();
}

bool GouraudTriangle::OnKeyPressEvent(const KeyPressEvent& keyEvent)
{
	int key = keyEvent.GetKey();
	rotateX_ = (key == KEY_KEY_X);
	rotateY_ = (key == KEY_KEY_Y);
	rotateZ_ = (key == KEY_KEY_Z);
	return true;
}

int main()
{
	Ref<GouraudTriangle> app(new GouraudTriangle);
	app->Create(640, 480, "Gouraud Triangle");
	app->Loop();

	return 0;
}