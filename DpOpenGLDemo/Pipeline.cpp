#include "Pipeline.h"

#include "DpFileStream.h"
#include "stb/stb_image.h"

#include <GL/freeglut.h> // for key define

#pragma comment(lib, "DpLib.lib")

long long GetCurrentTimeMillis()
{
	return GetTickCount();
}

App::App()
	: frameCount_(0)
	, frameTime_(0)
	, fps_(0)
{
	frameTime_ = startTime_ = GetCurrentTimeMillis();
}

void App::CalcFPS()
{
	++frameCount_;

	long long time = GetCurrentTimeMillis();
	if (time > frameTime_ + 1000)
	{
		frameTime_ = time;
		fps_ = frameCount_;
		frameCount_ = 0;
	}
}

void App::RenderFPS()
{

}

float App::GetRunningTime()
{
	float runningTime = (float)((double)GetCurrentTimeMillis() - (double)startTime_) / 1000.0f;
	return runningTime;
}

//////////////////////////////////////////////////////////////////////////
Technique::Technique()
	: shaderProg_(0)
{

}

Technique::~Technique()
{
	for (auto shaderObj : shaderObjList_)
	{
		glDeleteShader(shaderObj);
	}

	if (shaderProg_ != 0)
	{
		glDeleteProgram(shaderProg_);
		shaderProg_ = 0;
	}
}

bool Technique::Init()
{
	shaderProg_ = glCreateProgram();

	if (shaderProg_ == 0)
	{
		fprintf(stderr, "error create shader program\n");
		return false;
	}
	return true;
}

bool Technique::AddShader(GLenum shaderType, const char* strShader)
{
	GLuint shaderObj = glCreateShader(shaderType);
	if (shaderObj == 0)
	{
		fprintf(stderr, "error create shader type:%d\n", shaderType);
		return false;
	}

	shaderObjList_.push_back(shaderObj);

	const GLchar* p[1] = { strShader };
	GLint lengths[1] = { (GLint)strlen(strShader) };

	glShaderSource(shaderObj, 1, p, lengths);
	glCompileShader(shaderObj);
	GLint success;
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[1024];
		glGetShaderInfoLog(shaderObj, sizeof(infoLog), nullptr, infoLog);
		fprintf(stderr, "error compile shader:\n%s\n", infoLog);
		return false;
	}

	glAttachShader(shaderProg_, shaderObj);

	return true;
}

bool Technique::AddShaderFromFile(GLenum shaderType, const char* fileName)
{
	dopixel::core::FileStream fs(fileName, dopixel::core::FileStream::TextRead);
	size_t size = (size_t)fs.Size();
	char* fileData = new char[size + 1];
	dopixel::core::ON_SCOPE_EXIT([&fileData]() { SAFE_DELETEARRAY(fileData); });

	if (fileData)
	{
		size_t readSize = fs.Read(fileData, size);
		fileData[readSize] = 0;
#ifdef _DEBUG
		if (shaderType == GL_VERTEX_SHADER)
			fprintf(stdout, "vs:\n");
		else
			fprintf(stdout, "ps:\n");
		fprintf(stdout, "%s", fileData);
#endif // _DEBUG

		return AddShader(shaderType, fileData);
	}
	return false;
}

bool Technique::Finalize()
{
	GLint success = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shaderProg_);
	glGetProgramiv(shaderProg_, GL_LINK_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(shaderProg_, sizeof(errorLog), nullptr, errorLog);
		fprintf(stderr, "error link shader program:\n%s\n", errorLog);
		return false;
	}

	glValidateProgram(shaderProg_);
	glGetProgramiv(shaderProg_, GL_VALIDATE_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(shaderProg_, sizeof(errorLog), nullptr, errorLog);
		fprintf(stderr, "error validate shader program:\n%s\n", errorLog);
		return false;
	}

	// delete shader objects
	for (auto shaderObj : shaderObjList_)
	{
		glDeleteShader(shaderObj);
	}
	shaderObjList_.clear();

	return glGetError() == GL_NO_ERROR;
}

void Technique::Enable()
{
	glUseProgram(shaderProg_);
}

GLint Technique::GetUniformLocation(const char* uniformName)
{
	GLuint location = glGetUniformLocation(shaderProg_, uniformName);
	if (location == 0xffffffff)
	{
		fprintf(stderr, "warning! unable to get location of uniform:'%s'\n", uniformName);
	}
	return location;
}

GLint Technique::GetProgramParam(GLint param)
{
	GLint ret;
	glGetProgramiv(shaderProg_, param, &ret);
	return ret;
}
//////////////////////////////////////////////////////////////////////////

Texture::Texture(GLenum textureTarget, const CString& fileName)
	: textureTarget_(textureTarget)
	, fileName_(fileName)
	, width_(0)
	, height_(0)
	, data_(nullptr)
{
	free(data_);
}

