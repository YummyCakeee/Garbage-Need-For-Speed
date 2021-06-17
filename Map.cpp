#include "Map.h"

Map::Map(const GameGlobal& gameGlob)
{
	game = &gameGlob;
	LoadGameProps();
	treesCount = 100;
	botsCount = 15;
}

void Map::RenderSkybox()
{
	if (skybox == NULL) return;
	glDisable(GL_DEPTH_TEST);
	skybox->GetModel()->GetMesh(0)->GetMaterial()->GetShader()->use();
	if (camera != NULL)
		skybox->SetPosition(camera->GetPosition());
	skybox->UpdateModelProps();
	skybox->Draw();
	glEnable(GL_DEPTH_TEST);
}

bool Map::LoadGameProps()
{
	//tinyxml2::XMLDocument doc;
	//doc.LoadFile("C:\\Users\\nekit\\Documents\\Visual Studio 2019\\Projects\\"
	//	"Garbage Need For Speed\\properties.xml");
	//tinyxml2::XMLElement* root = doc.RootElement();

	////	Bots count setup
	//tinyxml2::XMLElement* node = root->FirstChildElement("bots_pop");
	//if (node != NULL)
	//{
	//	try
	//	{
	//		int value = std::stoi(node->GetText());
	//		if (value < 0) value = abs(value);
	//		botsCount = (unsigned int)value;
	//	}
	//	catch (std::invalid_argument)
	//	{
	//		std::cout << "ERROR::Reading bots count from props file error." << std::endl;
	//		botsCount = 15;
	//	}
	//}
	////	Trees count setup
	//node = root->FirstChildElement("trees_count");
	//if (node != NULL)
	//{
	//	try
	//	{
	//		int value = std::stoi(node->GetText());
	//		if (value < 0) value = abs(value);
	//		treesCount = (unsigned int)value;
	//	}
	//	catch (std::invalid_argument)
	//	{
	//		std::cout << "ERROR::Reading trees count from props file error." << std::endl;
	//		treesCount = 100;
	//	}
	//}
	return true;
}

Object* Map::GetPlayer()
{
	return player;
}

