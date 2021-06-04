#version 450 core

#define NR_DIR_LIGHTS 1
#define NR_POINT_LIGHTS 6
#define NR_SPOT_LIGHTS 6

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
	float alpha;
	int diffTextCount;
	int specTextCount;
	int ambTextCount;
};

struct ActiveMaterial
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
	float alpha;
};

struct DirLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};

struct PointLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
};

layout (shared, binding = 0) uniform Lights
{
	int dirLigtsCnt;
	int pntLigtsCnt;
	int sptLigtsCnt;
	DirLight dirLights[NR_DIR_LIGHTS];
	PointLight pointLights[NR_POINT_LIGHTS];
	SpotLight spotLights[NR_SPOT_LIGHTS];
};

vec4 CalcDirLight(DirLight light, inout ActiveMaterial material, vec3 normal, vec3 fragPos, vec3 viewDir);
vec4 CalcPointLight(PointLight light, inout ActiveMaterial material, vec3 normal, vec3 fragPos, vec3 viewDir);
vec4 CalcSpotLight(SpotLight light, inout ActiveMaterial material, vec3 normal, vec3 fragPos, vec3 viewDir);

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TextureCoords;
uniform vec3 viewPos;
uniform Material material;
uniform samplerCube skybox;
uniform bool hasSkybox;

void main()
{
	//	MATERIAL PREPARING
	ActiveMaterial activeMat = ActiveMaterial(vec4(0.0), vec4(0.0), vec4(0.0), 0.0f, 0.0f);

	if (material.ambTextCount <= 0)
	{
		activeMat.ambient = material.ambient;
	}
	else 
	{
		activeMat.ambient = texture2D(material.texture_diffuse1, TextureCoords);
	}
	if (material.diffTextCount <= 0)
	{
		activeMat.diffuse = material.diffuse;
		activeMat.diffuse.a = material.alpha;
	}
	else 
	{
		activeMat.diffuse = texture2D(material.texture_diffuse1, TextureCoords);
	}
	if (material.specTextCount <= 0)
	{
		activeMat.specular = material.specular;
	}
	else
	{
		activeMat.specular = texture2D(material.texture_specular1, TextureCoords);
	}
	
	activeMat.shininess = material.shininess;
	activeMat.alpha = material.alpha;
	activeMat.ambient = activeMat.diffuse;
	activeMat.ambient.a = 0.0f;
	activeMat.specular.a = 0.0f;

	//	COLOR CALCULATING

	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	//	Light Calculating
	
	//	Directional lights
	int dLightsC = min(dirLigtsCnt, NR_DIR_LIGHTS);
	for (int i = 0; i < dLightsC; i++)
	{
		FragColor += CalcDirLight(dirLights[i], activeMat, normal, FragPos, viewDir);
	}

	//	Point lights
	int pLightsC = min(pntLigtsCnt, NR_POINT_LIGHTS);
	for (int i = 0; i < pLightsC; i++)
	{
		FragColor += CalcPointLight(pointLights[i], activeMat, normal, FragPos, viewDir);
	}

	//	Spot lights
	int sLightsC = min(sptLigtsCnt, NR_SPOT_LIGHTS);
	for (int i = 0; i < 4; i++)
	{
		FragColor += CalcSpotLight(spotLights[i], activeMat, normal, FragPos, viewDir);	
	}
	FragColor.a = activeMat.diffuse.a;
	//	Final Mix

	//	Reflections Calculating

	if (hasSkybox)
	{
		vec3 R = reflect(-viewDir, normal);
		vec4 reflectColor = texture(skybox, R) * activeMat.specular;	
		FragColor.rgb = mix(FragColor.rgb, reflectColor.rgb, 0.3);
	}
	
}

vec4 CalcDirLight(DirLight light, inout ActiveMaterial material, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	vec3 reflDir = reflect(-lightDir, normal);
	//	Ambient
	vec4 ambient = vec4(light.ambient, 1.0f) * material.ambient;
	//	diffuse
	vec4 diffuse = vec4(max(dot(normal, lightDir), 0.0) * light.diffuse, 1.0f) * material.diffuse;
	//	specular
	vec4 specular = vec4(pow(max(dot(viewDir, reflDir), 0.0), material.shininess) * light.specular, 1.0f) * material.specular;
	return diffuse + specular + ambient;
}

vec4 CalcPointLight(PointLight light, inout ActiveMaterial material, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 reflDir = reflect(-lightDir, normal);
	float lightFragDistance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * lightFragDistance +
	light.quadratic * lightFragDistance * lightFragDistance);
	//	Ambient
	vec4 ambient =  vec4(light.ambient, 1.0f) * material.ambient;
	//	diffuse
	vec4 diffuse = vec4(max(dot(normal, lightDir), 0.0) * light.diffuse, 1.0f) * material.diffuse;
	//	specular
	vec4 specular = vec4(pow(max(dot(viewDir, reflDir), 0.0), material.shininess) * light.specular, 1.0f) * material.specular; 

	return (diffuse + specular + ambient) * attenuation;
}


vec4 CalcSpotLight(SpotLight light, inout ActiveMaterial material, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - FragPos);
	vec3 reflDir = reflect(-lightDir, normal);
	float lightFragDistance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * lightFragDistance +
	light.quadratic * lightFragDistance * lightFragDistance);
	//	Ambient
	vec4 ambient = vec4(light.ambient, 1.0f) * material.ambient;
	//	diffuse
	vec4 diffuse = vec4(max(dot(normal, lightDir), 0.0) * light.diffuse, 1.0f) * material.diffuse;
	//	specular
	vec4 specular = vec4(pow(max(dot(viewDir, reflDir), 0.0), material.shininess) * light.specular, 1.0f) * material.specular;

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0f, 1.0f);

	return ((diffuse + specular) * intensity + ambient) * attenuation;
}