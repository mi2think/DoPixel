#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include <GL/glew.h>

#include <list>

#include "DoPixel.h"
#include "DpVector2.h"
#include "DpVector3.h"
#include "DpMatrix44.h"
#include "DpQuaternion.h"
using namespace dopixel;
using namespace dopixel::math;

#define INVALID_UNIFORM		0xffffffff
#define INVALID_OGL_VALUE	0xffffffff

class App
{
protected:
	App();

	void CalcFPS();

	void RenderFPS();

	float GetRunningTime();
private:
	long long frameTime_;
	long long startTime_;
	int frameCount_;
	int fps_;
};

// Call back
class ICallbacks
{
public:
	virtual void KeyboardCB(int key) {}

	virtual void PassiveMouseCB(int x, int y) {}

	virtual void RenderSceneCB() {}

	virtual void MouseCB(int button, int state, int x, int y) {}
};

class Technique
{
public:
	Technique();
	virtual ~Technique();

	virtual bool Init();

	void Enable();
protected:
	bool AddShader(GLenum shaderType, const char* strShader);

	bool AddShaderFromFile(GLenum shaderType, const char* fileName);

	bool Finalize();

	GLint GetUniformLocation(const char* uniformName);

	GLint GetProgramParam(GLint param);

	GLuint shaderProg_;
private:
	typedef std::list<GLuint> ShaderObjList;
	ShaderObjList shaderObjList_;
};

class Texture
{
public:
	Texture(GLenum textureTarget, const CString& fileName);
	~Texture();

	bool Load();

	void Bind(GLenum textureUnit);
private:
	CString fileName_;
	int width_;
	int height_;
	unsigned char* data_;

	GLenum textureTarget_;
	GLuint textureObj_;
};

struct aiScene;
struct aiMesh;
class Mesh
{
public:
	struct Vertex
	{
		Vector3f pos;
		Vector2f tex;
		Vector3f normal;

		Vertex() {}
		Vertex(const Vector3f& _pos, const Vector2f& _tex, const Vector3f& _normal)
			: pos(_pos)
			, tex(_tex)
			, normal(_normal)
		{}
	};

	Mesh();
	~Mesh();

	bool LoadMesh(const CString& fileName);
	
	void Render();
private:
	bool InitFromScene(const aiScene* scene, const CString& fileName);
	void InitMesh(unsigned int index, const aiMesh* mesh);
	bool InitMaterials(const aiScene* scene, const CString& fileName);
	void Clear();

#define  INVALID_MATERIAL 0xffffffff

	struct MeshEntry 
	{
		MeshEntry();
		~MeshEntry();
		
		void Init(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices);
		GLuint VB_;
		GLuint IB_;
		unsigned int numIndices_;
		unsigned int materialIndex_;
	};

	std::vector<MeshEntry> entries_;
	std::vector<Texture*> textures_;
};

namespace basiclighting
{
	struct BaseLight
	{
		Vector3f color_;
		float ambientIntensity_;
		float diffuseIntensity_;

		BaseLight()
			: color_(0, 0, 0)
			, ambientIntensity_(0)
			, diffuseIntensity_(0)
		{}
	};

	struct DirectionalLight : public BaseLight
	{
		Vector3f direction_;

		DirectionalLight()
			: direction_(0, 0, 0)
		{}
	};

	struct PointLight : public BaseLight
	{
		struct Attenuation
		{
			float kc;
			float kl;
			float kq;
			Attenuation()
				: kc(1)
				, kl(0)
				, kq(0)
			{}
		};

		Vector3f position_;
		Attenuation attenuation_;

		PointLight()
			: position_(0, 0, 0)
		{}
	};

	struct SpotLight : public PointLight
	{
		Vector3f direction_;
		float theta_;
		float phi_;
		float pf_;

		SpotLight()
			: direction_(0, 0, 0)
			, theta_(0)
			, phi_(0)
			, pf_(1)
		{}
	};

	class BasicLightingTechnique : public Technique
	{
	public:
		static const unsigned int MAX_POINT_LIGHTS = 2;
		static const unsigned int MAX_SPOT_LIGHTS = 2;

		BasicLightingTechnique() {}

		virtual bool Init() override;

