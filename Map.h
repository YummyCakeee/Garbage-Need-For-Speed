#pragma once
#include <vector>
#include <fstream>
#include <filesystem>
#include <time.h>
#include <map>
#include <string>
#include "Object.h"
#include "LightSource.h"
#include "Camera.h"
#include "Bot.h"
#include "Shader.h"
#include "ParticleSystem.h"
#include <tinyxml2/tinyxml2.h>

class Map
{
private:
	std::vector<Object*> objects;
	std::vector<Bot*> bots;
	std::vector<Object*> roadObjects;
	std::vector<LightSource*> lights;
	LightsUBO lightsUbo;
	std::vector<const LightSource*> activeLights;
	std::map<std::string, Model*> models;
	std::map<std::string, Shader*> shaders;
	unsigned int botsCount;
	unsigned int treesCount;
	Object* player = NULL;
	Camera* camera = NULL;
	Object* skybox = NULL;
	void RenderSkybox();
	bool LoadGameProps();
public:
	Map();
	Object* GetPlayer();
	std::vector<Object*>& GetObjects();
	Camera* GetCamera();
	Object* GetSkybox();
	void Initialize();
	void AddObject(Object* object);
	void AddBot(Bot* bot);
	void ActBots(double dTime);
	void SetPlayer(Object* player);
	void SetCamera(Camera* camera);
	void SetRoadObject(Object* object);
	void SetSkybox(Object* skybox);
	void UpdateObjects(double dTime);
	void Render();
	void Update();
	void QuickCameraSetUp(Camera* camera);
	void Clear();
};