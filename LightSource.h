#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class SourceType
{
	POINT, DIRECTIONAL, SPOTLIGHT, UNDEFINED
};

class LightSource
{
protected:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	SourceType type;
	bool enabled;
	glm::vec3 GetUpDirection(glm::vec3 direction, glm::vec3 WorldUp) const;
public:
	LightSource();
	LightSource(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void SetAmbient(glm::vec3 ambient);
	void SetDiffuse(glm::vec3 diffuse);
	void SetSpecular(glm::vec3 specular);
	void SetColors(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void Enable(bool enable);
	glm::vec3 GetAmbient() const;
	glm::vec3 GetDiffuse() const;
	glm::vec3 GetSpecular() const;
	SourceType GetType() const;
	virtual glm::mat4 GetProjectionMatrix() const;
	bool IsEnabled() const;
	virtual void Update() const;
};

class MovingLight : public LightSource
{
protected:
	glm::vec3 position;
	glm::vec3 offset;
	bool isBindToObject;
	float constant;
	float linear;
	float quadratic;
public:
	MovingLight();
	MovingLight(glm::vec3 position);
	MovingLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position);
	MovingLight(glm::vec3 position, float constant, float linear, float quadratic);
	MovingLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
		glm::vec3 position, float constant, float linear, float quadratic);
	void SetPosition(glm::vec3 position);
	void SetOffset(glm::vec3 offset);
	void SetConstant(float constant);
	void SetLinear(float linear);
	void SetQuadratic(float quadratic);
	void SetAttenuationProps(float constant, float linear, float quadratic);
	glm::vec3 GetPosition() const;
	glm::vec3 GetOffset() const;
	float GetConstant() const;
	float GetLinear() const;
	float GetQuadratic() const;
	void SetBind(bool status);
	bool IsBindToObject() const;
	void Update() const override;
};

class PointLight : public MovingLight
{
public:
	PointLight();
	PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
		float constant, float linear, float quadratic);
};

class DirLight : public LightSource
{
private:
	glm::vec3 direction;
public:
	DirLight();
	DirLight(glm::vec3 direction);
	DirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void SetDirection(glm::vec3 direction);
	glm::vec3 GetDirection() const;
	virtual glm::mat4 GetProjectionMatrix() const override;
	glm::mat4 GetViewMatrix(const glm::vec3& lightPosition = glm::vec3(0.0f)) const;
};

class SpotLight : public MovingLight
{
private:
	glm::vec3 direction;
	glm::vec3 _startDirection;
	float cutOff;
	float outerCutOff;
public:
	SpotLight();
	SpotLight(glm::vec3 direction, float cutOff, float outerCutOff);
	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
		float cutOff, float outerCutOff);
	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
		float constant, float linear, float quadratic, float cutOff, float outerCutOff);

	void SetDirection(glm::vec3 direction);
	void ResetStartDirection();
	void SetCutOff(float cutOff);
	void SetOuterCutOff(float outerCutOff);
	glm::vec3 GetDirection() const;
	glm::vec3 GetStartDirection() const;
	float GetCutOff() const;
	float GetOuterCutOff() const;
	virtual glm::mat4 GetProjectionMatrix() const override;
	glm::mat4 GetViewMatrix() const;
};