#include "D3DDemoApp.h"

struct Vertex
{
	Vertex(float _x, float _y, float _z, float _u, float _v)
		: x(_x)
		, y(_y)
		, z(_z)
		, u(_u)
		, v(_v)
	{
	}
	float x, y, z;
	float u, v;
	static const DWORD FVF;
};

const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

class TextureCube : public D3DDemoApp
{
public:
	TextureCube() 
		: vertexBuffer(nullptr)
		, texture(nullptr)
		, xMove(0)
		, yMove(0)
		, zMove(-3)
	{}

	void OnCreate();

	void Run(float fElapsedTime);

	void Render(float fElapsedTime);

	bool MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void Release();
private:
	IDirect3DVertexBuffer9* vertexBuffer;
	IDirect3DTexture9* texture;

	float xMove;
	float yMove;
	float zMove;
};

void TextureCube::OnCreate()
{
	device->CreateVertexBuffer(36 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &vertexBuffer, nullptr);

	Vertex* v;
	vertexBuffer->Lock(0, 0, (void**)&v, 0);

	//      F---------G
	//     /|        /|
	//    / |       / |
	//   B---------C  |
	//   |  E------|--H
	//   | /       | /
	//   |/        |/
	//   A---------D
	
	// A: -1.0f, -1.0f, -1.0f
	// B: -1.0f,  1.0f, -1.0f
	// C:  1.0f,  1.0f, -1.0f
	// D:  1.0f, -1.0f, -1.0f
	// E: -1.0f, -1.0f,  1.0f
	// F: -1.0f,  1.0f,  1.0f
	// G:  1.0f,  1.0f,  1.0f
	// H:  1.0f, -1.0f,  1.0f

	// front
	// ABC
	v[0] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f);
	// ACD
	v[3] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f);
	v[4] = Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f);
	v[5] = Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f);
	
	// back
	// EFG
	v[6] = Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f);
	v[7]= Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f);
	v[8] = Vertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f);
	// EGH
	v[9] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
	v[10] = Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	v[11] = Vertex(1.0f, -1.0f, 1.0f, 1.0f, 1.0f);
	
	// left
	// EFB
	v[12] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
	v[13] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
	v[14] = Vertex(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f);
	// EBA
	v[15] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
	v[16] = Vertex(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f);
	v[17] = Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f);

	// right
	// DCG
	v[18] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	// DGH
	v[21] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	v[23] = Vertex(1.0f, -1.0f, 1.0f, 1.0f, 1.0f);

	// top
	// BFG
	v[24] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f);
	v[25] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
	v[26] = Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	// BGC
	v[27] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f);
	v[28] = Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	v[29] = Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f);

	// bottom
	// EAD
	v[30] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
	v[31] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f);
	v[32] = Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f);
	// EDH
	v[33] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
	v[34] = Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f);
	v[35] = Vertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f);

	vertexBuffer->Unlock();

	// set view matrix
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0.0f, 0.0f, -3.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	device->SetTransform(D3DTS_VIEW, &view);

	// set project matrix
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float)clientWidth / (float)clientHeight, 1.0f, 1000.0f);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	// Turn off lighting
	device->SetRenderState(D3DRS_LIGHTING, false);

	// Texture
	texture = GetTextureAlloc()->CreateTexture("crate.jpg");
}

void TextureCube::Run(float fElapsedTime)
{
	//D3DXVECTOR3 position(xMove, yMove, zMove);
	//D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	//D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	//D3DXMATRIX view;
	//D3DXMatrixLookAtLH(&view, &position, &target, &up);

	//device->SetTransform(D3DTS_VIEW, &view);

	// 
	// Update the scene: update camera position.
	//

	static float angle = (3.0f * D3DX_PI) / 2.0f;
	static float height = 2.0f;

	if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
		angle -= 0.5f * fElapsedTime;

	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
		angle += 0.5f * fElapsedTime;

	if (::GetAsyncKeyState(VK_UP) & 0x8000f)
		height += 5.0f * fElapsedTime;

	if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
		height -= 5.0f * fElapsedTime;

	D3DXVECTOR3 position(cosf(angle) * 3.0f, height, sinf(angle) * 3.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);

	device->SetTransform(D3DTS_VIEW, &V);
}

bool TextureCube::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CHAR:
		{
			switch (wParam)
			{
			case 'x':
				xMove += 1;
				break;
			case 'y':
				yMove += 1;
				break;
			case 'z':
				zMove += 1;
				break;
			case 'c':
				xMove = 0;
				yMove = 0;
				zMove = -3;
				break;
			}
		}
		break;
	}

	return D3DDemoApp::MsgProc(hwnd, msg, wParam, lParam);
}

void TextureCube::Render(float fElapsedTime)
{
	device->SetFVF(Vertex::FVF);

	device->SetStreamSource(0, vertexBuffer, 0, sizeof(Vertex));

	device->SetTexture(0, texture);

	device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);
}

void TextureCube::Release()
{
	SAFE_RELEASE(vertexBuffer);
}

int main()
{
	TextureCube app;
	app.Create(640, 480, "Texture Cube");
	app.Loop();
	return 0;
}