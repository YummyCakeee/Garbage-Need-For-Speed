#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;

out VS_OUT
{
	vec3 Normal;
	vec3 FragPos;
	vec4 FragPosLightSpace;
	vec2 TextureCoords;
}vs_out;

uniform mat4 model;
uniform mat4 normalMatrix;	//	Inversed and Transpossed Model Matrix
uniform mat4 lightSpaceMatrix;	//	Proj * View
uniform mat4 finalMatrix; //	Proj * View * model

void main()
{
	gl_Position = finalMatrix * vec4(aPos , 1.0f);
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0f);
	vs_out.Normal = mat3(normalMatrix) * normal;
	vs_out.TextureCoords = textureCoords;
}