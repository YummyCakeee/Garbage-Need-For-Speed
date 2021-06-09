#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <time.h>
#include <map>
#include <string>
#include <tinyxml2/tinyxml2.h>
#include "Object.h"
#include "LightSource.h"
#include "Camera.h"
#include "Bot.h"
#include "Shader.h"
#include "ParticleSystem.h"
#include "GameGlobal.h"

class GameGlobal;

class Map
{
private:
	const GameGlobal* game;
	std::vector<Object*> objects;
	std::vector<Bot*> bots;
	std::vector<Object*> roadObjects;
	std::vector<LightSource*> lights;
	LightsUBO lightsUbo;
	std::vector<const LightSource*> activeLights;
	std::map<std::string, Model*> models;
	unsigned int botsCount;
	unsigned int treesCount;
	Object* player = NULL;
	Camera* camera = NULL;
	Object* skybox = NULL;
	void RenderSkybox();
	bool LoadGameProps();
	void UpdateObjects(double dTime);
	void ActBots(double dTime);
public:
	Map(const GameGlobal& gameGlob);
	Object* GetPlayer();
	std::vector<Object*>& GetObjects();
	Camera* GetCamera();
	Object* GetSkybox();
	void Initialize();
	void AddObject(Object* object);
	void AddBot(Bot* bot);
	void SetPlayer(Object* player);
	void SetCamera(Camera* camera);
	void SetRoadObject(Object* object);
	void SetSkybox(Object* skybox);
	void Render();
	void Update(float dTime);
	void QuickCameraSetUp(Camera* camera);
	void Clear();
};