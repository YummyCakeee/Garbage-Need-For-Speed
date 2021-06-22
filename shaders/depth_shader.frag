#version 450 core

struct Material
{
	sampler2D texture_diffuse;
	vec4 diffuse;
	bool useDiffMap;
};

in VS_OUT
{
	vec4 FragPos;
	vec2 TextureCoords;
}vs_in;
uniform bool linearize;
uniform float farPlane;
uniform vec3 lightPos;
uniform Material material;

void main()
{
	float diffuseAlpha = 0.0f;
	if (material.useDiffMap)
		diffuseAlpha = texture2D(material.texture_diffuse, vs_in.TextureCoords).a;
	else diffuseAlpha = material.diffuse.a;
	if (diffuseAlpha <= 0.2f)
	{
		gl_FragDepth = 1.0f;
		return;
	}
	if (linearize)
	{
		float lightDist = length(vs_in.FragPos.xyz - lightPos) / farPlane;
		gl_FragDepth = lightDist;
	} 
	else gl_FragDepth = gl_FragCoord.z;
}