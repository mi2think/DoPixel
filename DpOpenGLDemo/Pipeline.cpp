#include "Pipeline.h"

#include "DpFileStream.h"
#include "stb/stb_image.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

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
		fprintf(stdout, "%s\n", fileData);
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

Texture::Texture(GLenum textureTarget, const string& fileName)
	: textureTarget_(textureTarget)
	, fileName_(fileName)
	, width_(0)
	, height_(0)
	, data_(nullptr)
{
}

Texture::~Texture()
{
	if (data_)
	{
		free(data_);
	}
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
		if (data_ != nullptr)
		{
			glGenTextures(1, &textureObj_);
			glBindTexture(textureTarget_, textureObj_);
			glTexImage2D(textureTarget_, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_);
			glTexParameterf(textureTarget_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(textureTarget_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(textureTarget_, 0);

			stbi_set_flip_vertically_on_load(false);
			return true;
		}
	}
	return false;
}

void Texture::Bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(textureTarget_, textureObj_);
}

//////////////////////////////////////////////////////////////////////////
Mesh::MeshEntry::MeshEntry()
{
	VB_ = INVALID_OGL_VALUE;
	IB_ = INVALID_OGL_VALUE;
	numIndices_ = 0;
	materialIndex_ = INVALID_MATERIAL;
}

Mesh::MeshEntry::~MeshEntry()
{
	if (VB_ != INVALID_OGL_VALUE)
	{
		glDeleteBuffers(1, &VB_);
	}
	if (IB_ != INVALID_OGL_VALUE)
	{
		glDeleteBuffers(1, &IB_);
	}
}

void Mesh::MeshEntry::Init(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices)
{
	numIndices_ = indices.size();

	glGenBuffers(1, &VB_);
	glBindBuffer(GL_ARRAY_BUFFER, VB_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IB_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices_, &indices[0], GL_STATIC_DRAW);
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	Clear();
}

void Mesh::Clear()
{
	for (auto texture : textures_)
	{
		delete texture;
	}
	textures_.clear();
	entries_.clear();
}

bool Mesh::LoadMesh(const string& fileName)
{
	Clear();

	bool ret = false;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	if (scene)
	{
		ret = InitFromScene(scene, fileName);
	}
	else
	{
		fprintf(stderr, "error parsing '%s' : '%s'", fileName.c_str(), importer.GetErrorString());
	}
	return ret;
}

bool Mesh::InitFromScene(const aiScene* scene, const string& fileName)
{
	entries_.resize(scene->mNumMeshes);
	textures_.resize(scene->mNumMaterials);

	for (unsigned int i = 0; i < entries_.size(); ++i)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		InitMesh(i, mesh);
	}

	return InitMaterials(scene, fileName);
}

void Mesh::InitMesh(unsigned int index, const aiMesh* mesh)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	const aiVector3D zero(0.0f, 0.0f, 0.0f);
	// vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		const aiVector3D* pos = &(mesh->mVertices[i]);
		const aiVector3D* normal = &(mesh->mNormals[i]);
		const aiVector3D* texCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &zero;

		Vertex v(Vector3f(pos->x, pos->y, pos->z), Vector2f(texCoord->x, texCoord->y), Vector3f(normal->x, normal->y, normal->z));
		vertices.push_back(v);
	}
	// indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];
		ASSERT(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	entries_[index].materialIndex_ = mesh->mMaterialIndex;
	entries_[index].Init(vertices, indices);
}

bool Mesh::InitMaterials(const aiScene* scene, const string& fileName)
{
	auto slashIndex = fileName.find_last_of("/");
	string dir;
	if (slashIndex == string::npos)
		dir = ".";
	else if (slashIndex == 0)
		dir = "/";
	else
		dir = fileName.substr(0, slashIndex);

	bool ret = true;
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		const aiMaterial* material = scene->mMaterials[i];
		textures_[i] = nullptr;

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
			{
				string fullPath = dir + "/" + path.data;
				textures_[i] = new Texture(GL_TEXTURE_2D, fullPath.c_str());
				if (!textures_[i]->Load())
				{
					fprintf(stderr, "error loading texture '%s'\n", fullPath.c_str());
					delete textures_[i];
					textures_[i] = nullptr;
					ret = false;
				}
				else
				{
					fprintf(stdout, "loaded texture '%s'\n", fullPath.c_str());
				}
			}
		}

		// loading a white texture in case model does not include its own texture
		if (!textures_[i])
		{
			textures_[i] = new Texture(GL_TEXTURE_2D, "../Resource/white.png");
			ret = textures_[i]->Load();
		}
	}

	return ret;
}

void Mesh::Render()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (unsigned int i = 0; i < entries_.size(); ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, entries_[i].VB_);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entries_[i].IB_);

		const unsigned int materialIndex = entries_[i].materialIndex_;
		if (materialIndex < textures_.size() && textures_[materialIndex] != nullptr)
		{
			textures_[materialIndex]->Bind(GL_TEXTURE0);
		}

		glDrawElements(GL_TRIANGLES, entries_[i].numIndices_, GL_UNSIGNED_INT, 0);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