void Map::Initialize()
{
	CameraTPM* camera = new CameraTPM(glm::vec3(0.0f, 0.0f, 2.0f));
	SetCamera(camera);
	//	Создание дороги

	std::vector<Texture> roadTextures;
	roadTextures.push_back(Texture(0, TextureType::Diffuse, "textures/road/asphalt_road_D.png"));
	roadTextures.push_back(Texture(0, TextureType::Specular, "textures/road/asphalt_road_wet_S.png"));
	Model* roadModel = Model::CreatePlane(7.0f, 7.0f, "road_section", &roadTextures, true);
	models.insert(std::make_pair("road_section", roadModel));
	roadModel->SetGlobalShader(game->shaders.find("standart")->second);
	roadModel->SetCamera(camera);
	Model* terrain[3] = {
		new Model(std::filesystem::canonical("models/Road Objects/terrain").string(), "terrain.obj", camera, glm::vec3(0.0f, 0.0f, -1.0f)) ,
		new Model(std::filesystem::canonical("models/Road Objects/terrain").string(), "terrain2.obj", camera, glm::vec3(0.0f, 0.0f, -1.0f)),
		new Model(std::filesystem::canonical("models/Road Objects/terrain").string(), "terrain3.obj", camera, glm::vec3(0.0f, 0.0f, -1.0f))
	};
	for (int i = 0; i < 3; i++)
	{
		terrain[i]->SetGlobalShader(game->shaders.find("standart")->second);
		terrain[i]->SetCamera(camera);
		Material* mat = terrain[i]->GetMesh(0)->GetMaterial();
		mat->SetProperty(MaterialProp::REFLECTIVITY, 0.0f);
		mat->SetColor(MaterialType::SPECULAR, glm::vec4(0.005f, 0.005f, 0.005f, 1.0f));
		models.insert(std::make_pair("terrain" + std::to_string(i + 1), terrain[i]));
	}
	for (int i = 0; i < 30; i++)
	{
		//	Road Section
		Object* roadSection = new Object(glm::vec3((i - 15) * 7.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f));
		roadSection->SetModel(roadModel);
		AddObject(roadSection);
		SetRoadObject(roadSection);
		//	Terrain
		Object* terrainSection = new Object(glm::vec3((i - 15) * 7.0f, -0.01f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f));
		terrainSection->SetModel(terrain[i % 3]);
		AddObject(terrainSection);
		SetRoadObject(terrainSection);
	}
	Model* StreetLightModel = new Model(std::filesystem::canonical("models/Road Objects/Street Light 5").string(),
		"street_light.obj", camera, glm::vec3(0.0f, 0.0f, -1.0f));
	StreetLightModel->SetScale(glm::vec3(0.085f));
	StreetLightModel->SetGlobalShader(game->shaders.find("standart")->second);
	models.insert(std::make_pair("street_light", StreetLightModel));
	for (int i = 0; i < 16; i++)
	{
		//	Street Light Left
		Object* streetLightLeft = new Object(glm::vec3((i - 8) * 14.0f, -0.1f, -3.7f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f));
		streetLightLeft->SetModel(StreetLightModel);
		AddObject(streetLightLeft);
		SetRoadObject(streetLightLeft);
		//	Adding Light Source
		SpotLight* lightLeft = new SpotLight(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.1f),
			glm::vec3(0.01f), glm::vec3(0.7f), glm::vec3(1.0f), 1.0f, 0.01f, 0.06f,
			glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(22.5f)));
		lightLeft->SetOffset(glm::vec3(0.0f, 5.0f, 1.0f));
		lights.push_back(lightLeft);
		streetLightLeft->BindLightSource("lamp", lightLeft);
		//	Street Light Right
		Object* streetLightRight = new Object(glm::vec3((i - 8) * 14.0f, -0.1f, 3.7f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f));
		streetLightRight->SetDirection(glm::vec3(-1.0f, 0.0f, 0.0f));
		streetLightRight->SetModel(StreetLightModel);
		AddObject(streetLightRight);
		SetRoadObject(streetLightRight);
		//	Adding Light Source
		SpotLight* lightRight = new SpotLight(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.1f),
			glm::vec3(0.01f), glm::vec3(0.7f), glm::vec3(1.0f), 1.0f, 0.01f, 0.06f,
			glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(22.5f)));
		lightRight->SetOffset(glm::vec3(0.0f, 5.0f, 1.0f));
		lights.push_back(lightRight);
		streetLightRight->BindLightSource("lamp", lightRight);

	}
	Model* trees[2] = {
	new Model(std::filesystem::canonical("models/Trees/tree7").string(), "Eastern Red Cedar_Low.obj", camera, glm::vec3(0.0f, 0.0f, -1.0f)),
	new Model(std::filesystem::canonical("models/Trees/tree8").string(), "Fraser Fir_Low.obj", camera, glm::vec3(0.0f, 0.0f, -1.0f))
	};
	trees[0]->SetScale(glm::vec3(0.0035f));
	trees[1]->SetScale(glm::vec3(0.01f));
	for (int i = 0; i < 2; i++)
	{
		trees[i]->SetGlobalShader(game->shaders.find("standart")->second);
		trees[i]->SetCamera(camera);
		//	Remove reflectivity
		std::vector<Mesh>* treeMeshes = trees[i]->GetMeshes();
		for (int j = 0; j < treeMeshes->size(); j++)
		{
			(*treeMeshes)[j].GetMaterial()->SetProperty(MaterialProp::REFLECTIVITY, 0.003f);
			(*treeMeshes)[j].GetMaterial()->SetProperty(MaterialProp::SHININESS, 256.0f);
			(*treeMeshes)[j].GetMaterial()->SetColor(MaterialType::SPECULAR, glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));
		}
		models.insert(std::make_pair("tree" + std::to_string(i + 1), trees[i]));
	}
	for (int i = 0, sign = 1; i < treesCount; i++)
	{
		Object* tree = new Object();
		tree->SetModel(trees[rand() % 2]);
		tree->SetDirection(glm::vec3((float)(rand() % 11) / 10.0f, 0.0f, (float)(rand() % 11) / 10.0f));
		tree->SetPosition(glm::vec3((rand() % 30 - 15) * 7.0f, 0.0f, sign * (rand() % 60 + 4.0f)));
		AddObject(tree);
		SetRoadObject(tree);
		sign *= -1;
	}

	//	Создание игрока
	Car* car = new Car(glm::vec3(1.0f, 0.0f, 0.0f), glm::dvec3(0.0f), glm::vec3(0.0f), 216.0f, 2000.0f, 50.0f, 50.0);
	SpotLight* lightLeft = new SpotLight(glm::vec3(0.0f), glm::vec3(0.8f, -0.3f, -0.1f),
		glm::vec3(0.004f), glm::vec3(0.9f), glm::vec3(1.0f), 1.0f, 0.09f, 0.2f,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(28.5f)));
	lightLeft->SetOffset(glm::vec3(1.0f, 0.3f, -0.28f));
	lights.push_back(lightLeft);
	car->BindLightSource("headlight_left", lightLeft);
	SpotLight* lightRight = new SpotLight(glm::vec3(0.0f), glm::vec3(0.8f, -0.3f, 0.25f),
		glm::vec3(0.004f), glm::vec3(0.9f), glm::vec3(1.0f), 1.0f, 0.09f, 0.2f,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(28.5f)));
	lightRight->SetOffset(glm::vec3(1.0f, 0.3f, 0.28f));
	lights.push_back(lightRight);
	car->BindLightSource("headlight_right", lightRight);
	Model* carModel = new Model(std::filesystem::canonical("models/2107").string(), "2107.obj", camera, glm::vec3(0.0f, 0.0f, -1.0f));
	carModel->SetGlobalShader(game->shaders.find("standart")->second);
	carModel->SetScale(glm::vec3(0.45f));
	models.insert(std::make_pair("vaz_2107", carModel));
	car->SetModel(carModel);
	AddObject(car);

	//	Создание ботов
	Model* botModels[3] =
	{
		new Model(std::filesystem::canonical("models/1997 Volkswagen Golf").string(),
		"1997_vw_golf_5door.obj", camera, glm::vec3(0.0f, 0.0f, 1.0f)),
		new Model(std::filesystem::canonical("models/Camry").string(), "Toyota Camry .obj", camera, glm::vec3(0.0f, 0.0f, 1.0f)),
		new Model(std::filesystem::canonical("models/Nissan 240SX").string(), "240sx.obj", camera, glm::vec3(0.0f, 0.0f, 1.0f))
	};

	botModels[0]->SetGlobalShader(game->shaders.find("standart")->second);
	botModels[0]->SetScale(glm::vec3(0.52f));
	models.insert(std::make_pair("volkswagen", botModels[0]));
	botModels[1]->SetGlobalShader(game->shaders.find("standart")->second);
	botModels[1]->SetScale(glm::vec3(0.83f));
	models.insert(std::make_pair("camry", botModels[1]));
	botModels[2]->SetGlobalShader(game->shaders.find("standart")->second);
	botModels[2]->SetScale(glm::vec3(0.55f));
	models.insert(std::make_pair("nissan", botModels[2]));
	for (int i = 0; i < botsCount; i++)
	{
		int sign = 1 - 2 * (rand() % 2);
		CarBot* bot = new CarBot(glm::vec3(sign * 1.0f, 0.0f, 0.0f), glm::dvec3(0.0f),
			glm::vec3((i - float(botsCount) / 2.0f) * 2.5f * 7.0f + (rand() % 9 - 4), 0.0f,
				sign * (1.0f + (rand() % 2) * 1.5f) + (rand() % 4 - 3) / 10.0f),
			216.0f, 2000.0f, 60.0f, 50.0);
		bot->SetModel(botModels[i % 3]);
		AddObject(bot);
		AddBot(bot);
	}

	//	Skybox
	std::vector<std::string> faces
	{
		"textures\\skybox\\field\\right.png", "textures\\skybox\\field\\left.png",
		"textures\\skybox\\field\\top.png", "textures\\skybox\\field\\bottom.png",
		"textures\\skybox\\field\\front.png", "textures\\skybox\\field\\back.png"
	};
	Model* skyboxModel = Model::CreateSkybox(faces);
	models.insert(std::make_pair("skybox", skyboxModel));
	skyboxModel->SetGlobalShader(game->shaders.find("skybox")->second);
	skyboxModel->SetCamera(camera);
	Object* skybox = new Object();
	skybox->SetModel(skyboxModel);
	skybox->SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
	SetSkybox(skybox);

	//	Глобальный свет
	DirLight* globalLight = new DirLight(glm::vec3(0.0f, -0.3f, -0.9f), glm::vec3(0.06f, 0.02f, 0.02f),
		glm::vec3(0.27f, 0.17f, 0.17f), glm::vec3(1.0f, 0.8f, 0.8f));
	lights.push_back(globalLight);

	//	Установка глобальных параметров
	SetPlayer(car);

	camera->BindToTarget(car->GetPosition());
	camera->offset = glm::vec3(0.0f, 0.2f, 0.0f);
	ParticleSystem* ps = new ParticleSystem(0.02f, 3, 600, true, false, glm::vec3(6.0f, 0.0f, 6.0f), 100.0f);
	ps->SetParticlesAcceleration(glm::vec3(0.0f, -0.2f, 0.0f));
	Model* rainDrop = new Model(std::filesystem::canonical("models/Other/raindrop").string(),
		"raindrop.obj", camera, glm::vec3(1.0f, 0.0f, 0.0f));
	rainDrop->SetGlobalShader(game->shaders.find("raindrop")->second);
	rainDrop->SetCamera(camera);
	rainDrop->GetMesh(0)->GetMaterial()->SetProperty(MaterialProp::ALPHA, 0.4f);
	rainDrop->GetMesh(0)->GetMaterial()->SetColor(MaterialType::DIFFUSE, glm::vec4(0.8f, 0.83f, 1.0f, 0.4f));
	models.insert(std::make_pair("rain_drop", rainDrop));
	ps->AddParticleModel(rainDrop);
	ps->SetParticlesScale(glm::vec3(0.08f));
	ps->BindToCamera(camera, glm::vec3(5.0f, 2.0f, 0.0f));
	AddObject(ps);

	//	Юниформ-буффер источников света
	lightsUbo = LightsUBO(*(game->shaders.find("standart")->second), 1, 4, 8);

	Texture skyboxTexture = skybox->GetModel()->GetMesh(0)->GetMaterial()->GetTextures()[0][0];
	for (auto it = models.begin(); it != models.end(); it++)
	{
		std::vector<Mesh>* mshs = it->second->GetMeshes();
		for (int i = 0; i < mshs->size(); i++)
		{
			(*mshs)[i].GetMaterial()->AddTexture(skyboxTexture);
		}
	}
}

