#include "ParticleSystem.h"

ParticleSystem::Particle::Particle(ParticleSystem* system) :
	system(system), Object(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f))
{
	acceleration = glm::vec3(0.0f, 0.3f, 0.0f);
	speed = glm::vec3(0.0f);
	opacity = 1.0f;
	elapsedTime = 0.0f;
	lifetime = 1.0f;
	alive = true;
}

ParticleSystem::Particle::Particle(ParticleSystem* system, glm::vec3 position, glm::vec3 acceleration) :
	system(system), Object(position, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f))
{
	this->acceleration = acceleration;
	speed = glm::vec3(0.0f);
	opacity = 1.0f;
	elapsedTime = 0.0f;
	lifetime = 1.0f;
	alive = true;
}

ParticleSystem::Particle::Particle(ParticleSystem* system, glm::vec3 position, glm::vec3 speed, glm::vec3 acceleration) :
	system(system), Object(position, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f))
{
	this->acceleration = acceleration;
	this->speed = speed;
	opacity = 1.0f;
	elapsedTime = 0.0f;
	lifetime = 1.0f;
	alive = true;
}

ParticleSystem::Particle::Particle(ParticleSystem* system, glm::vec3 position, glm::vec3 acceleration,
	float lifetime) : system(system), Object(position, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f))
{
	this->acceleration = acceleration;
	this->speed = glm::vec3(0.0f);
	SetOpacity(opacity);
	elapsedTime = 0.0f;
	SetLifetime(lifetime);
	alive = true;
}

ParticleSystem::Particle::Particle(ParticleSystem* system, glm::vec3 position, glm::vec3 speed, glm::vec3 acceleration,
	float lifetime) : system(system), Object(position, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f))
{
	this->acceleration = acceleration;
	this->speed = speed;
	opacity = 1.0f;
	elapsedTime = 0.0f;
	SetLifetime(lifetime);
	alive = true;
}

ParticleSystem::Particle::Particle(ParticleSystem* system, glm::vec3 position, glm::vec3 speed, glm::vec3 acceleration,
	float opacity, float lifetime) : system(system), Object(position, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f))
{
	this->acceleration = acceleration;
	this->speed = speed;
	SetOpacity(opacity);
	elapsedTime = 0.0f;
	SetLifetime(lifetime);
	alive = true;
}

void ParticleSystem::Particle::Update(double dTime)
{
	elapsedTime += dTime;
	if (elapsedTime >= lifetime)
	{
		alive = false;
		return;
	}
	speed += acceleration * dTime;
	position += speed * dTime;
	return;
}

void ParticleSystem::Particle::SetSpeed(glm::vec3 speed)
{
	this->speed = speed;
}

void ParticleSystem::Particle::SetAcceleration(glm::vec3 acceleration)
{
	this->acceleration = acceleration;
}

void ParticleSystem::Particle::SetOpacity(float opacity)
{
	this->opacity = glm::clamp(opacity, 0.0f, 1.0f);
}

void ParticleSystem::Particle::SetLifetime(float lifetime)
{
	this->lifetime = glm::max(0.0f, lifetime);
}

bool ParticleSystem::Particle::IsAlive()
{
	return alive;
}

ParticleSystem::ParticleSystem()
{
	spawnInterval = 0.5f;
	spawnCount = 5;
	maxCount = 500;
	gravitation = true;
	externalFactors = false;
	offset = glm::vec3(0.0f);
	offsetRandom = 0.0f;
	particlesScale = glm::vec3(1.0f);
	particlesAccel = glm::vec3(0.0f, 0.3f, 0.0f);
	systemOffsetOfTarget = glm::vec3(0.0f);
	_timer = 0.0;
	targetCamera = NULL;
}

ParticleSystem::ParticleSystem(float spawnInterval, unsigned int spawnCount, unsigned int maxCount,
	float gravitation, float externalFactors, glm::vec3 offset, float offsetRandom)
{
	this->spawnInterval = spawnInterval;
	this->spawnCount = spawnCount;
	this->maxCount = maxCount;
	this->gravitation = gravitation;
	this->externalFactors = externalFactors;
	this->offset = offset;
	SetOffsetRandom(offsetRandom);
	particlesAccel = glm::vec3(0.0f);
	particlesScale = glm::vec3(1.0f);
	_timer = 0.0;
	targetCamera = NULL;
}

