#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch)
{
	this->position = position;
	this->worldUp = worldUp;
	this->yaw = yaw;
	this->pitch = pitch;
	aspectRatio = 16.f / 9.f;
	zNear = 0.1f;
	zFar = 1000.0f;
	speed = 5.0f;
	sensivity = 0.2f;
	fov = 45.0f;
	_targetPosNone = glm::vec3(0.0f);
	targetPos = &_targetPosNone;
	mode = CameraMode::UNIMPLEMENTED;
	UpdateCameraVectors();
}
Camera::Camera(float poxX, float posY, float posZ, float worldUpX, float worldUpY, float worldUpZ, float yaw, float pitch)
{
	position = glm::vec3(poxX, posY, posZ);
	worldUp = glm::vec3(worldUpX, worldUpY, worldUpZ);
	this->yaw = yaw;
	this->pitch = pitch;
	aspectRatio = 16.f / 9.f;
	zNear = 0.1f;
	zFar = 1000.0f;
	speed = 5.0f;
	sensivity = 0.2f;
	fov = 45.0f;
	_targetPosNone = glm::vec3(0.0f);
	targetPos = &_targetPosNone;
	mode = CameraMode::UNIMPLEMENTED;
	UpdateCameraVectors();
}

glm::vec3 Camera::GetPosition() const
{
	return position;
}

glm::vec3 Camera::GetFront() const
{
	return front;
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
	return glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);
}

void Camera::Move(int32_t directions, float dTime)
{
	glm::vec3 dir = glm::vec3(0.0f);
	dir.z = (directions & CameraMove::FORWARD) / CameraMove::FORWARD -
		(directions & CameraMove::BACKWARD) / CameraMove::BACKWARD;
	dir.x = (directions & CameraMove::RIGHT) / CameraMove::RIGHT -
		(directions & CameraMove::LEFT) / CameraMove::LEFT;
	dir.y = (directions & CameraMove::UP) / CameraMove::UP -
		(directions & CameraMove::DOWN) / CameraMove::DOWN;
	if (glm::length(dir) != 0.0f)
		dir = glm::normalize(dir);

	position += front * dir.z * speed * dTime;
	position += right * dir.x * speed * dTime;
	position += up * dir.y * speed * dTime;

	UpdateCameraVectors();
}

void Camera::Rotate(float xOffset, float yOffset, bool constrainPitch, float pitchLimit)
{
	yaw += xOffset * sensivity;
	pitch += yOffset * sensivity;

	if (constrainPitch && pitchLimit > 0.0f)
	{
		if (pitch > pitchLimit)
			pitch = pitchLimit;
		if (pitch < -pitchLimit)
			pitch = -pitchLimit;
	}

	UpdateCameraVectors();
}

void Camera::ChangeFov(float value)
{
	fov -= value;
	if (fov < 1.0f) fov = 1.0f;
	else if (fov > 120.0f) fov = 120.0f;
	UpdateCameraVectors();
}

void Camera::SetFov(float value)
{
	if (value < 1.0f || value > 120.0f) return;
	fov = value;
	UpdateCameraVectors();
}

void Camera::BindToTarget(const glm::vec3* position)
{
	targetPos = position;
}

void Camera::UnbindFromTarget()
{
	_targetPosNone = glm::vec3(*targetPos);
	targetPos = &_targetPosNone;
}

void Camera::UpdateCameraVectors()
{
}

void Camera::ProcessInput(const std::vector<Key>& keys, Mouse& mouse, double dTime)
{
}

CameraFPM::CameraFPM(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch) :
	Camera(position, worldUp, yaw, pitch)
{
	mode = CameraMode::FIRSTPERSON;
	offset = glm::vec3(0.0f);
}

CameraFPM::CameraFPM(float poxX, float posY, float posZ, float worldUpX, float worldUpY, float worldUpZ,
	float yaw, float pitch) : Camera(poxX, posY, posZ, worldUpX, worldUpY, worldUpZ, yaw, pitch)
{
	mode = CameraMode::FIRSTPERSON;
	offset = glm::vec3(0.0f);
}

