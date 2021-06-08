#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <time.h>
#include "Shader.h"
#include "Model.h"
#include "Car.h"
#include "Camera.h"
#include "GameGlobal.h"
#include "GameGlobalStructs.h"
#include "Bot.h"

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
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gameGlob.gameProps.GetWindowWidth(), gameGlob.gameProps.GetWindowHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, gameGlob.gameProps.GetWindowWidth(), gameGlob.gameProps.GetWindowHeight());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	   // positions   // texCoords
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	//	OWN FRAMEBUFFER~

	Map map;
	map.Initialize();
	gameGlob.SetMap(&map);
	double oldTime = glfwGetTime(), newTime;
	//	Игровой цикл
	while (!glfwWindowShouldClose(win))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(0.0f, 0.82f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		newTime = glfwGetTime();
		gameGlob.SetDeltaTime(newTime - oldTime);
		oldTime = newTime;
		glfwPollEvents();
		gameGlob.ProcessInput();
		gameGlob.GetMap()->UpdateObjects(gameGlob.GetDeltaTime());
		gameGlob.GetMap()->ActBots(gameGlob.GetDeltaTime());
		gameGlob.GetMap()->Update();
		gameGlob.GetMap()->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		screenShader->use();
		screenShader->setBool("gammaCor", gameGlob.gameProps.IsGammaCorrectionEnabled());
		screenShader->setVec("playerSpeed", (glm::vec3)gameGlob.GetMap()->GetPlayer()->GetSpeed());
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(win);
	}
	gameGlob.GetMap()->Clear();
	delete screenShader;

	glfwTerminate();
	return 0;
}