		void SetWVP(const Matrix44f& WVP);
		void SetWorldMatrix(const Matrix44f& worldMatrix);
		void SetTextureUnit(unsigned int textureUnit);
		void SetEyeWorldPos(const Vector3f& eyeWorldPos);
		void SetSpecularIntensity(float specularIntensity);
		void SetSpecularPower(float power);
		void SetDirectionalLight(const DirectionalLight& light);
		void SetPointLights(unsigned int numLights, const PointLight* pLights);
		void SetSpotLights(unsigned int numLights, const SpotLight* pLights);
	private:
		GLuint WVPLocation_;
		GLuint worldMatrixLocation_;
		GLuint samplerLocation_;
		GLuint eyeWorldPosLocation_;
		GLuint specularIntensityLocation_;
		GLuint specularPowerLocation_;

		GLuint numPointLightsLocation_;
		GLuint numSpotLightsLocation_;

		struct
		{
			GLuint color;
			GLuint ambientIntensity;
			GLuint diffuseIntensity;
			GLuint direction;
		} directionalLight_;

		struct
		{
			GLuint color;
			GLuint ambientIntensity;
			GLuint diffuseIntensity;
			GLuint position;
			struct
			{
				GLuint kc;
				GLuint kl;
				GLuint kq;
			} atten;
		} pointLightsLocation_[MAX_POINT_LIGHTS];

		struct
		{
			GLuint color;
			GLuint ambientIntensity;
			GLuint diffuseIntensity;
			GLuint position;
			struct
			{
				GLuint kc;
				GLuint kl;
				GLuint kq;
			} atten;

			GLuint direction;
			GLuint cosThetaOver2;
			GLuint cosPhiOver2;
			GLuint pf;
		} spotLightsLocation_[MAX_SPOT_LIGHTS];
	};
}

// FPS camera 
class Camera
{
public:
	Camera(int windowWidth, int windowHight);
	Camera(const Vector3f& pos, const Vector3f& target, const Vector3f& up, int windowWidth, int windowHight);

	bool OnKayboard(int key);
	void OnMouse(int x, int y);

	void Update();
	void OnRender();

	void SetStepScale(float stepScale);

	const Vector3f& GetPosition() const { return position_; }
	const Vector3f& GetTarget() const { return target_; }
	const Vector3f& GetUp() const { return up_; }
private:
	void Init();

	Vector3f position_;
	Vector3f target_;
	Vector3f up_;

	float stepScale_;

	Vector2i mousePos_;
	float angleH_;
	float angleV_;

	bool onLEdge_;	// on left edge
	bool onREdge_;	// on right edge
	bool onTEdge_;	// on top edge
	bool onBEdge_;	// on bottom edge

	int windowWidth_;
	int windowHeight_;

	bool needUpdate_;
};


struct PersProjInfo
{
	float FOV;
	float Width;
	float Height;
	float zNear;
	float zFar;
};

class Pipeline
{
public:
	Pipeline();

	void Scale(float s);
	void Scale(float x, float y, float z);
	void Scale(const Vector3f& scale);
	
	void WorldPos(float x, float y, float z);
	void WorldPos(const Vector3f& pos);
	
	void Rotate(float x, float y, float z);
	void Rotate(const Vector3f& rorate);

	void SetPerspectiveProj(const PersProjInfo& p);

	void SetCamera(const Camera& camera);

	// world
	const Matrix44f& GetOGLWorldTrans();
	// view
	const Matrix44f& GetOGLViewTrans();
	// projection
	const Matrix44f& GetOGLProjTrans();

	// world projection
	const Matrix44f& GetOGLWorldProjTrans();
	// world view projection
	const Matrix44f& GetOGLWorldViewProjTrans();
private:
	Vector3f scale_;
	Vector3f position_;
	Vector3f rotate_;

	PersProjInfo persProjInfo_;
	const Camera* camera_;

	Matrix44f worldTrans_;
	Matrix44f viewTrans_;
	Matrix44f projTrans_;

	Matrix44f worldProjTrans_;
	Matrix44f worldViewProjTrans_;
};

//////////////////////////////////////////////////////////////////////////
//functions

void GLUTBackendInit(int argc, char** argv, bool withDepth, bool withStencil);
bool GLUTBackendCreateWindow(unsigned int width, unsigned int height, bool fullScreen, const char* title);
void GLUTBackendRun(ICallbacks* callbacks);
void GLUTBackendSwapBuffers();
void GLUTBackendLeaveMainLoop();

#endif