void CameraFPM::UpdateCameraVectors()
{
	position = *targetPos + offset;
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	this->front = glm::normalize(front);
	right = glm::normalize(glm::cross(this->front, worldUp));
	up = glm::normalize(glm::cross(this->right, front));
}

void CameraFPM::ProcessInput(const std::vector<Key>& keys, Mouse& mouse, double dTime)
{
	glm::dvec2 offset = mouse.GetOffset();
	if (offset != glm::dvec2(0.0))
		Rotate(offset.x, -offset.y, false);
}

CameraTPM::CameraTPM(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch) :
	Camera(position, worldUp, yaw, pitch)
{
	mode = CameraMode::THIRDPERSON;
	radius = 5;
}

CameraTPM::CameraTPM(float poxX, float posY, float posZ, float worldUpX, float worldUpY, float worldUpZ,
	float yaw, float pitch) : Camera(poxX, posY, posZ, worldUpX, worldUpY, worldUpZ, yaw, pitch)
{
	mode = CameraMode::THIRDPERSON;
	radius = 5;
}

void CameraTPM::UpdateCameraVectors()
{
	glm::vec3 front;
	position.x = targetPos->x + offset.x + cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius;
	position.y = targetPos->y + offset.y + sin(glm::radians(pitch)) * radius;
	position.z = targetPos->z + offset.z + sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * radius;
	front = *targetPos - position;
	this->front = glm::normalize(front);
	right = glm::normalize(glm::cross(this->front, worldUp));
	up = glm::normalize(glm::cross(this->right, front));
}

void CameraTPM::ProcessInput(const std::vector<Key>& keys, Mouse& mouse, double dTime)
{
	glm::dvec2 offset = mouse.GetOffset();
	if (offset != glm::dvec2(0.0))
		Rotate(offset.x, -offset.y, false);
	offset = mouse.GetScrollOffset();
	if (offset.y != 0.0)
		ChangeRadius(offset.y);
}

void CameraTPM::ChangeRadius(float offset)
{
	radius += offset * sensivity;
	if (radius > maxRadius) radius = maxRadius;
	else if (radius < minRadius) radius = minRadius;
}

CameraFM::CameraFM(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch) :
	Camera(position, worldUp, yaw, pitch)
{
	mode = CameraMode::FREE;
}

CameraFM::CameraFM(float poxX, float posY, float posZ, float worldUpX, float worldUpY, float worldUpZ,
	float yaw, float pitch) : Camera(poxX, posY, posZ, worldUpX, worldUpY, worldUpZ, yaw, pitch)
{
	mode = CameraMode::FREE;
}

void CameraFM::UpdateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	this->front = glm::normalize(front);
	right = glm::normalize(glm::cross(this->front, worldUp));
	up = glm::normalize(glm::cross(this->right, front));
}

void CameraFM::ProcessInput(const std::vector<Key>& keys, Mouse& mouse, double dTime)
{
	int32_t dirs = 0;
	if (keys[int(KeysEnum::UP)].state == KeyState::PRESS)
	{
		dirs |= CameraMove::FORWARD;
	}
	if (keys[int(KeysEnum::DOWN)].state == KeyState::PRESS)
	{
		dirs |= CameraMove::BACKWARD;
	}
	if (keys[int(KeysEnum::LEFT)].state == KeyState::PRESS)
	{
		dirs |= CameraMove::LEFT;
	}
	if (keys[int(KeysEnum::RIGHT)].state == KeyState::PRESS)
	{
		dirs |= CameraMove::RIGHT;
	}
	Move(dirs, dTime);

	glm::dvec2 offset = mouse.GetOffset();
	if (offset != glm::dvec2(0.0))
		Rotate(offset.x, -offset.y, false);
	offset = mouse.GetScrollOffset();
	if (offset.y != 0.0)
		ChangeFov(offset.y);
}