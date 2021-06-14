#version 450 core
layout(location = 0) in vec3 pos;
uniform mat4 finalMatrix;

void main()
{
	gl_Position = finalMatrix * vec4(pos , 1.0f);
}