#include "Force.h"

Force::Force()
{
	force = glm::vec3(0.0f);
}

Force::Force(const glm::vec3& force)
{
	this->force = force;
}

Force::Force(const Force& force)
{
	this->force = force.force;
}

Force::~Force()
{
}

glm::vec3 Force::GetForce()
{
	return force;
}

void Force::SetForce(const glm::vec3& force)
{
	this->force = force;
}

Force& Force::operator=(const Force& force)
{
	this->force = force.force;
	return *this;
}