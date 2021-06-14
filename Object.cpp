#include "Object.h"

unsigned int Object::idCounter = 0;

Object::Object()
{
	id = idCounter++;
	model = NULL;
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	direction = glm::vec3(1.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	worldUp = up;
	_startDirection = direction;
	speed = glm::vec3(0.0f, 0.0f, 0.0f);
	force = glm::vec3(0.0f, 0.0f, 0.0f);
	forces = std::vector<Force>();
}

Object::Object(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& scale)
{
	id = idCounter++;
	model = NULL;
	SetPosition(position);
	SetDirection(direction);
	SetScale(scale);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	UpdateVectors();
	_startDirection = direction;
	speed = glm::vec3(0.0f, 0.0f, 0.0f);
	force = glm::vec3(0.0f, 0.0f, 0.0f);
	forces = std::vector<Force>();
}

Object::Object(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& scale, const glm::vec3& worldUp)
{
	id = idCounter++;
	model = NULL;
	SetPosition(position);
	SetDirection(direction);
	SetScale(scale);
	SetWorldUp(worldUp);
	UpdateVectors();
	_startDirection = direction;
	speed = glm::vec3(0.0f, 0.0f, 0.0f);
	force = glm::vec3(0.0f, 0.0f, 0.0f);
	forces = std::vector<Force>();
}

void Object::AddForce(const glm::vec3& force)
{
	forces.push_back(Force(force));
}

void Object::RemoveForce(int index)
{
	if (index < forces.size() && index >= 0)
	{
		forces.erase(forces.begin() + index);
	}
}

void Object::SumForces()
{
	for (auto& i : forces)
	{
		force += i.GetForce();
	}
}

void Object::ClearForces()
{
	force = glm::vec3(0.0f);
	forces.clear();
}

unsigned int Object::GetID()
{
	return id;
}

Model* Object::GetModel()
{
	return model;
}

const glm::vec3* Object::GetPosition()
{
	return &position;
}

glm::vec3 Object::GetDirection()
{
	return direction;
}

glm::dvec3 Object::GetSpeed()
{
	return speed;
}

glm::dvec3 Object::GetSpeedProjToDirection()
{
	return glm::proj(speed, (glm::dvec3)direction);
}

glm::mat4 Object::GetModelMatrix()
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	glm::vec3 crossRes = glm::cross(_startDirection, direction);

	if (crossRes != glm::vec3(0.0f))
	{
		float angle = glm::acos(glm::dot(_startDirection, direction));
		glm::quat direction = glm::angleAxis(angle, glm::normalize(crossRes));
		model = model * glm::mat4_cast(direction);
	}
	else if (direction == -_startDirection)
	{
		glm::quat direction = glm::angleAxis(glm::radians(180.0f), up);
		model = model * glm::mat4_cast(direction);
	}
	model = glm::scale(model, scale);
	return model;
}

void Object::SetPosition(const glm::vec3& position)
{
	this->position = position;
}

void Object::SetDirection(const glm::vec3& direction)
{
	if (glm::length(direction) != 0.0f)
		this->direction = glm::normalize(direction);
	else this->direction = glm::vec3(1.0f, 0.0f, 0.0f);
	UpdateVectors();
	//_startDirection = direction;
}

void Object::SetScale(const glm::vec3& scale)
{
	if (scale.x >= 0.0f && scale.y >= 0 && scale.z >= 0)
		this->scale = scale;
	else this->scale = glm::vec3(1.0f);
}

void Object::SetModel(Model* model)
{
	this->model = model;
}

void Object::SetWorldUp(glm::vec3 up)
{
	if (glm::length(direction) != 0.0f)
		this->direction = glm::normalize(direction);
	else this->direction = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Object::UpdateModelProps()
{
	model->SetWorldPosition(*GetPosition());
	model->SetRotation(GetDirection());
	model->SetScaleMultiplicator(scale);
}

void Object::BindLightSource(const std::string& name, MovingLight* light)
{
	if (!light->IsBindToObject() && lights.find(name) == lights.end())
	{
		light->SetBind(true);
		lights.insert(std::make_pair(name, light));
	}
}

void Object::UnbindLightSource(const std::string& name)
{
	auto elem = lights.find(name);
	if (elem != lights.end())
	{
		auto light = elem->second;
		light->SetBind(false);
		if (light->GetType() == SourceType::SPOTLIGHT)
			((SpotLight*)light)->ResetStartDirection();
		lights.erase(elem);
	}
}

void Object::UnbindAllLightSources()
{
	for (auto it = lights.begin(); it != lights.end(); it++)
	{
		auto light = it->second;
		light->SetBind(false);
		if (light->GetType() == SourceType::SPOTLIGHT)
			((SpotLight*)light)->ResetStartDirection();
	}
	lights.clear();
}

void Object::UpdateLightsPositions()
{
	glm::mat4 model = GetModelMatrix();
	for (auto it = lights.begin(); it != lights.end(); it++)
	{
		auto light = it->second;
		light->SetPosition(glm::vec3(model * glm::vec4(light->GetOffset(), 1.0f)));
		if (light->GetType() == SourceType::SPOTLIGHT)
			((SpotLight*)light)->SetDirection(glm::mat3(model) * ((SpotLight*)light)->GetStartDirection());
	}
}

void Object::UpdateVectors()
{
	glm::vec3 right = glm::normalize(glm::cross(direction, worldUp));
	up = glm::normalize(glm::cross(right, direction));
}

MovingLight* Object::GetLightSource(const std::string& name)
{
	return lights.find(name)->second;
}

void Object::Draw(const Shader* shader)
{
	if (model != NULL)
	{
		UpdateModelProps();
		if (shader == NULL)
			model->Draw();
		else model->Draw(*shader);
	}
	else
	{
		std::cout << "WARNING:: Can't draw object without model set: " << this->id << std::endl;
	}
}

void Object::ProcessInput(const std::vector<Key>& keys, Mouse& mouse, double dTime) {}

void Object::Move(double dTime) {}

void Object::Update(double dTime)
{
	Move(dTime);
	UpdateLightsPositions();
}