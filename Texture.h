#pragma once
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <iostream>
#include <string>
#include <vector>

enum class TextureType
{
	Diffuse, Specular, Normal, Height, CubeMap, COLOR, DEPTH, STENCIL, UNDEFINED
};

class Texture
{
private:
	unsigned int id;
	TextureType type;
	std::string path;
	int channelsCount;
	float width;
	float height;
public:
	Texture();
	Texture(unsigned int id, TextureType type, const std::string& path, float width = 0.0f, float height = 0.0f, int channelsCount = 3);
	unsigned int GetId();
	TextureType GetType();
	std::string GetPath();
	int GetChannelsCount();
	float GetWidth();
	float GetHeight();
	void SetType(TextureType type);
	void SetPath(const std::string& path);
	void SetSize(float width, float height);
	void SetWidth(float width);
	void SetHeight(float height);
	static Texture LoadTexture(const std::string& path, bool useSRGB = false);
	static Texture CreateEmptyTexture(int width, int height, TextureType type = TextureType::COLOR);
	static unsigned int LoadCubeMap(const std::vector<std::string>& pathes);
};