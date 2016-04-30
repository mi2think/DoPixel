#include "DemoApp.h"

using namespace dopixel;
using namespace dopixel::math;

class WireframeTriangle : public DemoApp
{
public:
	virtual void OnCreate() override;

	virtual void OnUpdate(const Timestep& timestep) override;

	virtual void OnRender(const Timestep& timestep) override;
private:
	VertexBufferRef vb_;
	Matrix44f transMatrix_;
	float angleY_;
};

void WireframeTriangle::OnCreate()
{
	DemoApp::OnCreate();

	vb_ = new VertexBuffer;
	
	// position
	Ref<VertexArray3f> positions(new VertexArray3f(3));
	Vector3f* pos = positions->DataAs<Vector3f>();
	pos[0] = Vector3f(0, 50, 0);
	pos[1] = Vector3f(50, -50, 0);
	pos[2] = Vector3f(-50, -50, 0);
	vb_->SetPositions(positions);

	// color
	Ref<VertexArray3f> colors(new VertexArray3f(3));
	Vector3f* color = colors->DataAs<Vector3f>();
	color[0] = Vector3f(1, 0, 0);
	color[1] = Vector3f(1, 0, 0);
	color[2] = Vector3f(1, 0, 0);
	vb_->SetColors(colors);

	// camera
	cameraController_ = new ModelViewCameraController();
	cameraController_->Attach(camera_);
	cameraController_->SetWindow(width_, height_);
	cameraController_->SetView(Vector3f(0, 0, -100), Vector3f(0, 0, 0), Vector3f(0, 1, 0));

	renderer_->SetShadeMode(ShadeMode::Wireframe);
	renderer_->SetCameraController(cameraController_);

	angleY_ = 0.0f;
	MaxtrixTranslation(transMatrix_, Vector3f(0, 0, 100));

	renderer_->SetTransform(Transform::World, transMatrix_);
	renderer_->SetTransform(Transform::View, cameraController_->GetViewMatrix());
	renderer_->SetTransform(Transform::Projection, cameraController_->GetProjMatrix());

	renderer_->SetCullMode(CullMode::None);
}

void WireframeTriangle::OnUpdate(const Timestep& timestep)
{
	if (++angleY_ >= 360.0f)
		angleY_ = 0.0f;

	Matrix44f rotMatrix;
	MatrixRotationY(rotMatrix, angle2radian(angleY_));

	Matrix44f worldMatrix;
	MatrixMultiply(worldMatrix, rotMatrix, transMatrix_);

	renderer_->SetTransform(Transform::World, worldMatrix);
}

void WireframeTriangle::OnRender(const Timestep& timestep)
{
	renderer_->DrawPrimitive(vb_);
}

int main()
{
	Ref<WireframeTriangle> app(new WireframeTriangle);
	app->Create(400, 400, "Wireframe Triangle");
	app->Loop();

	return 0;
}