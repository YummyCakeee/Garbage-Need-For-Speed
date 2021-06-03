#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

enum class KeysEnum
{
	W, S, A, D, E, F, V, J, K, L, UP, DOWN, LEFT, RIGHT, SPACE, ESC, ENTER
};

enum class KeyState
{
	PRESS, RELEASE, UNPRESS
};

struct Key
{
	KeysEnum key;
	KeyState state;
};

class Mouse
{
private:
	glm::dvec2 offset;
	glm::dvec2 position;
	glm::dvec2 scrollOffset;
public:
	Mouse();
	Mouse(glm::dvec2 position);
	void UpdatePosition(glm::dvec2 position);
	void SetScrollOffset(glm::dvec2 offset);
	glm::dvec2 GetPosition();
	glm::dvec2 GetOffset();
	glm::dvec2 GetScrollOffset();
	void ResetOffsets();
};