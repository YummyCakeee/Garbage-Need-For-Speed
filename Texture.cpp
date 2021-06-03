#include "Texture.h"

Texture::Texture()
{
	id = 0;
	type = TextureType::UNDEFINED;
	path = "";
	channelsCount = 3;
}

Texture::Texture(unsigned int id, TextureType type, const std::string& path, int channelsCount)
{
	this->id = id;
	this->type = type;
	this->path = path;
	this->channelsCount = channelsCount;
}

unsigned int Texture::GetId()
{
	return id;
}

TextureType Texture::GetType()
{
	return type;
}

std::string Texture::GetPath()
{
	return path;
}

int Texture::GetChannelsCount()
{
	return channelsCount;
}

void Texture::SetType(TextureType type)
{
	this->type = type;
}

void Texture::SetPath(const std::string& path)
{
	this->path = path;
}

Texture Texture::LoadTexture(const std::string& path)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	int textureWdth, textureHght;
	int channels;
	unsigned char* image = SOIL_load_image(path.c_str(), &textureWdth, &textureHght, &channels, SOIL_LOAD_AUTO);
	if (!image)
	{
		std::cout << "ERROR:: Couldn't load texture:: Path:" + path;
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &texture);
		return Texture(0, TextureType::UNDEFINED, path);
	}
	switch (channels)
	{
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R, textureWdth, textureHght, 0, GL_R, GL_UNSIGNED_BYTE, image);
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWdth, textureHght, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		break;
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWdth, textureHght, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		break;
	default:
		break;
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return Texture(texture, TextureType::UNDEFINED, path, channels);
}

unsigned int Texture::LoadCubeMap(const std::vector<std::string>& pathes)
{
	if (pathes.size() != 6)
	{
		std::cout << "ERROR:: Couldn't load cube map:: textures count is not equall to 6" << std::endl;
		return 0;
	}
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	for (int i = 0; i < pathes.size(); i++)
	{
		int textureWdth, textureHght;
		int channels;
		unsigned char* image = SOIL_load_image(pathes[i].c_str(), &textureWdth, &textureHght, &channels, SOIL_LOAD_AUTO);
		if (!image)
		{
			std::cout << "ERROR:: Couldn't load texture:: Path:" + pathes[i];
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glDeleteTextures(1, &texture);
			return 0;
		}
		if (channels == 4)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, textureWdth, textureHght, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, textureWdth, textureHght, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture;
}