#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int width, int height, const Shader* shader)
{
	glGenFramebuffers(1, &id);
	this->width = width;
	this->height = height;
	this->shader = shader;
	boundTexture = NULL;
	InitScreenField();
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &id);
	glDeleteVertexArrays(1, &sfVAO);
	glDeleteBuffers(1, &sfVBO);
}

void FrameBuffer::InitScreenField()
{
	float vertices[24] = {
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};
	glGenVertexArrays(1, &sfVAO);
	glGenBuffers(1, &sfVBO);
	glBindVertexArray(sfVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sfVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

unsigned int FrameBuffer::ID() const
{
	return id;
}

Texture FrameBuffer::GetTexture() const
{
	return texture;
}

const Texture* FrameBuffer::GetBoundTexture()
{
	return boundTexture;
}

void FrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FrameBuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::BindTexture(const Texture* texture)
{
}

void FrameBuffer::UnbindTexture()
{
}

void FrameBuffer::SetShader(const Shader* shader)
{
	this->shader = shader;
}

void FrameBuffer::Render() const
{
	if (shader == NULL)
	{
		std::cout << "ERROR:: FrameBuffer :: Couldn't render due shader is not set" << std::endl;
		return;
	}
	Unbind();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	shader->use();
	if (shader->GetType() == ShaderType::SCREEN)
	{
		((ScreenShader*)shader)->loadMainInfo();
	}
	glBindVertexArray(sfVAO);
	if (boundTexture != NULL)
		glBindTexture(GL_TEXTURE_2D, boundTexture->GetId());
	else glBindTexture(GL_TEXTURE_2D, texture.GetId());
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

ScreenFrameBuffer::ScreenFrameBuffer(int width, int height, const Shader* shader) : FrameBuffer(width, height, shader)
{
	SetupBuffer();
}

ScreenFrameBuffer::~ScreenFrameBuffer()
{
}

void ScreenFrameBuffer::SetupBuffer()
{
	Bind();
	texture = Texture::CreateEmptyTexture(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.GetId(), 0);

	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;
	}
	Unbind();
}

void ScreenFrameBuffer::PrepareForRender()
{
	glViewport(0, 0, width, height);
	Bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

DepthFrameBuffer::DepthFrameBuffer(int width, int height, const Shader* shader) : FrameBuffer(width, height, shader)
{
	SetupBuffer();
}

DepthFrameBuffer::~DepthFrameBuffer()
{
}

void DepthFrameBuffer::SetupBuffer()
{
	Bind();
	texture = Texture::CreateEmptyTexture(width, height, TextureDataType::DEPTH, TextureType::TEXTURE2D);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.GetId(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;
	}
	Unbind();
}

void DepthFrameBuffer::BindTexture(const Texture* texture)
{
	Bind();
	boundTexture = texture;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, boundTexture->GetId(), 0);
	Unbind();
}

void DepthFrameBuffer::UnbindTexture()
{
	Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.GetId(), 0);
	boundTexture = NULL;
	Unbind();
}

void DepthFrameBuffer::PrepareForRender()
{
	if (boundTexture != NULL)
		glViewport(0, 0, boundTexture->GetWidth(), boundTexture->GetHeight());
	else glViewport(0, 0, width, height);
	Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}