#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Texture.h"
#include "Shader.h"

class FrameBuffer
{
protected:
	unsigned int id;
	float width;
	float height;
	Texture texture;
	const Shader* shader;
	unsigned int sfVAO, sfVBO;
	FrameBuffer(float width, float height, const Shader* shader = NULL);
	virtual void SetupBuffer() = 0;
	void InitScreenField();
public:
	~FrameBuffer();
	unsigned int ID();
	void Bind();
	void Unbind();
	virtual void PrepareForRender() = 0;
	void Render();
};

class ScreenFrameBuffer : public FrameBuffer
{
private:
	unsigned int renderBuffer;
	void SetupBuffer() override;
public:
	ScreenFrameBuffer(float width, float height, const Shader* shader = NULL);
	~ScreenFrameBuffer();
	void PrepareForRender() override;


};
