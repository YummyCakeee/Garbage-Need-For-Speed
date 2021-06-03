#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "GameGlobalStructs.h"

enum CameraMove
{
	FORWARD = 1,
	BACKWARD = 2,
	RIGHT = 4,
	LEFT = 8,
	UP = 16,
	DOWN = 32,
};

enum class CameraMode
{
	FIRSTPERSON,
	THIRDPERSON,
	FREE,
	UNIMPLEMENTED
};


class Camera
{
protected:
	float fov;
	float zNear;
	float zFar;
	float aspectRatio;
	float speed;
	float sensivity;
	float yaw;
	float pitch;
	const glm::vec3* targetPos;
	glm::vec3 _targetPosNone;
	CameraMode mode;
public:
	glm::vec3 offset;
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = 90.0f, float pitch = 0.0f);
	Camera(float poxX, float posY, float posZ,
		float worldUpX, float worldUpY, float worldUpZ, float yaw, float pitch);
	glm::vec3 GetPosition() const;
	glm::vec3 GetFront() const;
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
	void Move(int32_t directions, float dTime);
	void Rotate(float xOffset, float yOffset, bool constrainPitch = true, float pitchLimit = 89.0f);
	void ChangeFov(float value);
	void SetFov(float value);
	void BindToTarget(const glm::vec3* position);
	void UnbindFromTarget();
	virtual void UpdateCameraVectors();
	virtual void ProcessInput(const std::vector<Key>& keys, Mouse& mouse, double dTime);
};

class CameraFPM sealed : public Camera
{
public:
	CameraFPM(glm::vec3 position = glm::vec3(0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = 90.0f, float pitch = 0.0f);
	CameraFPM(float poxX, float posY, float posZ,
		float worldUpX, float worldUpY, float worldUpZ, float yaw, float pitch);
	virtual void UpdateCameraVectors() override;
	virtual void ProcessInput(const std::vector<Key>& keys, Mouse& mouse, double dTime) override;
private:

};

class CameraTPM sealed : public Camera
{
public:
	CameraTPM(glm::vec3 position = glm::vec3(0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = 90.0f, float pitch = 0.0f);
	CameraTPM(float poxX, float posY, float posZ,
		float worldUpX, float worldUpY, float worldUpZ, float yaw, float pitch);
	virtual void UpdateCameraVectors() override;
	virtual void ProcessInput(const std::vector<Key>& keys, Mouse& mouse, double dTime) override;
	void ChangeRadius(float offset);
private:
	float radius;
	const float maxRadius = 5;
	const float minRadius = 2;
};

class CameraFM sealed : public Camera
{
public:
	CameraFM(glm::vec3 position = glm::vec3(0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = 90.0f, float pitch = 0.0f);
	CameraFM(float poxX, float posY, float posZ,
		float worldUpX, float worldUpY, float worldUpZ, float yaw, float pitch);
	virtual void UpdateCameraVectors() override;
	virtual void ProcessInput(const std::vector<Key>& keys, Mouse& mouse, double dTime) override;
private:

};
