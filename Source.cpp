#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <time.h>
#include "Shader.h"
#include "GameGlobal.h"
#include "GameGlobalStructs.h"
#include "FrameBuffer.h"


using namespace std;
GameGlobal gameGlob = GameGlobal(0.0, 1280, 720);

void OnResize(GLFWwindow* win, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Mouse_callback(GLFWwindow* window, double x, double y)
{
	gameGlob.mouse.UpdatePosition(glm::dvec2(x, y));
}

void Scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	gameGlob.mouse.SetScrollOffset(glm::dvec2(xOffset, yOffset));
}

void Key_callback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_UP)
		{
			gameGlob.SetKeyState(KeysEnum::UP, KeyState::PRESS);
		}
		if (key == GLFW_KEY_DOWN)
		{
			gameGlob.SetKeyState(KeysEnum::DOWN, KeyState::PRESS);
		}
		if (key == GLFW_KEY_LEFT)
		{
			gameGlob.SetKeyState(KeysEnum::LEFT, KeyState::PRESS);
		}
		if (key == GLFW_KEY_RIGHT)
		{
			gameGlob.SetKeyState(KeysEnum::RIGHT, KeyState::PRESS);
		}
		if (key == GLFW_KEY_W)
		{
			gameGlob.SetKeyState(KeysEnum::W, KeyState::PRESS);
		}
		if (key == GLFW_KEY_S)
		{
			gameGlob.SetKeyState(KeysEnum::S, KeyState::PRESS);
		}
		if (key == GLFW_KEY_A)
		{
			gameGlob.SetKeyState(KeysEnum::A, KeyState::PRESS);
		}
		if (key == GLFW_KEY_D)
		{
			gameGlob.SetKeyState(KeysEnum::D, KeyState::PRESS);
		}
		if (key == GLFW_KEY_F)
		{
			gameGlob.SetKeyState(KeysEnum::F, KeyState::PRESS);
		}
		if (key == GLFW_KEY_V)
		{
			gameGlob.SetKeyState(KeysEnum::V, KeyState::PRESS);
		}
		if (key == GLFW_KEY_J)
		{
			gameGlob.SetKeyState(KeysEnum::J, KeyState::PRESS);
		}
		if (key == GLFW_KEY_K)
		{
			gameGlob.SetKeyState(KeysEnum::K, KeyState::PRESS);
		}
		if (key == GLFW_KEY_L)
		{
			gameGlob.SetKeyState(KeysEnum::L, KeyState::PRESS);
		}
		if (key == GLFW_KEY_SPACE)
		{
			gameGlob.SetKeyState(KeysEnum::SPACE, KeyState::PRESS);
		}
		if (key == GLFW_KEY_ENTER)
		{
			gameGlob.SetKeyState(KeysEnum::ENTER, KeyState::PRESS);
		}
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(win, true);
		}
	}
	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_UP)
		{
			gameGlob.SetKeyState(KeysEnum::UP, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_DOWN)
		{
			gameGlob.SetKeyState(KeysEnum::DOWN, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_LEFT)
		{
			gameGlob.SetKeyState(KeysEnum::LEFT, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_RIGHT)
		{
			gameGlob.SetKeyState(KeysEnum::RIGHT, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_W)
		{
			gameGlob.SetKeyState(KeysEnum::W, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_S)
		{
			gameGlob.SetKeyState(KeysEnum::S, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_A)
		{
			gameGlob.SetKeyState(KeysEnum::A, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_D)
		{
			gameGlob.SetKeyState(KeysEnum::D, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_F)
		{
			gameGlob.SetKeyState(KeysEnum::F, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_V)
		{
			gameGlob.SetKeyState(KeysEnum::V, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_J)
		{
			gameGlob.SetKeyState(KeysEnum::J, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_K)
		{
			gameGlob.SetKeyState(KeysEnum::K, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_L)
		{
			gameGlob.SetKeyState(KeysEnum::L, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_SPACE)
		{
			gameGlob.SetKeyState(KeysEnum::SPACE, KeyState::RELEASE);
		}
		if (key == GLFW_KEY_ENTER)
		{
			gameGlob.SetKeyState(KeysEnum::ENTER, KeyState::RELEASE);
		}
	}

}

int main()
{
#pragma region WINDOW INITIALIZATION
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.5);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(gameGlob.gameProps.GetWindowWidth(), gameGlob.gameProps.GetWindowHeight(),
		"Garbage Need For Speed", NULL, NULL);
	if (win == NULL)
	{
		cout << "Не удалось создать окно\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);
	if (glewInit() != GLEW_OK)
	{
		cout << "Не удалось инициализировать glew\n";
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(win, OnResize);
	glfwSetCursorPosCallback(win, Mouse_callback);
	glfwSetScrollCallback(win, Scroll_callback);
	glfwSetKeyCallback(win, Key_callback);
	glfwSetCursorPos(win, (double)gameGlob.gameProps.GetWindowWidth() / 2.0f, (double)gameGlob.gameProps.GetWindowHeight() / 2.0f);
	glewExperimental = GL_TRUE;
	glViewport(0, 0, gameGlob.gameProps.GetWindowWidth(), gameGlob.gameProps.GetWindowHeight());
#pragma endregion

	srand(time(NULL));

	Shader* screenShader = new Shader("shaders\\screen_shader.vert", "shaders\\screen_shader.frag");
	//	OWN FRAMEBUFFER
	ScreenFrameBuffer sfb = ScreenFrameBuffer(gameGlob.gameProps.GetWindowWidth(), gameGlob.gameProps.GetWindowHeight(), screenShader);
	//	OWN FRAMEBUFFER~

	Map map;
	map.Initialize();
	gameGlob.SetMap(&map);
	double oldTime = glfwGetTime(), newTime;
	//	Игровой цикл
	while (!glfwWindowShouldClose(win))
	{
		newTime = glfwGetTime();
		gameGlob.SetDeltaTime(newTime - oldTime);
		oldTime = newTime;
		glfwPollEvents();
		gameGlob.ProcessInput();
		gameGlob.GetMap()->Update(gameGlob.GetDeltaTime());
		sfb.Bind();
		sfb.PrepareForRender();
		gameGlob.GetMap()->Render();
		sfb.Render();
		glfwSwapBuffers(win);
	}
	gameGlob.GetMap()->Clear();
	delete screenShader;

	glfwTerminate();
	return 0;
}