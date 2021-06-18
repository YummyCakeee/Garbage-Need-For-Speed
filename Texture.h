#pragma once
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <iostream>
#include <string>
#include <vector>

enum class TextureDataType
{
	DIFFUSE, SPECULAR, NORMAL, HEIGHT, COLOR, DEPTH, STENCIL, UNDEFINED
};

enum class TextureType
{
	TEXTURE2D, CUBEMAP
};

class Texture
{
private:
	unsigned int id;
	TextureType type;
	TextureDataType dataType;
	std::string path;
	int channelsCount;
	int width;
	int height;
public:
	Texture();
	Texture(unsigned int id, TextureDataType dataType, TextureType type, const std::string& path, int width = 0.0f, int height = 0.0f, int channelsCount = 3);
	unsigned int GetId() const;
	TextureDataType GetDataType() const;
	TextureType GetType() const;
	std::string GetPath() const;
	int GetChannelsCount() const;
	int GetWidth() const;
	int GetHeight() const;
	void SetDataType(TextureDataType dataType);
	void SetType(TextureType type);
	void SetPath(const std::string& path);
	void SetSize(int width, int height);
	void SetWidth(int width);
	void SetHeight(int height);
	void Delete();
	static Texture LoadTexture(const std::string& path, bool useSRGB = false);
	static Texture CreateEmptyTexture(int width, int height, TextureDataType dataType = TextureDataType::COLOR, TextureType type = TextureType::TEXTURE2D);
	static unsigned int LoadCubeMap(const std::vector<std::string>& pathes);
};