//////////////////////////////////////////////////////////////////////////
namespace basiclighting
{
	bool BasicLightingTechnique::Init()
	{
		if (!Technique::Init())
			return false;

		if (!AddShaderFromFile(GL_VERTEX_SHADER, "../DpOpenGLDemo/basic_lighting.vs"))
			return false;

		if (!AddShaderFromFile(GL_FRAGMENT_SHADER, "../DpOpenGLDemo/basic_lighting.ps"))
			return false;

		if (!Finalize())
			return false;

		WVPLocation_ = GetUniformLocation("gWVP");
		worldMatrixLocation_ = GetUniformLocation("gWorld");
		samplerLocation_ = GetUniformLocation("gSampler");
		eyeWorldPosLocation_ = GetUniformLocation("gEyeWorldPos");
		specularIntensityLocation_ = GetUniformLocation("gSpecularIntensity");
		specularPowerLocation_ = GetUniformLocation("gSpecularPower");
		numPointLightsLocation_ = GetUniformLocation("gNumPointLights");
		numSpotLightsLocation_ = GetUniformLocation("gNumSpotLights");

		directionalLight_.color = GetUniformLocation("gDirectionalLight.base.color");
		directionalLight_.ambientIntensity = GetUniformLocation("gDirectionalLight.base.ambientIntensity");
		directionalLight_.diffuseIntensity = GetUniformLocation("gDirectionalLight.base.diffuseIntensity");
		directionalLight_.direction = GetUniformLocation("gDirectionalLight.direction");

		for (unsigned int i = 0; i < MAX_POINT_LIGHTS; ++i)
		{
			auto& light = pointLightsLocation_[i];

			const char* p = dopixel::str_format("gPointLights[%d].base.color", i);
			light.color = GetUniformLocation(p);

			p = dopixel::str_format("gPointLights[%d].base.ambientIntensity", i);
			light.ambientIntensity = GetUniformLocation(p);

			p = dopixel::str_format("gPointLights[%d].base.diffuseIntensity", i);
			light.diffuseIntensity = GetUniformLocation(p);

			p = dopixel::str_format("gPointLights[%d].position", i);
			light.position = GetUniformLocation(p);

			p = dopixel::str_format("gPointLights[%d].atten.kc", i);
			light.atten.kc = GetUniformLocation(p);

			p = dopixel::str_format("gPointLights[%d].atten.kl", i);
			light.atten.kl = GetUniformLocation(p);

			p = dopixel::str_format("gPointLights[%d].atten.kq", i);
			light.atten.kq = GetUniformLocation(p);

			if (light.color == INVALID_UNIFORM || light.ambientIntensity == INVALID_UNIFORM ||
				light.diffuseIntensity == INVALID_UNIFORM || light.position == INVALID_UNIFORM ||
				light.position == INVALID_UNIFORM || light.atten.kc == INVALID_UNIFORM ||
				light.atten.kl == INVALID_UNIFORM || light.atten.kq == INVALID_UNIFORM)
				return false;
		}

		for (unsigned int i = 0; i < MAX_SPOT_LIGHTS; ++i)
		{
			auto& light = spotLightsLocation_[i];

			const char* p = dopixel::str_format("gSpotLights[%d].pl.base.color", i);
			light.color = GetUniformLocation(p);

			p = dopixel::str_format("gSpotLights[%d].pl.base.ambientIntensity", i);
			light.ambientIntensity = GetUniformLocation(p);

			p = dopixel::str_format("gSpotLights[%d].pl.base.diffuseIntensity", i);
			light.diffuseIntensity = GetUniformLocation(p);

			p = dopixel::str_format("gSpotLights[%d].pl.position", i);
			light.position = GetUniformLocation(p);

			p = dopixel::str_format("gSpotLights[%d].pl.atten.kc", i);
			light.atten.kc = GetUniformLocation(p);

			p = dopixel::str_format("gSpotLights[%d].pl.atten.kl", i);
			light.atten.kl = GetUniformLocation(p);

			p = dopixel::str_format("gSpotLights[%d].pl.atten.kq", i);
			light.atten.kq = GetUniformLocation(p);

			p = dopixel::str_format("gSpotLights[%d].direction", i);
			light.direction = GetUniformLocation(p);

			p = dopixel::str_format("gSpotLights[%d].cosThetaOver2", i);
			light.cosThetaOver2 = GetUniformLocation(p);

			p = dopixel::str_format("gSpotLights[%d].cosPhiOver2", i);
			light.cosPhiOver2 = GetUniformLocation(p);

			p = dopixel::str_format("gSpotLights[%d].pf", i);
			light.pf = GetUniformLocation(p);

			if (light.color == INVALID_UNIFORM || light.ambientIntensity == INVALID_UNIFORM ||
				light.diffuseIntensity == INVALID_UNIFORM || light.position == INVALID_UNIFORM ||
				light.position == INVALID_UNIFORM || light.atten.kc == INVALID_UNIFORM ||
				light.atten.kl == INVALID_UNIFORM || light.atten.kq == INVALID_UNIFORM ||
				light.direction == INVALID_UNIFORM || light.cosThetaOver2 == INVALID_UNIFORM ||
				light.cosPhiOver2 == INVALID_UNIFORM || light.pf == INVALID_UNIFORM)
				return false;
		}


		if (directionalLight_.ambientIntensity == INVALID_UNIFORM ||
			directionalLight_.diffuseIntensity == INVALID_UNIFORM ||
			directionalLight_.color == INVALID_UNIFORM ||
			directionalLight_.direction == INVALID_UNIFORM ||
			WVPLocation_ == INVALID_UNIFORM ||
			worldMatrixLocation_ == INVALID_UNIFORM ||
			samplerLocation_ == INVALID_UNIFORM ||
			eyeWorldPosLocation_ == INVALID_UNIFORM ||
			specularIntensityLocation_ == INVALID_UNIFORM ||
			specularPowerLocation_ == INVALID_UNIFORM ||
			numPointLightsLocation_ == INVALID_UNIFORM ||
			numSpotLightsLocation_ == INVALID_UNIFORM)
			return false;

		return true;
	}

