#version 330 core

layout(location = 0) in vec3 aPos;
out vec3 TexCoords;

uniform mat4 finalMatrix;

void main()
{
	gl_Position = (finalMatrix * vec4(aPos , 1.0f)).xyww;
	TexCoords = aPos;
}