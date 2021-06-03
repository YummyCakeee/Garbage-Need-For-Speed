#include "GameGlobal.h"

GameGlobal::GameGlobal()
{
	SetDeltaTime(0.0);
	SetWindowSize(100, 100);
	InitKeys();
	mouse = Mouse(glm::dvec2(100.0));
	timeCoef = 1.0;
}

GameGlobal::GameGlobal(double dTime, glm::ivec2 windowSize)
{
	SetDeltaTime(dTime);
	SetWindowSize(windowSize);
	InitKeys();
	mouse = Mouse(glm::dvec2(windowSize) / 2.0);
	timeCoef = 1.0;
}
GameGlobal::GameGlobal(double dTime, int windowWidth, int windowHeight)
{
	SetDeltaTime(dTime);
	SetWindowSize(windowWidth, windowHeight);
	InitKeys();
	mouse = Mouse(glm::dvec2(windowWidth, windowHeight) / 2.0);
	timeCoef = 1.0;
}

double GameGlobal::GetDeltaTime()
{
	return dTime * timeCoef;
}

glm::ivec2 GameGlobal::GetWindowSize()
{
	return glm::ivec2(windowWidth, windowHeight);
}

int GameGlobal::GetWindowWidth()
{
	return windowWidth;
}

int GameGlobal::GetWindowHeight()
{
	return windowHeight;
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

void GameGlobal::SetWindowSize(glm::ivec2 size)
{
	SetWindowSize(size.x, size.y);
}

void GameGlobal::SetWindowSize(int width, int height)
{
	windowWidth = width;
	if (windowWidth <= 0) windowWidth = 100;
	windowHeight = height;
	if (windowHeight <= 0) windowHeight = 100;
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
	for (int i = 0; i < static_cast<int>(KeysEnum::ENTER); i++)
	{
		Key key;
		key.key = (KeysEnum)i;
		key.state = KeyState::RELEASE;
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