	void BasicLightingTechnique::SetWVP(const Matrix44f& WVP)
	{
		glUniformMatrix4fv(WVPLocation_, 1, GL_TRUE, &(WVP.m[0][0]));
	}

	void BasicLightingTechnique::SetWorldMatrix(const Matrix44f& worldMatrix)
	{
		glUniformMatrix4fv(worldMatrixLocation_, 1, GL_TRUE, &(worldMatrix.m[0][0]));
	}

	void BasicLightingTechnique::SetTextureUnit(unsigned int textureUnit)
	{
		glUniform1i(samplerLocation_, textureUnit);
	}

	void BasicLightingTechnique::SetEyeWorldPos(const Vector3f& eyeWorldPos)
	{
		glUniform3f(eyeWorldPosLocation_, eyeWorldPos.x, eyeWorldPos.y, eyeWorldPos.z);
	}

	void BasicLightingTechnique::SetSpecularIntensity(float specularIntensity)
	{
		glUniform1f(specularIntensityLocation_, specularIntensity);
	}

	void BasicLightingTechnique::SetSpecularPower(float specularPower)
	{
		glUniform1f(specularPowerLocation_, specularPower);
	}

	void BasicLightingTechnique::SetDirectionalLight(const DirectionalLight& light)
	{
		glUniform3f(directionalLight_.color, light.color_.x, light.color_.y, light.color_.z);
		glUniform1f(directionalLight_.ambientIntensity, light.ambientIntensity_);
		glUniform1f(directionalLight_.diffuseIntensity, light.diffuseIntensity_);
		Vector3f dir = light.direction_;
		dir.Normalize();
		glUniform3f(directionalLight_.direction, dir.x, dir.y, dir.z);
	}

	void BasicLightingTechnique::SetPointLights(unsigned int numLights, const PointLight* pLights)
	{
		glUniform1i(numPointLightsLocation_, numLights);
		for (unsigned int i = 0; i < numLights; ++i)
		{
			const auto& loc = pointLightsLocation_[i];
			const auto& light = pLights[i];

			glUniform1f(loc.ambientIntensity, light.ambientIntensity_);
			glUniform1f(loc.diffuseIntensity, light.diffuseIntensity_);
			glUniform3f(loc.color, light.color_.x, light.color_.y, light.color_.z);
			glUniform3f(loc.position, light.position_.x, light.position_.y, light.position_.z);
			glUniform1f(loc.atten.kc, light.attenuation_.kc);
			glUniform1f(loc.atten.kl, light.attenuation_.kl);
			glUniform1f(loc.atten.kq, light.attenuation_.kq);
		}
	}

	void BasicLightingTechnique::SetSpotLights(unsigned int numLights, const SpotLight* pLights)
	{
		glUniform1i(numSpotLightsLocation_, numLights);
		for (unsigned int i = 0; i < numLights; ++i)
		{
			const auto& loc = spotLightsLocation_[i];
			const auto& light = pLights[i];

			glUniform1f(loc.ambientIntensity, light.ambientIntensity_);
			glUniform1f(loc.diffuseIntensity, light.diffuseIntensity_);
			glUniform3f(loc.color, light.color_.x, light.color_.y, light.color_.z);
			glUniform3f(loc.position, light.position_.x, light.position_.y, light.position_.z);
			glUniform1f(loc.atten.kc, light.attenuation_.kc);
			glUniform1f(loc.atten.kl, light.attenuation_.kl);
			glUniform1f(loc.atten.kq, light.attenuation_.kq);

			Vector3f dir = light.direction_;
			dir.Normalize();
			glUniform3f(loc.direction, dir.x, dir.y, dir.z);

			glUniform1f(loc.cosThetaOver2, cos(light.theta_ / 2.0f));
			glUniform1f(loc.cosPhiOver2, cos(light.phi_ / 2.0f));

			glUniform1f(loc.pf, light.pf_);
		}
	}
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
		const char* p = dopixel::str_format("%dx%d:%d@60", width, height, bpp);
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
