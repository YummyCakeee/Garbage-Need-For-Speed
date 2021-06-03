#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>
#include "Object.h"
#include "GameGlobalStructs.h"
#include "Map.h"

class GameGlobal
{
private:
	double dTime;
	double timeCoef;
	int windowWidth;
	int windowHeight;
	std::vector<Key> keys;
	void InitKeys();
	Map* map = NULL;
public:
	Mouse mouse;
	GameGlobal();
	GameGlobal(double dTime, glm::ivec2 windowSize);
	GameGlobal(double dTime, int windowWidth, int windowHeight);
	void SetDeltaTime(double dTime);
	void SetWindowSize(glm::ivec2 size);
	void SetWindowSize(int width, int height);
	void SetKeyState(KeysEnum key, KeyState state);
	void SetPlayer(Object* player);
	void SetMap(Map* map);
	void SetTimeCoef(double coefficient);
	void ChangeTimeCoef(double changingValue);
	double GetDeltaTime();
	glm::ivec2 GetWindowSize();
	int GetWindowWidth();
	int GetWindowHeight();
	Map* GetMap();
	void ProcessInput();
};


