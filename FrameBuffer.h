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
	const Texture* boundTexture;
	const Shader* shader;
	unsigned int sfVAO, sfVBO;
	FrameBuffer(int width, int height, const Shader* shader = NULL);
	virtual void SetupBuffer() = 0;
	void InitScreenField();
public:
	FrameBuffer() = delete;
	~FrameBuffer();
	unsigned int ID() const;
	Texture GetTexture() const;
	const Texture* GetBoundTexture();
	void Bind() const;
	void Unbind() const;
	virtual void BindTexture(const Texture& texture);
	virtual void UnbindTexture();
	void SetShader(const Shader* shader);
	virtual void PrepareForRender() = 0;
	void Render() const;
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

class DepthFrameBuffer : public FrameBuffer
{
private:
	void SetupBuffer() override;
public:
	DepthFrameBuffer(int width, int height, const Shader* shader = NULL);
	~DepthFrameBuffer();
	virtual void BindTexture(const Texture& texture) override;
	virtual void UnbindTexture() override;
	virtual void PrepareForRender() override;
};
