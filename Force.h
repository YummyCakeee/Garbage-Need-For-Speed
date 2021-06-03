#pragma once
#define GLM_FORCE_RADIANS
#include <string>
#include <glm/glm.hpp>

class Force
{
public:
	Force();
	Force(const glm::vec3& force);
	Force(const Force& force);
	~Force();
	glm::vec3 GetForce();
	void SetForce(const glm::vec3& force);
	Force& operator=(const Force& force);

private:
	glm::vec3 force;
};
