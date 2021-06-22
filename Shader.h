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
#include <list>
#include <glm/gtc/type_ptr.hpp>
#include "LightSource.h"
#include "Mesh.h"


class Material;
class Mesh;

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
	void setMatrix4F(const std::string& name, const glm::mat4& m) const;
	ShaderType GetType() const;
	unsigned int ID() const;
	virtual void draw(unsigned int VAO, size_t indicesCount) const;
	virtual void clearSamplers() const;
	virtual void clearShaderInfo();
	void clear();
};

struct LightInfo
{
	std::list<glm::mat4> lightSpaceMats;
	unsigned int shadowMapID;
	SourceType type;
	float farPlane;
	LightInfo(const std::list<glm::mat4>& lightSpaceMats, unsigned int shadowMapID, SourceType type, float farPlane = 25);
	LightInfo(const glm::mat4& lightSpaceMat, unsigned int shadowMapID, SourceType type, float farPlane = 25);
	LightInfo(unsigned int shadowMapID, SourceType type, float farPlane = 25);
};

struct MaterialShaderInfo
{
	glm::vec3 viewPos;
	glm::mat4 spaceMatrix;
	glm::mat4 modelMatrix;
	std::list<LightInfo> lightsInfo;
	MaterialShaderInfo();
};

class MaterialShader : public Shader
{
private:
	MaterialShaderInfo shaderInfo;
	void loadMatrices(const glm::vec3* viewPos = NULL, const glm::mat4* spaceMatrix = NULL, const glm::mat4* modelMatrix = NULL) const;
	void loadMaterial(const Material* material) const;
	void loadLightsInfo(const std::list<LightInfo>* = NULL) const;
	int maxMatAndSkyboxTexsCnt;
public:
	MaterialShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL);
	~MaterialShader();
	void loadMainInfo(const glm::vec3* viewPos = NULL, const glm::mat4* spaceMatrix = NULL,
		const glm::mat4* modelMatrix = NULL, const Material* material = NULL) const;
	void setSpaceMatrix(const glm::mat4& spaceMatrix);
	void setViewPos(const glm::vec3& position);
	void setModelMatrix(const glm::mat4& modelMatrix);
	void addLightInfo(const LightInfo& light);
	virtual void clearSamplers() const override;
	virtual void clearShaderInfo() override;
};

struct ShadowMapShaderInfo
{
	std::list<glm::mat4> lightSpaceMatrices;
	glm::mat4 modelMatrix;
	glm::vec3 lightPos;
	float farPlane;
	bool linearizeDepth;
	ShadowMapShaderInfo();
};

class ShadowMapShader : public Shader
{
private:
	ShadowMapShaderInfo shaderInfo;
	void loadMatrices(const std::list<glm::mat4>* lightSpaceMatrices = NULL, const glm::mat4* modelMatrix = NULL) const;
	void loadLightPosAndFarPlane(const glm::vec3* lightPos = NULL, float farPlane = 0.0f) const;
	void loadMaterial(const Material* material) const;
public:
	ShadowMapShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL);
	~ShadowMapShader();
	void loadMainInfo(const std::list<glm::mat4>* lightSpaceMatrices = NULL, const glm::mat4* modelMatrix = NULL, 
		const glm::vec3* lightPos = NULL, float farPlane = 0.0f, const Material* material = NULL) const;
	void setLightSpaceMatrix(const glm::mat4& lightSpaceMatrix);
	void setLightSpaceMatrices(std::list<glm::mat4>& lightSpaceMatrices);
	void setModelMatrix(const glm::mat4& modelMatrix);
	void setLightPos(const glm::vec3& lightPos);
	void setFarPlane(float farPlane);
	void enableLinearDepth(bool enable);
	virtual void clearSamplers() const override;
	virtual void clearShaderInfo() override; 
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
	void LoadInfo(const std::vector<const LightSource*>& lights) const;
	int GetDirLightsCount() const;
	int GetPointLightsCount() const;
	int GetSpotLightsCount() const;
};
