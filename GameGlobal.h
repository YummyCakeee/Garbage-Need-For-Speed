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
	std::vector<Key> keys;
	void InitKeys();
	Map* map = NULL;
	class GameProperties
	{
	private:
		int windowWidth;
		int windowHeight;
		bool gammaCorrection = false;
		float gammaCorValue;
	public:
		GameProperties();
		GameProperties(int windowWidth, int windowHeight);
		GameProperties(glm::ivec2 windowSize);
		~GameProperties();
		void SetWindowSize(glm::ivec2 size);
		void SetWindowSize(int width, int height);
		void EnableGammaCorrection(bool enable);
		void SetGammaCorrectionValue(float value);
		glm::ivec2 GetWindowSize();
		int GetWindowWidth();
		int GetWindowHeight();
		bool IsGammaCorrectionEnabled();
	};
public:
	GameProperties gameProps;
	Mouse mouse;
	GameGlobal();
	GameGlobal(double dTime, glm::ivec2 windowSize);
	GameGlobal(double dTime, int windowWidth, int windowHeight);
	void SetDeltaTime(double dTime);
	void SetKeyState(KeysEnum key, KeyState state);
	void SetPlayer(Object* player);
	void SetMap(Map* map);
	void SetTimeCoef(double coefficient);
	void ChangeTimeCoef(double changingValue);
	double GetDeltaTime();
	Map* GetMap();
	void ProcessInput();
};



