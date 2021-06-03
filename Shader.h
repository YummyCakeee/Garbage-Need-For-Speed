#pragma once
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "LightSource.h"
#include "Camera.h"

class SceneInfo
{
private:
	const std::vector<const LightSource*>* lights;
public:
	SceneInfo();
	~SceneInfo();
	void SetSceneLights(const std::vector<const LightSource*>* lights);
	const std::vector<const LightSource*>* GetSceneLights() const;
};

class Shader
{
private:
	SceneInfo* sceneInfo;
	const Camera* camera;
	const glm::mat4* modelMatrix;
	void loadLights() const;
	void loadCameraAndModelMatrix(const Camera* camera = NULL, const glm::mat4* modelMatrix = NULL) const;
public:
	Shader(const char* vertexPath, const char* fragmentPath);
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
	void loadMainInfo(const Camera* camera = NULL, const glm::mat4* modelMatrix = NULL) const;
	void setSceneInfo(const SceneInfo* const sceneInfo);
	void setCameraAndModelMatrix(const Camera* camera, const glm::mat4* modelMatrix);
	unsigned int ID() const;

private:
	unsigned int programID;
	void checkCompileErrors(unsigned int shader, std::string type);
};

class LightsUBO
{
private:
	GLuint uboBuf;
	GLint* dirLghtOffsets;
	GLint* pntLghtOffsets;
	GLint* sptLghtOffsets;
	int dirLightsCnt;
	int pntLightsCnt;
	int sptLightsCnt;
	void GenerateBuffer(const Shader& shader);
public:
	LightsUBO(const Shader& shader, int dirLightsCnt, int pntLightsCnt, int sptLightsCnt);
	~LightsUBO();
	void LoadInfo(const std::vector<const LightSource*>& lights);
};