bool Texture::Load()
{
	dopixel::core::FileStream fs(fileName_.c_str(), dopixel::core::FileStream::BinaryRead);
	size_t size = (size_t)fs.Size();
	unsigned char* fileData = new unsigned char[size];
	dopixel::core::ON_SCOPE_EXIT([&fileData]() { SAFE_DELETEARRAY(fileData); });

	if (fileData)
	{
		size_t readSize = fs.Read(fileData, size);
		ASSERT(readSize == size);

		stbi_set_flip_vertically_on_load(true);

		int comp = 0;
		data_ = stbi_load_from_memory(fileData, size, &width_, &height_, &comp, 4);

		glGenTextures(1, &textureObj_);
		glBindTexture(textureTarget_, textureObj_);
		glTexImage2D(textureTarget_, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_);
		glTexParameterf(textureTarget_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(textureTarget_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(textureTarget_, 0);

		stbi_set_flip_vertically_on_load(false);
		return true;
	}
	return false;
}

void Texture::Bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(textureTarget_, textureObj_);
}

//////////////////////////////////////////////////////////////////////////

#define MARGIN 10
#define EDGE_STEP 0.5f

Camera::Camera(int windowWidth, int windowHight)
	: position_(0, 0, 0)
	, target_(0, 0, 1)
	, up_(0, 1, 0)
	, stepScale_(1.0f)
	, windowWidth_(windowWidth)
	, windowHeight_(windowHight)
	, onLEdge_(false)
	, onREdge_(false)
	, onTEdge_(false)
	, onBEdge_(false)
	, needUpdate_(false)
{
	target_.Normalize();
	up_.Normalize();
	Init();
}

Camera::Camera(const Vector3f& pos, const Vector3f& target, const Vector3f& up, int windowWidth, int windowHight)
	: position_(pos)
	, target_(target)
	, up_(up)
	, stepScale_(1.0f)
	, windowWidth_(windowWidth)
	, windowHeight_(windowHight)
	, onLEdge_(false)
	, onREdge_(false)
	, onTEdge_(false)
	, onBEdge_(false)
{
	target_.Normalize();
	up_.Normalize();
	Init();
}

void Camera::Init()
{
	// horizontal angle define by the following circle
	//         90
	//          |
	// 180 -----|----- x 0
	//          |
	//          z
	//         270

	// r = 1
	// z = rsin(angle)
	// x = rcos(angle)

	// when we rotate 90 from (1, 0, 0) around (0, 1, 0), using quaternion will get (0, 0, -1)

	// asin: [-90, 90], when z in [-1, 1]
	Vector3f htarget = target_;
	htarget.y = 0;
	htarget.Normalize();

	if (htarget.z >= 0.0f)
	{
		if (htarget.x >= 0.0f )
		{
			// angle should in [270, 360]
			angleH_ = 360 - radian2angle(asin(htarget.z));
		}
		else
		{
			// angle should in [180, 270]
			angleH_ = 180 + radian2angle(asin(htarget.z));
		}
	}
	else
	{
		if (htarget.x >= 0.0f)
		{
			// angle should in [0, 90]
			angleH_ = radian2angle(asin(-htarget.z));
		}
		else
		{
			// angle should in [90, 180]
			angleH_ = 90 + radian2angle(asin(-htarget.z));
		}
	}

	// vertical angle we define when (up, down) -> [-90, 90]
	angleV_ = -radian2angle(asin(target_.y));

	// init mouse pos to center of window
	mousePos_.x = windowWidth_ / 2;
	mousePos_.y = windowHeight_ / 2;
}

bool Camera::OnKayboard(int key)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		position_ += target_ * stepScale_;
		break;
	case GLUT_KEY_DOWN:
		position_ -= target_ * stepScale_;
		break;
	case GLUT_KEY_LEFT:
		{
			Vector3f left = CrossProduct(target_, up_);
			left.Normalize();
			position_ += left * stepScale_;
		}
		break;
	case GLUT_KEY_RIGHT:
		{
			Vector3f right = CrossProduct(up_, target_);
			right.Normalize();
			position_ += right * stepScale_;
		}
		break;
	default:
		break;
	}
	return false;
}

void Camera::OnMouse(int x, int y)
{
	const int deltaX = x - mousePos_.x;
	const int deltaY = y - mousePos_.y;

	mousePos_.x = x;
	mousePos_.y = y;

	angleH_ += (float)deltaX / 20.0f;
	angleV_ += (float)deltaY / 20.0f;

	if (deltaX == 0)
	{
		if (x <= MARGIN)
			onLEdge_ = true;
		else if (x >= windowWidth_ - MARGIN)
			onREdge_ = true;
	}
	else
	{
		onLEdge_ = false;
		onREdge_ = false;
	}

	if (deltaY == 0)
	{
		if (y <= MARGIN)
			onTEdge_ = true;
		else if (y >= windowHeight_ - MARGIN)
			onBEdge_ = true;
	}
	else
	{
		onTEdge_ = false;
		onBEdge_ = false;
	}

	Update();
}

