#include "D3DDemoApp.h"

struct ColorVertex
{
	ColorVertex(float _x, float _y, float _z, D3DCOLOR _color)
		: x(_x)
		, y(_y)
		, z(_z)
		, color(_color)
	{
	}

	float x, y, z;
	D3DCOLOR color;
	static const DWORD FVF;
};

const DWORD ColorVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;


class GouraudTriangle : public D3DDemoApp
{
public:
	GouraudTriangle() : vertexBuffer(nullptr) {}

	void OnCreate();

	void Run(float fElapsedTime);

	void Render(float fElapsedTime);

	void Release();
private:
	IDirect3DVertexBuffer9*  vertexBuffer;
	D3DXMATRIX worldMatrix;
};

void GouraudTriangle::Run(float fElapsedTime)
{

}

void GouraudTriangle::OnCreate()
{
	// Create vertex buffer
	device->CreateVertexBuffer(12 * sizeof(ColorVertex), D3DUSAGE_WRITEONLY, ColorVertex::FVF, D3DPOOL_MANAGED, &vertexBuffer, nullptr);

	// Fill buffer with triangle data
	ColorVertex* v;
	vertexBuffer->Lock(0, 0, (void**)&v, 0);

	v[0] = ColorVertex(-2.25f, 0.0f, 2.0f, D3DCOLOR_XRGB(255, 0, 0));
	v[1] = ColorVertex(-1.25f, 1.0f, 2.0f, D3DCOLOR_XRGB(0, 255, 0));
	v[2] = ColorVertex(-0.25f, 0.0f, 2.0f, D3DCOLOR_XRGB(0, 0, 255));

	v[3] = ColorVertex(0.25f, 0.0f, 2.0f, D3DCOLOR_XRGB(255, 0, 0));
	v[4] = ColorVertex(1.25f, 1.0f, 2.0f, D3DCOLOR_XRGB(0, 255, 0));
	v[5] = ColorVertex(2.25f, 0.0f, 2.0f, D3DCOLOR_XRGB(0, 0, 255));

	v[6] = ColorVertex(-2.25f, -1.0f, 2.0f, D3DCOLOR_XRGB(255, 0, 0));
	v[7] = ColorVertex(-1.25f,  0.0f, 2.0f, D3DCOLOR_XRGB(0, 255, 0));
	v[8] = ColorVertex(-0.25f, -1.5f, 2.0f, D3DCOLOR_XRGB(0, 0, 255));

	v[9]  = ColorVertex(0.25f, -1.5f, 2.0f, D3DCOLOR_XRGB(255, 0, 0));
	v[10] = ColorVertex(1.25f,  0.0f, 2.0f, D3DCOLOR_XRGB(0, 255, 0));
	v[11] = ColorVertex(2.25f, -1.0f, 2.0f, D3DCOLOR_XRGB(0, 0, 255));

	vertexBuffer->Unlock();

	// Set the view matrix
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 0, 1), &D3DXVECTOR3(0, 1, 0));
	device->SetTransform(D3DTS_VIEW, &view);

	// Set the projection matrix
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float)clientWidth / (float)clientHeight, 1.0f, 1000.0f);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	// Turn off lighting
	device->SetRenderState(D3DRS_LIGHTING, false);
}

void GouraudTriangle::Render(float fElapsedTime)
{
	device->SetFVF(ColorVertex::FVF);

	device->SetStreamSource(0, vertexBuffer, 0, sizeof(ColorVertex));

	D3DXMatrixTranslation(&worldMatrix, 0.0f, 0.0f, 0.0f);
	device->SetTransform(D3DTS_WORLD, &worldMatrix);

	// draw the triangle with flat shading
	device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

	// draw the triangle with grouraud shading
	device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	device->DrawPrimitive(D3DPT_TRIANGLELIST, 3, 3);
}

void GouraudTriangle::Release()
{
	SAFE_RELEASE(vertexBuffer);
}

int main()
{
	GouraudTriangle app;
	app.Create(640, 480, "Gouraud Triangle");
	app.Loop();

	return 0;
}