#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>
#include "Object.h"
#include "Camera.h"

class ParticleSystem : public Object
{
private:
	class Particle : public Object
	{
	private:
		glm::dvec3 acceleration;
		float opacity;
		float lifetime;
		double elapsedTime;
		bool alive;
		ParticleSystem* const system;
	public:
		Particle(ParticleSystem* system);
		Particle(ParticleSystem* system, glm::vec3 position, glm::vec3 acceleration);
		Particle(ParticleSystem* system, glm::vec3 position, glm::vec3 acceleration, float lifetime);
		Particle(ParticleSystem* system, glm::vec3 position, glm::vec3 speed, glm::vec3 acceleration);
		Particle(ParticleSystem* system, glm::vec3 position, glm::vec3 speed, glm::vec3 acceleration, float lifetime);
		Particle(ParticleSystem* system, glm::vec3 position, glm::vec3 speed, glm::vec3 acceleration, float opacity, float lifetime);
		void Update(double dTime);
		void SetSpeed(glm::vec3 speed);
		void SetAcceleration(glm::vec3 acceleration);
		void SetOpacity(float opacity);
		void SetLifetime(float lifetime);
		bool IsAlive();
	};
	std::vector<Particle*> particles;
	std::vector<Model*> particleModels;
	bool externalFactors;
	bool gravitation;
	float spawnInterval;
	unsigned int spawnCount;
	unsigned int maxCount;
	glm::vec3 offset;
	float offsetRandom;
	double _timer;
	glm::vec3 particlesAccel;
	glm::vec3 particlesScale;
	const Camera* targetCamera;
	glm::vec3 systemOffsetOfTarget;
	void UpdateWithBoundCamera();
public:
	ParticleSystem();
	ParticleSystem(float spawnInterval = 0.2f, unsigned int spawnCount = 5, unsigned int maxCount = 100,
		float gravitation = true, float externalFactors = false, glm::vec3 offset = glm::vec3(0.0f), float offsetRandom = 0.0f);
	void SetOffsetRandom(float offsetRandom);
	void SetParticlesScale(glm::vec3 scale);
	void SetParticlesAcceleration(glm::vec3 acceleration);
	void AddParticleModel(Model* model);
	void ResetParticleModels();
	void BindToCamera(const Camera* camera, glm::vec3 systemOffset = glm::vec3(0.0f));
	void UnbindFromCamera();
	virtual void Update(double dTime) override;
	virtual void Draw() override;
};