void Camera::Update()
{
	// vertical axis
	const Vector3f VAxis(0.0f, 1.0f, 0.0f);

	// rotate the view vector by the horizontal angle around the vertical axis
	Vector3f view(1.0f, 0.0f, 0.0f);	
	view = QuaternionRotateVector(view, VAxis, angle2radian(angleH_));

	// rotate the view vector by the vertical angle around the horizontal axis
	Vector3f HAxis = CrossProduct(VAxis, view);
	HAxis.Normalize();
	view = QuaternionRotateVector(view, HAxis, angle2radian(angleV_));

	target_ = view;
	target_.Normalize();

	up_ = CrossProduct(target_, HAxis);
	up_.Normalize();
}

void Camera::OnRender()
{
	bool shouldUpdate = false;
	
	if (onLEdge_)
		angleH_ -= EDGE_STEP;
	else if (onREdge_)
		angleH_ += EDGE_STEP;

	shouldUpdate = (onLEdge_ || onREdge_);

	if (onTEdge_)
	{
		if (angleV_ > -90.0f)
		{
			angleV_ -= EDGE_STEP;
			shouldUpdate = true;
		}
	}
	else if (onBEdge_)
	{
		if (angleV_ < 90.0f)
		{
			angleV_ += EDGE_STEP;
			shouldUpdate = true;
		}
	}

	if (shouldUpdate)
	{
		Update();
	}
}

//////////////////////////////////////////////////////////////////////////


Pipeline::Pipeline()
	: scale_(1, 1, 1)
	, position_(0, 0, 0)
	, rotate_(0, 0, 0)
	, camera_(nullptr)
{

}

void Pipeline::Scale(float s)
{
	Scale(s, s, s);
}

void Pipeline::Scale(float x, float y, float z)
{
	scale_.x = x;
	scale_.y = y;
	scale_.z = z;
}

void Pipeline::Scale(const Vector3f& scale)
{
	Scale(scale.x, scale.y, scale.z);
}

void Pipeline::WorldPos(float x, float y, float z)
{
	position_.x = x;
	position_.y = y;
	position_.z = z;
}

void Pipeline::WorldPos(const Vector3f& pos)
{
	WorldPos(pos.x, pos.y, pos.z);
}

void Pipeline::Rotate(float x, float y, float z)
{
	rotate_.x = x;
	rotate_.y = y;
	rotate_.z = z;
}

void Pipeline::Rotate(const Vector3f& rorate)
{
	Rotate(rorate.x, rorate.y, rorate.z);
}

void Pipeline::SetPerspectiveProj(const PersProjInfo& proj)
{
	persProjInfo_ = proj;
}

void Pipeline::SetCamera(const Camera& camera)
{
	camera_ = &camera;
}

const Matrix44f& Pipeline::GetOGLWorldTrans()
{
	// Note: OpenGL using matrix by col major
	Matrix44f translationTrans;
	MaxtrixTranslation(translationTrans, position_);
	translationTrans.Transpose();

	Matrix44f scaleTrans;
	MatrixScaling(scaleTrans, scale_);
	scaleTrans.Transpose();

	// Note:
	// (rx * ry * rz)' = rz' * ry' * rx'
	Matrix44f rotateTrans;
	MatrixRotationZYX(rotateTrans, rotate_.x, rotate_.y, rotate_.z);
	rotateTrans.Transpose();

	// (t * r * s) = t' * r' * s'
	Matrix44f n;
	MatrixMultiply(n, rotateTrans, scaleTrans);
	MatrixMultiply(worldTrans_, translationTrans, n);

	return worldTrans_;
}

const Matrix44f& Pipeline::GetOGLViewTrans()
{
	// Note: inverse translation of camera position!
	Matrix44f translationTrans;
	MaxtrixTranslation(translationTrans, -camera_->GetPosition());
	translationTrans.Transpose();

	// n: target
	Vector3f n = camera_->GetTarget();
	n.Normalize();
	// v: up
	Vector3f v = camera_->GetUp();
	v.Normalize();
	// u: right
	// u = v x n
	Vector3f u = CrossProduct(v, n);
	// v = n x u
	v = CrossProduct(n, u);

	Matrix44f rotateTrans;
	auto& m = rotateTrans.m;
	m[0][0] = u.x;   m[0][1] = u.y;   m[0][2] = u.z;   m[0][3] = 0.0f;
	m[1][0] = v.x;   m[1][1] = v.y;   m[1][2] = v.z;   m[1][3] = 0.0f;
	m[2][0] = n.x;   m[2][1] = n.y;   m[2][2] = n.z;   m[2][3] = 0.0f;
	m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;

	MatrixMultiply(viewTrans_, rotateTrans, translationTrans);
	return viewTrans_;
}

