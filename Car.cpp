#include "Car.h"

Car::Car() :Object()
{
	force = glm::vec3(0.0f);
	forces = std::vector<Force>();
	maxTorque = 250.0f;
	mass = 2000.0f;
	wheelsAxisDist = 50.0f;
	wheelsRadius = 0.42f;
	gearRatio = GEAR_RATIO_2;
	diffRatio = 3.42f;
	transEfficiency = 0.7f;
	gasPercentage = 0.3f;
	maxTorque = 300.0f;
	angularVel = 0.0f;
	maxTurnAngle = 45.0;
	currentTurnAngle = 0.0;
}

Car::Car(const glm::vec3& dir, const glm::dvec3& speed, const glm::vec3& position,
	const glm::vec3& force, float maxTorque, float mass, float wheelsAxisDist, double maxTurnAngle) :
	Object(position, dir, glm::vec3(1.0f))
{
	this->speed = speed;
	this->force = force;
	forces = std::vector<Force>();
	this->mass = mass;
	this->wheelsAxisDist = wheelsAxisDist;
	wheelsRadius = 0.42f;
	gearRatio = GEAR_RATIO_2;
	diffRatio = 3.42f;
	transEfficiency = 0.7f;
	gasPercentage = 0.3f;
	this->maxTorque = maxTorque;
	angularVel = 0.0f;
	this->maxTurnAngle = maxTurnAngle;
	currentTurnAngle = 0.0;
}

Car::Car(const glm::vec3& dir, const glm::dvec3& speed, const glm::vec3& position,
	float maxTorque, float mass, float wheelsAxisDist, double maxTurnAngle) :
	Object(position, dir, glm::vec3(1.0f))
{
	this->speed = speed;
	this->force = glm::vec3(0.0f);
	forces = std::vector<Force>();
	this->maxTorque = maxTorque;
	this->mass = mass;
	this->wheelsAxisDist = wheelsAxisDist;
	wheelsRadius = 0.42f;
	gearRatio = GEAR_RATIO_1;
	diffRatio = 3.42f;
	transEfficiency = 0.7f;
	gasPercentage = 0.3f;
	angularVel = 0.0f;
	this->maxTurnAngle = maxTurnAngle;
	currentTurnAngle = 0.0;
}

Car::~Car()
{
}

void Car::Move(double deltaTime)
{
	AddResistanceForces();
	SumForces();
	glm::dvec3 acc;
	acc = (glm::dvec3)(force / mass);
	speed += acc * deltaTime;
	position += speed * deltaTime;
	direction = glm::normalize(glm::rotate(direction, (float)(angularVel * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f)));
	speed = glm::rotate(speed, angularVel * deltaTime, glm::dvec3(0.0, 1.0, 0.0));
	ClearForces();
	UpdateVectors();
}

void Car::Turn(double angle, double deltaTime)
{
	if (angle > maxTurnAngle) angle = maxTurnAngle;
	else if (angle < -maxTurnAngle) angle = -maxTurnAngle;
	double dAngle = angle - currentTurnAngle;
	currentTurnAngle += dAngle * deltaTime * 3.0;
	if (currentTurnAngle > maxTurnAngle) currentTurnAngle = maxTurnAngle;
	else if (currentTurnAngle < -maxTurnAngle) currentTurnAngle = -maxTurnAngle;
	float speedAbs = glm::length(speed);
	angularVel = speedAbs / wheelsAxisDist * sin(currentTurnAngle * M_PI / 180.0) * 180.0 / M_PI;
}

void Car::SetGearRatio(float ratio)
{
	if (ratio < 0.0f)
		ratio *= -1;
	this->gearRatio = ratio;
}

void Car::AddResistanceForces()
{
	glm::dvec3 longSpeed = GetSpeedProjToDirection();
	glm::vec3 speedDir = (glm::vec3)speed;
	if (glm::length(speedDir) != 0.0f)
		speedDir = glm::normalize(speedDir);

	//	Drag
	float cDrag = 0.42f;
	glm::vec3 forceVec = (glm::vec3)(-speed * glm::length(speed)) * cDrag;
	AddForce(forceVec);

	// Rubbing
	float cRub = cDrag * 30.0f;
	forceVec = (glm::vec3)(-longSpeed) * cRub;
	AddForce(forceVec);

	//	Friction
	forceVec = -speedDir * mass * 9.81f * 0.25f * (1.1f - glm::abs(glm::dot(speedDir, direction)));
	AddForce(forceVec);

	//	Centrifugal
	if (currentTurnAngle != 0.0)
	{
		double radius = wheelsAxisDist / glm::sin(glm::radians(glm::abs(currentTurnAngle)));
		forceVec = glm::rotate(longSpeed, -90.0 * currentTurnAngle / abs(currentTurnAngle), glm::dvec3(0.0, 1.0, 0.0)) *
			glm::length(longSpeed) * double(mass) / radius;
		AddForce(forceVec);
	}
}

float Car::GetEngineForce()
{
	return maxTorque * 2500.0f / 7022.0f;
}

float Car::GetDriveForce()
{
	return maxTorque * gearRatio * diffRatio * transEfficiency / wheelsRadius;
}

float Car::GetWheelsAxisDist()
{
	return wheelsAxisDist;
}

double Car::GetCurrentTurnAngle()
{
	return currentTurnAngle;
}

double Car::GetMaxTurnAngle()
{
	return maxTurnAngle;
}

void Car::ProcessInput(const std::vector<Key>& keys, Mouse& mouse, double dTime)
{
	//	Обработка нажатых клавиш
	if (keys[int(KeysEnum::W)].state == KeyState::PRESS)
	{
		SetGearRatio(GEAR_RATIO_1);
		AddForce(direction * GetDriveForce());
	}
	if (keys[int(KeysEnum::S)].state == KeyState::PRESS)
	{
		SetGearRatio(GEAR_RATIO_R);
		AddForce(-direction * GetDriveForce());
	}
	if (keys[int(KeysEnum::A)].state == KeyState::PRESS)
	{
		Turn(40.0 / (glm::length(GetSpeedProjToDirection()) / 2.0 + 0.001), dTime);
	}
	if (keys[int(KeysEnum::D)].state == KeyState::PRESS)
	{
		Turn(-40.0 / (glm::length(GetSpeedProjToDirection()) / 2.0 + 0.001), dTime);
	}
	if (keys[int(KeysEnum::F)].state == KeyState::PRESS)
	{
		auto headlight = lights.find("headlight_left")->second;
		headlight->Enable(!headlight->IsEnabled());
		headlight = lights.find("headlight_right")->second;
		headlight->Enable(!headlight->IsEnabled());
	}
	if (keys[int(KeysEnum::SPACE)].state == KeyState::PRESS)
	{
		glm::vec3 longSpeedDir = (glm::vec3)GetSpeedProjToDirection();
		if (glm::length(longSpeedDir) != 0.0f)
			longSpeedDir = glm::normalize(longSpeedDir);
		AddForce(-longSpeedDir * GetDriveForce() * 2.7f);
	}
	//	Обработка отпускания клавиш
	if (keys[int(KeysEnum::A)].state == KeyState::UNPRESS &&
		keys[int(KeysEnum::D)].state == KeyState::UNPRESS)
	{
		Turn(0.0, dTime);
	}

}