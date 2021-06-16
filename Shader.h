#pragma once
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include "LightSource.h"
#include "Camera.h"

class SceneInfo
{
private:
public:
	SceneInfo();
	~SceneInfo();
};

enum class ShaderType
{
	MATERIAL, SHADOW_MAP, SCREEN
};

class Shader
{
protected:
	unsigned int programID;
	ShaderType type;
	Shader(ShaderType type, const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL);
	std::string readShaderFromFile(const char* path);
	void checkCompileErrors(unsigned int shader, std::string type);
public:
	~Shader();
	void use() const;
	void free();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec(const std::string& name, float x, float y) const;
	void setVec(const std::string& name, float x, float y, float z) const;
	void setVec(const std::string& name, float x, float y, float z, float w) const;
	void setVec(const std::string& name, glm::vec2 vector) const;
	void setVec(const std::string& name, glm::vec3 vector) const;
	void setVec(const std::string& name, glm::vec4 vector) const;
	void setMatrix4F(const std::string& name, glm::mat4& m) const;
	ShaderType GetType() const;
	unsigned int ID() const;
};

struct LightInfo
{
	glm::mat4 lightSpaceMat;
	//std::list<unsigned int> shadowMapsID;

};

class MaterialShader : public Shader
{
private:
	const Camera* camera;
	const glm::mat4* modelMatrix;
	//std::list<LightInfo> lights;
	void loadCameraAndModelMatrix(const Camera* camera = NULL, const glm::mat4* modelMatrix = NULL) const;
public:
	//	TEST
	unsigned int shadowMapTexture;
	glm::mat4 lightSpaceMat;
	//	~TEST
	MaterialShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL);
	~MaterialShader();
	void loadMainInfo(const Camera* camera = NULL, const glm::mat4* modelMatrix = NULL) const;
	void setCameraAndModelMatrix(const Camera* camera, const glm::mat4* modelMatrix);
};

class ShadowMapShader : public Shader
{
private:
	const glm::mat4* lightSpaceMatrix;
	const glm::mat4* modelMatrix;
	void loadLightSpaceAndModelMatrix(const glm::mat4* lightSpaceMatrix = NULL, const glm::mat4* modelMatrix = NULL) const;
public:
	ShadowMapShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL);
	~ShadowMapShader();
	void loadMainInfo(const glm::mat4* lightSpaceMatrix = NULL, const glm::mat4* modelMatrix = NULL) const;
	void setLightSpaceAndModelMatrix(const glm::mat4* lightSpaceMatrix, const glm::mat4* modelMatrix);
};

class ScreenShader : public Shader
{
private:
	glm::dvec3 playerSpeed;
	void loadPlayerSpeed(const glm::dvec3* playerSpeed = NULL) const;
public:
	ScreenShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL);
	~ScreenShader();
	void loadMainInfo(const glm::dvec3* playerSpeed = NULL) const;
	void setPlayerSpeed(glm::dvec3 playerSpeed);
};

class LightsUBO
{
private:
	GLuint lightsUBO;
	GLint countersOffsets[3];
	GLint* dirLghtOffsets;
	GLint* pntLghtOffsets;
	GLint* sptLghtOffsets;
	int dirLightsCnt;
	int pntLightsCnt;
	int sptLightsCnt;
	void GenerateBuffer(const Shader& shader);
public:
	LightsUBO();
	LightsUBO(const Shader& shader, int dirLightsCnt, int pntLightsCnt, int sptLightsCnt);
	~LightsUBO();
	void LoadInfo(const std::vector<const LightSource*>& lights);
};