void ParticleSystem::SetOffsetRandom(float offsetRandom)
{
	this->offsetRandom = glm::clamp(offsetRandom, 0.0f, 1.0f);
}

void ParticleSystem::SetParticlesScale(glm::vec3 scale)
{
	if (scale.x >= 0 && scale.y >= 0 && scale.z >= 0)
		particlesScale = scale;
	else particlesScale = glm::vec3(1.0f);
}

void ParticleSystem::SetParticlesAcceleration(glm::vec3 acceleration)
{
	particlesAccel = acceleration;
}

void ParticleSystem::AddParticleModel(Model* model)
{
	particleModels.push_back(model);
}

void ParticleSystem::ResetParticleModels()
{
	particleModels.clear();
}

void ParticleSystem::BindToCamera(const Camera* camera, glm::vec3 systemOffset)
{
	this->targetCamera = camera;
	systemOffsetOfTarget = systemOffset;
}

void ParticleSystem::UnbindFromCamera()
{
	this->targetCamera = NULL;
}

void ParticleSystem::UpdateWithBoundCamera()
{
	if (targetCamera == NULL) return;
	glm::mat3 camModel = glm::mat3(1.0f);
	if (glm::length(systemOffsetOfTarget) != 0.0f)
	{
		glm::vec3 camFront = targetCamera->GetFront();
		camFront.y = 0.0f;
		camFront = glm::normalize(camFront);
		glm::vec3 crossRes = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), camFront);
		if (crossRes != glm::vec3(0.0f))
		{
			float angle = glm::acos(glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), camFront));
			glm::quat dir = glm::angleAxis(angle, glm::normalize(crossRes));
			camModel = camModel * glm::mat3_cast(dir);
		}
		else if (glm::vec3(-1.0f, 0.0f, 0.0f) == camFront)
		{
			glm::quat dir = glm::angleAxis(glm::radians(180.0f), up);
			camModel = camModel * glm::mat3_cast(dir);
		}
	}
	position = targetCamera->GetPosition() + targetCamera->GetFront() + camModel * systemOffsetOfTarget;
}

void ParticleSystem::Update(double dTime)
{
	UpdateWithBoundCamera();
	auto random = [](float randomChance)
	{return (rand() % (int)(101 - (1.0f - randomChance) * 100) + ((1.0f - randomChance) * 100.0f)) / 100.0f; };
	_timer += dTime;
	if (_timer >= spawnInterval)
	{
		_timer -= spawnInterval;
		glm::vec3 gravi = glm::vec3(0.0f);
		for (int i = 0; i < spawnCount; i++)
		{
			int signX = 1 - 2 * (rand() % 2);
			int signY = 1 - 2 * (rand() % 2);
			int signZ = 1 - 2 * (rand() % 2);
			if (gravitation) gravi.y = -1.8f;
			Particle* particle = new Particle(this, position +
				glm::vec3(offset.x * random(offsetRandom) * signX, offset.y * random(offsetRandom) * signY,
					offset.z * random(offsetRandom) * signZ), particlesAccel + gravi, 5.0f);
			particle->SetScale(particlesScale);
			if (particleModels.size() > 0)
				particle->SetModel(particleModels[rand() % particleModels.size()]);
			particles.push_back(particle);
		}
	}
	for (auto it = particles.begin(); it != particles.end(); it++)
	{
		if (!(*it)->IsAlive())
			it = particles.erase(it);
	}
	if (particles.size() > maxCount)
		particles.erase(particles.begin(), particles.begin() + (particles.size() - maxCount));
	for (auto it = particles.begin(); it != particles.end(); it++)
	{
		(*it)->Update(dTime);
	}
}

void ParticleSystem::Draw()
{
	if (model != NULL)
	{
		UpdateModelProps();
		model->Draw();
	}
	else
	{
		//std::cout << "WARNING:: Can't draw object without model set: " << this->id << std::endl;
	}
	for (auto it = particles.begin(); it != particles.end(); it++)
	{
		(*it)->Draw();
	}
}