void Map::AddObject(Object* object)
{
	objects.push_back(object);
}

std::vector<Object*>& Map::GetObjects()
{
	return objects;
}

void Map::SetPlayer(Object* player)
{
	this->player = player;
}

void Map::SetCamera(Camera* camera)
{
	this->camera = camera;
}

void Map::SetRoadObject(Object* object)
{
	roadObjects.push_back(object);
}

void Map::SetSkybox(Object* skybox)
{
	this->skybox = skybox;
}

Camera* Map::GetCamera()
{
	return camera;
}

Object* Map::GetSkybox()
{
	return skybox;
}

void Map::AddBot(Bot* bot)
{
	bots.push_back(bot);
}

void Map::UpdateObjects(double dTime)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dTime);
	}
}

void Map::ActBots(double dTime)
{
	for (int i = 0; i < bots.size(); i++)
	{
		bots[i]->Act(dTime);
	}
}

void Map::Render()
{	
	//	Отрисовка карт теней
	//	Привязка буфера глубины
	game->depthBuffer->Bind();
	game->depthBuffer->PrepareForRender();
	ShadowMapShader* shdMapShader = (ShadowMapShader*)(game->shaders.find("depth")->second);
	DirLight* dLight = (DirLight*)lights[lights.size() - 1];
	glm::mat4 lightSpaceMat = (dLight)->GetProjectionMatrix() * 
		(dLight)->GetViewMatrix(camera->GetPosition() - dLight->GetDirection() * 25.0f);
	shdMapShader->setLightSpaceAndModelMatrix(&lightSpaceMat, NULL);
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Draw(shdMapShader);
	}
	//game->depthBuffer->Render();
	game->depthBuffer->Unbind();

	//	Передача в шейдер карт теней и матриц пространства источников света
	MaterialShader* matShader = (MaterialShader*)(game->shaders.find("standart")->second);
	for (auto it = activeLights.begin(); it != activeLights.end(); it++)
	{
		std::vector<glm::mat4> lSpaceMats;
		lSpaceMats.push_back(lightSpaceMat);
		std::vector<unsigned int> shadowMapsID;
		shadowMapsID.push_back(game->depthBuffer->GetTexture().GetId());
		matShader->addLightInfo(LightInfo(lSpaceMats, shadowMapsID, SourceType::DIRECTIONAL));
	}
	//	~TEST

	//	Привязка экранного кадрового буфера
	game->screenBuffer->Bind();
	game->screenBuffer->PrepareForRender();
	//	Отрисовка скайбокса
	RenderSkybox();
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Draw();
	}
	//	Вывод экранного буфера на экран
	game->screenBuffer->Render();
	
}

