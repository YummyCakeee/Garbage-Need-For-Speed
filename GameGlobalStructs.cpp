#include "GameGlobalStructs.h"

Mouse::Mouse()
{
	offset = glm::dvec2(0.0);
	scrollOffset = glm::dvec2(0.0);
	position = glm::dvec2(0.0);
}

Mouse::Mouse(glm::dvec2 position)
{
	offset = glm::dvec2(0.0);
	scrollOffset = glm::dvec2(0.0);
	this->position = position;
}

void Mouse::UpdatePosition(glm::dvec2 position)
{
	offset = position - this->position;
	this->position = position;
}

void Mouse::SetScrollOffset(glm::dvec2 offset)
{
	scrollOffset = offset;
}

glm::dvec2 Mouse::GetPosition()
{
	return position;
}

glm::dvec2 Mouse::GetOffset()
{
	return offset;
}

glm::dvec2 Mouse::GetScrollOffset()
{
	return scrollOffset;
}

void Mouse::ResetOffsets()
{
	offset = glm::dvec2(0.0);
	scrollOffset = glm::dvec2(0.0);
}