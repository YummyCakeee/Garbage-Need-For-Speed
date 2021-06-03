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
	Diffuse, Specular, Normal, Height, CubeMap, UNDEFINED
};

class Texture
{
private:
	unsigned int id;
	TextureType type;
	std::string path;
	int channelsCount;
public:
	Texture();
	Texture(unsigned int id, TextureType type, const std::string& path, int channelsCount = 3);
	unsigned int GetId();
	TextureType GetType();
	std::string GetPath();
	int GetChannelsCount();
	void SetType(TextureType type);
	void SetPath(const std::string& path);
	static Texture LoadTexture(const std::string& path);
	static unsigned int LoadCubeMap(const std::vector<std::string>& pathes);
};