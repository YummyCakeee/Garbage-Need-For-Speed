#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTextureCoords;

uniform mat4 model;
out vec2 TextureCoords;

void main()
{
	gl_Position = model * vec4(aPos, 1.0f);
	TextureCoords = aTextureCoords;
}