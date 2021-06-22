#version 450 core
layout(location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTextureCoords;

out VS_OUT
{
	vec4 FragPos;
	vec2 TextureCoords;
}vs_out;

uniform mat4 finalMatrix;
uniform mat4 model;

void main()
{
	vs_out.FragPos = model * vec4(aPos, 1.0f);
	vs_out.TextureCoords = aTextureCoords;
	gl_Position = finalMatrix * vec4(aPos , 1.0f);
}