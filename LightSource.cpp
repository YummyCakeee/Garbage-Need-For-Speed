#include "LightSource.h"

LightSource::LightSource()
{
	ambient = glm::vec3(0.0f);
	diffuse = glm::vec3(0.0f);
	specular = glm::vec3(0.0f);
	type = SourceType::UNDEFINED;
	enabled = true;
}

LightSource::LightSource(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	SetAmbient(ambient);
	SetDiffuse(diffuse);
	SetSpecular(specular);
	type = SourceType::UNDEFINED;
	enabled = true;
}

void LightSource::SetAmbient(glm::vec3 ambient)
{
	this->ambient = glm::clamp(ambient, glm::vec3(0.0f), glm::vec3(1.0f));
}

void LightSource::SetDiffuse(glm::vec3 diffuse)
{
	this->diffuse = glm::clamp(diffuse, glm::vec3(0.0f), glm::vec3(1.0f));
}

void LightSource::SetSpecular(glm::vec3 specular)
{
	this->specular = glm::clamp(specular, glm::vec3(0.0f), glm::vec3(1.0f));
}

void LightSource::SetColors(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	SetAmbient(ambient);
	SetDiffuse(diffuse);
	SetSpecular(specular);
}

void LightSource::Enable(bool enable)
{
	enabled = enable;
}

glm::vec3 LightSource::GetAmbient() const
{
	return ambient;
}

glm::vec3 LightSource::GetDiffuse() const
{
	return diffuse;
}

glm::vec3 LightSource::GetSpecular() const
{
	return specular;
}

SourceType LightSource::GetType() const
{
	return type;
}

glm::mat4 LightSource::GetProjectionMatrix() const
{
	return glm::mat4(1.0f);
}

bool LightSource::IsEnabled() const
{
	return enabled;
}

void LightSource::Update() const
{

}

//	Moving Light

MovingLight::MovingLight()
{
	position = glm::vec3(0.0f);
	isBindToObject = false;
	offset = glm::vec3(0.0f);
	constant = 0.0f;
	linear = 0.0f;
	quadratic = 0.0f;
}

MovingLight::MovingLight(glm::vec3 position)
{
	this->position = position;
	isBindToObject = false;
	offset = glm::vec3(0.0f);
	constant = 0.0f;
	linear = 0.0f;
	quadratic = 0.0f;
}

MovingLight::MovingLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position)
{
	this->position = position;
	isBindToObject = false;
	offset = glm::vec3(0.0f);
	constant = 0.0f;
	linear = 0.0f;
	quadratic = 0.0f;
	SetAmbient(ambient);
	SetDiffuse(diffuse);
	SetSpecular(specular);
}

MovingLight::MovingLight(glm::vec3 position, float constant, float linear, float quadratic)
{
	this->position = position;
	isBindToObject = false;
	offset = glm::vec3(0.0f);
	SetConstant(constant);
	SetLinear(linear);
	SetQuadratic(quadratic);
}

MovingLight::MovingLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
	glm::vec3 position, float constant, float linear, float quadratic) :
	LightSource(ambient, diffuse, specular)
{
	this->position = position;
	isBindToObject = false;
	offset = glm::vec3(0.0f);
	SetConstant(constant);
	SetLinear(linear);
	SetQuadratic(quadratic);
}


void MovingLight::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void MovingLight::SetOffset(glm::vec3 offset)
{
	this->offset = offset;
}


glm::vec3 MovingLight::GetPosition() const
{
	return position;
}

glm::vec3 MovingLight::GetOffset() const
{
	return offset;
}

void MovingLight::SetBind(bool status)
{
	isBindToObject = status;
}

bool MovingLight::IsBindToObject() const
{
	return isBindToObject;
}

void MovingLight::Update() const
{

}

void MovingLight::SetConstant(float constant)
{
	this->constant = glm::clamp(constant, 0.0f, 1.0f);
}

void MovingLight::SetLinear(float linear)
{
	this->linear = glm::clamp(linear, 0.0f, 1.0f);
}

