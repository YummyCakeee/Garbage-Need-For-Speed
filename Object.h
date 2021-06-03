#pragma once
#define GLM_FORCE_RADIANS
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <string>
#include "GameGlobalStructs.h"
#include "Model.h"
#include "Force.h"

class Object
{
protected:
	static unsigned int idCounter;
	unsigned int id;
	Model* model;
	std::map<std::string, MovingLight*> lights;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;
	glm::vec3 worldUp;
	glm::vec3 _startDirection;
	glm::vec3 scale;
	glm::dvec3 speed;
	glm::vec3 force;
	std::vector<Force> forces;
	virtual void Move(double dTime);
public:
	Object();
	Object(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& scale);
	Object(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& scale, const glm::vec3& worldUp);
	void AddForce(const glm::vec3& force);
	void RemoveForce(int index);
	void SumForces();
	void ClearForces();
	Model* GetModel();
	unsigned int GetID();
	virtual const glm::vec3* GetPosition();
	virtual glm::vec3 GetDirection();
	glm::dvec3 GetSpeed();
	glm::dvec3 GetSpeedProjToDirection();
	glm::mat4 GetModelMatrix();
	MovingLight* GetLightSource(const std::string& name);
	void SetPosition(const glm::vec3& position);
	void SetDirection(const glm::vec3& direction);
	void SetScale(const glm::vec3& scale);
	void SetModel(Model* model);
	void SetWorldUp(glm::vec3 up);
	void UpdateModelProps();
	void BindLightSource(const std::string& name, MovingLight* light);
	void UnbindLightSource(const std::string& name);
	void UnbindAllLightSources();
	void UpdateLightsPositions();
	void UpdateVectors();
	virtual void Draw();
	virtual void ProcessInput(const std::vector<Key>& keys, Mouse& mouse, double dTime);
	virtual void Update(double dTime);
	virtual ~Object() = default;
};

