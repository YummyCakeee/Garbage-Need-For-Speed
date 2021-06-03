#include "Bot.h"

CarBot::CarBot(const glm::vec3& dir, const glm::dvec3& speed, const glm::vec3& position,
	const glm::vec3& force, float maxTorque, float mass, float wheelsAxisDist, double maxTurnAngle) :
	Car(dir, speed, position, maxTorque, mass, wheelsAxisDist, maxTurnAngle)
{

}
CarBot::CarBot(const glm::vec3& dir, const glm::dvec3& speed, const glm::vec3& position,
	float maxTorque, float mass, float wheelsAxisDist, double maxTurnAngle) :
	Car(dir, speed, position, maxTorque, mass, wheelsAxisDist, maxTurnAngle)
{

}

void CarBot::Act(double deltaTime)
{
	ClearForces();
	if (glm::length(GetSpeedProjToDirection()) < 12.0f)
		AddForce(direction * GetDriveForce());
}