void MovingLight::SetQuadratic(float quadratic)
{
	this->quadratic = glm::clamp(quadratic, 0.0f, 1.0f);
}

void MovingLight::SetAttenuationProps(float constant, float linear, float quadratic)
{
	SetConstant(constant);
	SetLinear(linear);
	SetQuadratic(quadratic);
}

float MovingLight::GetConstant() const
{
	return constant;
}

float MovingLight::GetLinear() const
{
	return linear;
}

float MovingLight::GetQuadratic() const
{
	return quadratic;
}

//	Point Light

PointLight::PointLight()
{
	type = SourceType::POINT;
}

PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
	MovingLight(ambient, diffuse, specular, position)
{
	type = SourceType::POINT;
}

PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
	float constant, float linear, float quadratic) :
	MovingLight(ambient, diffuse, specular, position, constant, linear, quadratic)
{
	type = SourceType::POINT;
}


//	Directional Light

DirLight::DirLight()
{
	direction = glm::vec3(1.0f, 0.0f, 0.0f);
	type = SourceType::DIRECTIONAL;
}

DirLight::DirLight(glm::vec3 direction)
{
	SetDirection(direction);
	type = SourceType::DIRECTIONAL;
}

DirLight::DirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
	LightSource(ambient, diffuse, specular)
{
	SetDirection(direction);
	type = SourceType::DIRECTIONAL;
}

void DirLight::SetDirection(glm::vec3 direction)
{
	if (glm::length(direction) != 0.0f)
		this->direction = glm::normalize(direction);
}

glm::vec3 DirLight::GetDirection() const
{
	return direction;
}

glm::mat4 DirLight::GetProjectionMatrix() const
{
	return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
}

glm::mat4 DirLight::GetViewMatrix() const
{
	return glm::lookAt(glm::vec3(0.0f), direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

//	SpotLight

SpotLight::SpotLight()
{
	direction = glm::vec3(1.0f, 0.0f, 0.0f);
	_startDirection = direction;
	cutOff = 0.0f;
	outerCutOff = 0.0f;
	type = SourceType::SPOTLIGHT;
}

SpotLight::SpotLight(glm::vec3 direction, float cutOff, float outerCutOff)
{
	SetDirection(direction);
	SetCutOff(cutOff);
	SetOuterCutOff(outerCutOff);
	type = SourceType::SPOTLIGHT;
	_startDirection = direction;
}

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
	float cutOff, float outerCutOff) :
	MovingLight(ambient, diffuse, specular, position)
{
	SetDirection(direction);
	SetCutOff(cutOff);
	SetOuterCutOff(outerCutOff);
	type = SourceType::SPOTLIGHT;
	_startDirection = direction;
}
SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
	float constant, float linear, float quadratic, float cutOff, float outerCutOff) :
	MovingLight(ambient, diffuse, specular, position, constant, linear, quadratic)
{
	SetDirection(direction);
	SetCutOff(cutOff);
	SetOuterCutOff(outerCutOff);
	type = SourceType::SPOTLIGHT;
	_startDirection = direction;
}

void SpotLight::SetDirection(glm::vec3 direction)
{
	if (glm::length(direction) > 0.0f)
		this->direction = glm::normalize(direction);
}

void SpotLight::ResetStartDirection()
{
	_startDirection = direction;
}

void SpotLight::SetCutOff(float cutOff)
{
	this->cutOff = glm::clamp(cutOff, -1.0f, 1.0f);
}

void SpotLight::SetOuterCutOff(float outerCutOff)
{
	this->outerCutOff = glm::clamp(outerCutOff, -1.0f, 1.0f);
}

glm::vec3 SpotLight::GetDirection() const
{
	return direction;
}

glm::vec3 SpotLight::GetStartDirection() const
{
	return _startDirection;
}

float SpotLight::GetCutOff() const
{
	return cutOff;
}

float SpotLight::GetOuterCutOff() const
{
	return outerCutOff;
}