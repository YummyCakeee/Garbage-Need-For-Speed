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

glm::ivec2 GameGlobal::GameProperties::GetWindowSize() const
{
	return glm::ivec2(windowWidth, windowHeight);
}

int GameGlobal::GameProperties::GetWindowWidth() const
{
	return windowWidth;
}

int GameGlobal::GameProperties::GetWindowHeight() const
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

bool GameGlobal::GameProperties::IsGammaCorrectionEnabled() const
{
	return gammaCorrection;
}

GameGlobal::GameGlobal() : gameProps(100, 100)
{
	Initialize();
	SetDeltaTime(0.0);
	mouse = Mouse(glm::dvec2(100.0));
	timeCoef = 1.0;
}

GameGlobal::GameGlobal(double dTime, glm::ivec2 windowSize) : gameProps(windowSize)
{
	Initialize();
	SetDeltaTime(dTime);
	mouse = Mouse(glm::dvec2(windowSize) / 2.0);
	timeCoef = 1.0;
}
GameGlobal::GameGlobal(double dTime, int windowWidth, int windowHeight) : gameProps(windowWidth, windowHeight)
{
	Initialize();
	SetDeltaTime(dTime);
	mouse = Mouse(glm::dvec2(windowWidth, windowHeight) / 2.0);
	timeCoef = 1.0;
}

GameGlobal::~GameGlobal()
{
	//	Shaders clearing
	for (auto it = shaders.begin(); it != shaders.end(); it++)
	{
		delete it->second;
	}
	shaders.clear();
}

void GameGlobal::Initialize()
{
	InitOpenGL();
	srand(time(NULL));
	map = NULL;
	Shader* standartShader = new Shader("shaders\\standart_shader.vert", "shaders\\standart_shader.frag");
	shaders.insert(std::make_pair("standart", standartShader));
	Shader* skyboxShader = new Shader("shaders\\skybox_shader.vert", "shaders\\skybox_shader.frag");
	shaders.insert(std::make_pair("skybox", skyboxShader));
	Shader* raindropShader = new Shader("shaders\\raindrop_shader.vert", "shaders\\raindrop_shader.frag");
	shaders.insert(std::make_pair("raindrop", raindropShader));
	Shader* screenShader = new Shader("shaders\\screen_shader.vert", "shaders\\screen_shader.frag");
	shaders.insert(std::make_pair("screen", screenShader));
	screenBuffer = new ScreenFrameBuffer(gameProps.GetWindowWidth(), gameProps.GetWindowHeight(), screenShader);
	InitKeys();
	map = new Map(*this);
	map->Initialize();
}

bool GameGlobal::InitOpenGL()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.5);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(gameProps.GetWindowWidth(), gameProps.GetWindowHeight(),
		"Garbage Need For Speed", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Не удалось создать окно\n";
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Не удалось инициализировать glew\n";
		glfwTerminate();
		return false;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, (double)gameProps.GetWindowWidth() / 2.0f, (double)gameProps.GetWindowHeight() / 2.0f);
	glewExperimental = GL_TRUE;
	glViewport(0, 0, gameProps.GetWindowWidth(), gameProps.GetWindowHeight());
}

double GameGlobal::GetDeltaTime() const
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

GLFWwindow* GameGlobal::GetWindow()
{
	return window;
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