void Map::QuickCameraSetUp(Camera* camera)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->GetModel()->SetCamera(camera);
	}
}

void Map::Update(float dTime)
{
	//	Очистка информации шейдеров
	for (auto it = game->shaders.begin(); it != game->shaders.end(); it++)
	{
		it->second->clear();
	}
	//	Выполнение ботами действий и обновление всех объектов
	ActBots(dTime);
	UpdateObjects(dTime);

	//	Обновление камеры ,исходя из скорости автомобиля игрока
	Camera* camera = player->GetModel()->GetCamera();
	camera->SetFov(45 + glm::length(player->GetSpeed()));
	camera->UpdateCameraVectors();
	ScreenShader* screenShader = (ScreenShader*)(game->shaders.find("screen")->second);
	screenShader->setPlayerSpeed(player->GetSpeed());
	const glm::vec3* playerPos = player->GetPosition();

	//	Обновление статических объектов
	for (int i = 0; i < roadObjects.size(); i++)
	{
		const glm::vec3* objectPos = roadObjects[i]->GetPosition();
		float dist = playerPos->x - objectPos->x;
		if (abs(dist) > 7.0f * 30 / 2.0f)
		{
			roadObjects[i]->SetPosition(*objectPos +
				glm::vec3(glm::sign(dist) * 30 * 7.0f, 0.0f, 0.0f));
		}
	}

	//	Обработка ботов
	for (int i = 0; i < bots.size(); i++)
	{
		CarBot* bot = ((CarBot*)bots[i]);
		const glm::vec3* botPos = bot->GetPosition();
		float dist = playerPos->x - botPos->x;
		if (abs(dist) > 7.0f * 30 / 2.0f)
		{
			bot->SetPosition(*botPos + glm::vec3(glm::sign(dist) * 30 * 7.0f, 0.0f, 0.0f));
		}
	}

	//	Обновление источников света
	glm::vec3 lightPos = glm::vec3(0.0f);
	glm::vec3 camDir = glm::normalize(player->GetModel()->GetCamera()->GetFront());
	glm::vec3 camPos = player->GetModel()->GetCamera()->GetPosition();
	activeLights.clear();
	std::list<std::pair<int, float>> lightsDistances;
	for (int i = 0; i < lights.size(); i++)
	{
		switch (lights[i]->GetType())
		{
		case SourceType::POINT:
		{
			lightPos = ((PointLight*)lights[i])->GetPosition();
		}
		break;
		case SourceType::SPOTLIGHT:
		{
			lightPos = ((SpotLight*)lights[i])->GetPosition();
		}
		break;
		case SourceType::DIRECTIONAL:
		{
			lightsDistances.push_back(std::make_pair(i, 0.0f));
			continue;
		}
		default:
			continue;
		}
		glm::vec3 lightDir = glm::normalize(lightPos - camPos);
		glm::vec3 crossRes = glm::cross(lightDir, camDir);
		if (crossRes != glm::vec3(0.0f))
		{
			float angle = glm::acos(glm::dot(lightDir, camDir)) * 180.0f / glm::pi<float>();
			float lightDist = glm::distance(lightPos, camPos);
			if (lightDist <= 55.0f && angle < 110.0f || lightDist < 5.0f)
			{
				lightsDistances.push_back(std::make_pair(i, lightDist));
			}
		}
	}

	auto cmp = [](const std::pair<float, int>& a,
		const std::pair<float, int>& b)
	{
		return a.second < b.second;
	};

	lightsDistances.sort(cmp);

	for (auto it = lightsDistances.begin(); it != lightsDistances.end(); it++)
	{
		activeLights.push_back(lights[it->first]);
	}
	lightsUbo.LoadInfo(activeLights);
}

void Map::Clear()
{
	//	Objects clearing
	for (int i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
	objects.clear();
	//	Models clearing
	for (auto it = models.begin(); it != models.end(); it++)
	{
		delete it->second;
	}
	lights.clear();
	//	Lights clearing
	for (int i = 0; i < lights.size(); i++)
	{
		delete lights[i];
	}
	lights.clear();
}