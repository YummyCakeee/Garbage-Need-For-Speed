#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;
out vec3 Normal;
out vec3 FragPos;
out vec2 TextureCoords;
uniform mat4 model;
uniform mat4 normalMatrix;
uniform mat4 finalMatrix;

void main()
{
	gl_Position = finalMatrix * vec4(aPos , 1.0f);
	FragPos = vec3(model * vec4(aPos, 1.0f));
	Normal = mat3(normalMatrix) * normal;
	TextureCoords = textureCoords;
}