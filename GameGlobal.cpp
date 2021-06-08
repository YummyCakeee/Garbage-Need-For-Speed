#include "GameGlobal.h"

GameGlobal::GameProperties::GameProperties()
{
	gammaCorrection = true;
	gammaCorValue = 2.2f;
	windowWidth = 100;
	windowHeight = 100;
}

GameGlobal::GameProperties::GameProperties(int windowWidth, int windowHeight)
{
	gammaCorrection = true;
	gammaCorValue = 2.2f;
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
}

GameGlobal::GameProperties::GameProperties(glm::ivec2 windowSize)
{
	gammaCorrection = true;
	gammaCorValue = 2.2f;
	windowWidth = windowSize.x;
	windowHeight = windowSize.y;
}

GameGlobal::GameProperties::~GameProperties()
{
}

void GameGlobal::GameProperties::SetWindowSize(glm::ivec2 size)
{
	SetWindowSize(size.x, size.y);
}

void GameGlobal::GameProperties::SetWindowSize(int width, int height)
{
	windowWidth = width;
	if (windowWidth <= 0) windowWidth = 100;
	windowHeight = height;
	if (windowHeight <= 0) windowHeight = 100;
}

glm::ivec2 GameGlobal::GameProperties::GetWindowSize()
{
	return glm::ivec2(windowWidth, windowHeight);
}

int GameGlobal::GameProperties::GetWindowWidth()
{
	return windowWidth;
}

int GameGlobal::GameProperties::GetWindowHeight()
{
	return windowHeight;
}

void GameGlobal::GameProperties::EnableGammaCorrection(bool enable)
{
	gammaCorrection = enable;
}

void GameGlobal::GameProperties::SetGammaCorrectionValue(float value)
{
	gammaCorValue = glm::clamp(value, 1.0f, 3.0f);
}

bool GameGlobal::GameProperties::IsGammaCorrectionEnabled()
{
	return gammaCorrection;
}

GameGlobal::GameGlobal() : gameProps(100, 100)
{
	SetDeltaTime(0.0);
	InitKeys();
	mouse = Mouse(glm::dvec2(100.0));
	timeCoef = 1.0;
}

GameGlobal::GameGlobal(double dTime, glm::ivec2 windowSize) : gameProps(windowSize)
{
	SetDeltaTime(dTime);
	InitKeys();
	mouse = Mouse(glm::dvec2(windowSize) / 2.0);
	timeCoef = 1.0;
}
GameGlobal::GameGlobal(double dTime, int windowWidth, int windowHeight) : gameProps(windowWidth, windowHeight)
{
	SetDeltaTime(dTime);
	InitKeys();
	mouse = Mouse(glm::dvec2(windowWidth, windowHeight) / 2.0);
	timeCoef = 1.0;
}

double GameGlobal::GetDeltaTime()
{
	return dTime * timeCoef;
}

void GameGlobal::SetKeyState(KeysEnum key, KeyState state)
{
	if (state > (KeyState)3) return;
	for (int i = 0; i < keys.size(); i++)
	{
		if (keys[i].key == key)
			keys[i].state = state;
	}
}

void GameGlobal::SetDeltaTime(double dTime)
{
	if (dTime >= 0.0) this->dTime = dTime;
}

void GameGlobal::SetMap(Map* map)
{
	this->map = map;
}

void GameGlobal::SetTimeCoef(double coefficient)
{
	timeCoef = glm::clamp(coefficient, 0.001, 100.0);
}

void GameGlobal::ChangeTimeCoef(double changingValue)
{
	SetTimeCoef(timeCoef + changingValue);
}

void GameGlobal::InitKeys()
{
	for (int i = 0; i <= static_cast<int>(KeysEnum::ENTER); i++)
	{
		Key key;
		key.key = (KeysEnum)i;
		key.state = KeyState::UNPRESS;
		keys.push_back(key);
	}
}

void GameGlobal::ProcessInput()
{
	Camera* camera = map->GetCamera();
	if (camera != NULL)
		camera->ProcessInput(keys, mouse, dTime);
	Object* player = map->GetPlayer();
	if (player != NULL)
		player->ProcessInput(keys, mouse, dTime);
	mouse.ResetOffsets();

	if (keys[int(KeysEnum::J)].state == KeyState::PRESS)
	{
		ChangeTimeCoef(-0.05);
	}
	if (keys[int(KeysEnum::K)].state == KeyState::PRESS)
	{
		SetTimeCoef(1.0);
	}
	if (keys[int(KeysEnum::L)].state == KeyState::PRESS)
	{
		ChangeTimeCoef(0.05);
	}
	if (keys[int(KeysEnum::ENTER)].state == KeyState::RELEASE)
	{
		gameProps.EnableGammaCorrection(!gameProps.IsGammaCorrectionEnabled());
	}
	for (auto it = keys.begin(); it != keys.end(); it++)
	{
		if (it->state == KeyState::RELEASE)
			it->state = KeyState::UNPRESS;
	}
}

void GameGlobal::SetPlayer(Object* player)
{
	if (map != NULL)
		map->SetPlayer(player);
}

Map* GameGlobal::GetMap()
{
	return map;
}
