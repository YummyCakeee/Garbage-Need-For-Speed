#version 450 core

#define NR_DIR_LIGHTS 1
#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 8

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out VS_OUT
{
	vec3 Normal;
	vec3 FragPos;
	vec4 FragPosDLightSpaces[NR_DIR_LIGHTS];
	vec4 FragPosSLightSpaces[NR_SPOT_LIGHTS];
	vec2 TextureCoords;
}vs_out;

uniform mat4 model;
uniform mat4 normalMatrix;	//	Inversed and Transpossed Model Matrix
uniform mat4 dLightSpaceMatrix[NR_DIR_LIGHTS];	//	Proj * View
uniform mat4 sLightSpaceMatrix[NR_SPOT_LIGHTS];	//	Proj * View
uniform mat4 finalMatrix; //	Proj * View * model

void main()
{
	gl_Position = finalMatrix * vec4(aPos , 1.0f);
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0f));
	for (int i = 0; i < NR_DIR_LIGHTS; i++)
	{
		vs_out.FragPosDLightSpaces[i] = dLightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0f);
	}
	for (int i = 0; i < NR_SPOT_LIGHTS; i++)
	{
		vs_out.FragPosSLightSpaces[i] = sLightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0f);
	}
	vs_out.Normal = mat3(normalMatrix) * aNormal;
	vs_out.TextureCoords = aTextureCoords;

	vec3 T = normalize(vec3(normalMatrix * vec4(aTangent, 0.0f)));
	vec3 B = normalize(vec3(normalMatrix * vec4(aBitangent, 0.0f)));
	vec3 N = normalize(vec3(normalMatrix * vec4(aNormal, 0.0f)));

	mat3 TBN = mat3(T, B, N);
}