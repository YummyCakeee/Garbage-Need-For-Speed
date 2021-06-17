#pragma once
#define _USE_MATH_DEFINES
#define GLM_FORCE_RADIANS
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "Object.h"
#include "Force.h"

#define GEAR_RATIO_1 2.66f
#define GEAR_RATIO_2 1.78f
#define GEAR_RATIO_3 1.30f
#define GEAR_RATIO_4 1.0f
#define GEAR_RATIO_5 0.74f
#define GEAR_RATIO_6 0.50f
#define GEAR_RATIO_R 2.90f

class Car : public Object
{
private:
	void AddResistanceForces();
	float mass;
	float wheelsAxisDist;
	float wheelsRadius;
	float gearRatio;
	float diffRatio;
	float transEfficiency;
	float gasPercentage;
	float maxTorque;
	float angularVel;
	double maxTurnAngle;
	double currentTurnAngle;
public:
	Car();
	Car(const glm::vec3& dir, const glm::dvec3& speed, const glm::vec3& position,
		const glm::vec3& force, float maxTorque, float mass, float wheelsAxisDist, double maxTurnAngle);
	Car(const glm::vec3& dir, const glm::dvec3& speed, const glm::vec3& position,
		float maxTorque, float mass, float wheelsAxisDist, double maxTurnAngle);
	~Car();

	virtual void Move(double deltaTime) override final;
	void Turn(double angle, double deltaTime);
	void SetGearRatio(float ratio);
	float GetEngineForce();
	float GetWheelsAxisDist();
	double GetCurrentTurnAngle();
	double GetMaxTurnAngle();
	float GetDriveForce();
	virtual void ProcessInput(const std::vector<Key>& keys, Mouse& mouse, double dTime) override;
};