const Matrix44f& Pipeline::GetOGLProjTrans()
{
	const PersProjInfo& p = persProjInfo_;

	const float ar = p.Width / p.Height;
	const float zRange = p.zNear - p.zFar;
	const float tanHalfFOV = tanf(angle2radian(p.FOV / 2.0f));

	auto& m = projTrans_.m;

	m[0][0] = 1.0f / (tanHalfFOV * ar); m[0][1] = 0.0f;					m[0][2] = 0.0f;							m[0][3] = 0.0;
	m[1][0] = 0.0f;                     m[1][1] = 1.0f / tanHalfFOV;	m[1][2] = 0.0f;							m[1][3] = 0.0;
	m[2][0] = 0.0f;                     m[2][1] = 0.0f;					m[2][2] = (-p.zNear - p.zFar) / zRange; m[2][3] = 2.0f * p.zFar * p.zNear / zRange;
	m[3][0] = 0.0f;                     m[3][1] = 0.0f;					m[3][2] = 1.0f;							m[3][3] = 0.0;

	return projTrans_;
}

const Matrix44f& Pipeline::GetOGLWorldProjTrans()
{
	GetOGLProjTrans();

	GetOGLWorldTrans();

	MatrixMultiply(worldProjTrans_, projTrans_, worldTrans_);

	return worldProjTrans_;
}

const Matrix44f& Pipeline::GetOGLWorldViewProjTrans()
{
	GetOGLWorldTrans();

	GetOGLViewTrans();

	GetOGLProjTrans();

	// world -> view -> projection
	Matrix44f n;
	MatrixMultiply(n, projTrans_, viewTrans_);
	MatrixMultiply(worldViewProjTrans_, n, worldTrans_);

	return worldViewProjTrans_;
}

//////////////////////////////////////////////////////////////////////////
//functions

static ICallbacks* s_pCallbacks = nullptr;
static bool s_withDepth = false;
static bool s_withStencil = false;

static void RenderSceneCB()
{
	s_pCallbacks->RenderSceneCB();
}

static void IdleCB()
{
	s_pCallbacks->RenderSceneCB();
}

static void SpecialKeyboardCB(int key, int x, int y)
{
	s_pCallbacks->KeyboardCB(key);
}

static void KeyboardCB(unsigned char key, int x, int y)
{
	s_pCallbacks->KeyboardCB(key);
}

static void PassiveMouseCB(int x, int y)
{
	s_pCallbacks->PassiveMouseCB(x, y);
}

static void MouseCB(int button, int state, int x, int y)
{
	s_pCallbacks->MouseCB(button, state, x, y);
}

static void InitCallbacks()
{
	glutDisplayFunc(RenderSceneCB);
	glutIdleFunc(IdleCB);
	glutSpecialFunc(SpecialKeyboardCB);
	glutKeyboardFunc(KeyboardCB);
	glutPassiveMotionFunc(PassiveMouseCB);
	glutMouseFunc(MouseCB);
}

void GLUTBackendInit(int argc, char** argv, bool withDepth, bool withStencil)
{
	s_withDepth = withDepth;
	s_withStencil = withStencil;

	glutInit(&argc, argv);
	
	unsigned int displayMode = GLUT_DOUBLE | GLUT_RGBA;
	if (withDepth)
		displayMode |= GLUT_DEPTH;
	if (withStencil)
		displayMode |= GLUT_STENCIL;

	glutInitDisplayMode(displayMode);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
}

bool GLUTBackendCreateWindow(unsigned int width, unsigned int height, bool fullScreen, const char* title)
{
	if (fullScreen)
	{
		int bpp = 32;
		const char* p = dopixel::core::str_format("%dx%d:%d@60", width, height, bpp);
		glutGameModeString(p);
		glutEnterGameMode();
	}
	else
	{
		glutInitWindowSize(width, height);
		glutInitWindowPosition(0, 0);
		glutCreateWindow(title);
	}

	// Must be done after glut is initialized
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "error: '%s'\n", glewGetErrorString(res));
		return false;
	}

	return true;
}

void GLUTBackendRun(ICallbacks* callbacks)
{
	if (!callbacks)
	{
		fprintf(stderr, "%s: callbacks not specified\n", __FUNCTION__);
		return;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	if (s_withDepth)
		glEnable(GL_DEPTH_TEST);

	s_pCallbacks = callbacks;
	InitCallbacks();
	glutMainLoop();
}

void GLUTBackendSwapBuffers()
{
	glutSwapBuffers();
}

void GLUTBackendLeaveMainLoop()
{
	glutLeaveMainLoop();
}
