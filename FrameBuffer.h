#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Texture.h"
#include "Shader.h"

class FrameBuffer
{
protected:
	unsigned int id;
	int width;
	int height;
	Texture texture;
	const Shader* shader;
	unsigned int sfVAO, sfVBO;
	FrameBuffer(int width, int height, const Shader* shader = NULL);
	virtual void SetupBuffer() = 0;
	void InitScreenField();
public:
	FrameBuffer() = delete;
	~FrameBuffer();
	unsigned int ID();
	void Bind();
	void Unbind();
	void SetShader(const Shader* shader);
	virtual void PrepareForRender() = 0;
	void Render();
};

class ScreenFrameBuffer : public FrameBuffer
{
private:
	unsigned int renderBuffer;
	void SetupBuffer() override;
public:
	ScreenFrameBuffer(int width, int height, const Shader* shader = NULL);
	~ScreenFrameBuffer();
	void PrepareForRender() override;


};
