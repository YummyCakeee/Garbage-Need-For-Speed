#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "Car.h"

class Bot
{
protected:

public:
	Bot() = default;
	virtual void Act(double deltaTime) = 0;

};

class CarBot : public Bot, public Car
{
private:

public:
	CarBot(const glm::vec3& dir, const glm::dvec3& speed, const glm::vec3& position,
		const glm::vec3& force, float maxTorque, float mass, float wheelsAxisDist, double maxTurnAngle);
	CarBot(const glm::vec3& dir, const glm::dvec3& speed, const glm::vec3& position,
		float maxTorque, float mass, float wheelsAxisDist, double maxTurnAngle) ;
	virtual void Act(double deltaTime) override;
};