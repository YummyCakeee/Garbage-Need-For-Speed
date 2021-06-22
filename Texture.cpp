#include "Texture.h"

Texture::Texture()
{
	id = 0;
	type = TextureType::TEXTURE2D;
	dataType = TextureDataType::UNDEFINED;
	path = "";
	channelsCount = 3;
	width = 0.0f;
	height = 0.0f;
}

Texture::Texture(unsigned int id, TextureDataType dataType, TextureType type, const std::string& path, int width, int height, int channelsCount)
{
	this->id = id;
	this->dataType = dataType;
	this->type = type;
	this->path = path;
	this->channelsCount = channelsCount;
	this->width = width;
	this->height = height;
}

unsigned int Texture::GetId() const
{
	return id;
}

TextureDataType Texture::GetDataType() const
{
	return dataType;
}

TextureType Texture::GetType() const
{
	return type;
}

std::string Texture::GetPath() const
{
	return path;
}

int Texture::GetChannelsCount() const
{
	return channelsCount;
}

int Texture::GetWidth() const
{
	return width;
}

int Texture::GetHeight() const
{
	return height;
}

void Texture::SetDataType(TextureDataType dataType)
{
	this->dataType = dataType;
}

void Texture::SetType(TextureType type)
{
	this->type = type;
}

void Texture::SetPath(const std::string& path)
{
	this->path = path;
}

void Texture::SetSize(int width, int height)
{
	SetWidth(width);
	SetHeight(height);
}

void Texture::SetWidth(int width)
{
	if (width < 0.0f) this->width = 0.0f;
	else this->width = width;
}

void Texture::SetHeight(int height)
{
	if (height < 0.0f) this->height = 0.0f;
	else this->height = height;
}

void Texture::Delete()
{
	glDeleteTextures(1, &id);
}

Texture Texture::LoadTexture(const std::string& path, bool useSRGB)
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
		return Texture(0, TextureDataType::UNDEFINED, TextureType::TEXTURE2D, path);
	}
	switch (channels)
	{
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R, textureWdth, textureHght, 0, GL_R, GL_UNSIGNED_BYTE, image);
		break;
	case 3:
		if (useSRGB)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, textureWdth, textureHght, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWdth, textureHght, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		break;
	case 4:
		if (useSRGB)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, textureWdth, textureHght, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
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
	return Texture(texture, TextureDataType::UNDEFINED, TextureType::TEXTURE2D, path, textureWdth, textureHght, channels);
}

Texture Texture::CreateEmptyTexture(int width, int height, TextureDataType dataType, TextureType type)
{
	switch (type)
	{
	case TextureType::TEXTURE2D:
		return CreateEmpty2DTexture(width, height, dataType);
	case TextureType::CUBEMAP:
		return CreateEmptyCubeMapTexture(width, height, dataType);
	}
}

Texture Texture::CreateEmpty2DTexture(int width, int height, TextureDataType dataType)
{
	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	switch (dataType)
	{
	case TextureDataType::DEPTH:
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}; break;
	case TextureDataType::STENCIL:
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX,
			width, height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}; break;
	default:
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
			width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}; break;
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	return Texture(texture, dataType, TextureType::TEXTURE2D, "", width, height);
}

Texture Texture::CreateEmptyCubeMapTexture(int width, int height, TextureDataType dataType)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	switch (dataType)
	{
	case TextureDataType::DEPTH:
	{
		for (int i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	};	break;
	default:
		break;
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return Texture(texture, dataType, TextureType::CUBEMAP, "", width, height);
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
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, textureWdth, textureHght, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, textureWdth, textureHght, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture;
}