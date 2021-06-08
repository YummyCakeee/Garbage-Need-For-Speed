#version 450 core

#define NR_DIR_LIGHTS 1
#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 10

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
	float alpha;
	float reflectivity;
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

layout (shared, binding = 0) uniform DirLightsInfo
{
	int dirLigtsCnt;
	int pntLigtsCnt;
	int sptLigtsCnt;
	DirLight dirLights[NR_DIR_LIGHTS];
	PointLight pointLights[NR_POINT_LIGHTS];
	SpotLight spotLights[NR_SPOT_LIGHTS];
};

vec4 CalcDirLight(int lightIndex, vec3 normal, vec3 fragPos, vec3 viewDir);
vec4 CalcPointLight(int lightIndex, vec3 normal, vec3 fragPos, vec3 viewDir);
vec4 CalcSpotLight(int lightIndex, vec3 normal, vec3 fragPos, vec3 viewDir);

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TextureCoords;
uniform vec3 viewPos;
uniform Material material;
uniform samplerCube skybox;
uniform bool hasSkybox;

ActiveMaterial activeMat = ActiveMaterial(vec4(0.0), vec4(0.0), vec4(0.0), 0.0f, 0.0f);

void main()
{
	//	MATERIAL PREPARING
	if (dirLights[0].direction == vec3(0.0f, -0.3f, -0.9f))
	{
	FragColor = vec4(1.0f);
	return;
	}
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
		FragColor += CalcDirLight(i, normal, FragPos, viewDir);
	}

	//	Point lights
	int pLightsC = min(pntLigtsCnt, NR_POINT_LIGHTS);
	for (int i = 0; i < pLightsC; i++)
	{
		FragColor += CalcPointLight(i, normal, FragPos, viewDir);
	}

	//	Spot lights
	int sLightsC = min(sptLigtsCnt, NR_SPOT_LIGHTS);
	for (int i = 0; i < sLightsC; i++)
	{
		FragColor += CalcSpotLight(i, normal, FragPos, viewDir);	
	}
	FragColor.a = activeMat.diffuse.a;
	//	Final Mix

	//	Reflections Calculating

	if (hasSkybox)
	{
		vec3 R = reflect(-viewDir, normal);
		vec4 reflectColor = texture(skybox, R) * activeMat.specular;	
		FragColor.rgb = mix(FragColor.rgb, reflectColor.rgb, material.reflectivity);
	}
	
}

vec4 CalcDirLight(int lightIndex, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	int i = lightIndex;
	vec3 lightDir = normalize(-dirLights[i].direction);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	//	Ambient
	vec4 ambient = vec4(dirLights[i].ambient, 1.0f) * activeMat.ambient;
	//	diffuse
	vec4 diffuse = vec4(max(dot(normal, lightDir), 0.0) * dirLights[i].diffuse, 1.0f) * activeMat.diffuse;
	//	specular
	vec4 specular = vec4(pow(max(dot(normal, halfWayDir), 0.0), activeMat.shininess) * dirLights[i].specular, 1.0f) * activeMat.specular;
	return diffuse + specular + ambient;
}

vec4 CalcPointLight(int lightIndex, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	int i = lightIndex;
	vec3 lightDir = normalize(pointLights[i].position - fragPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float lightFragDistance = length(pointLights[i].position - fragPos);
	float attenuation = 1.0f / (pointLights[i].constant + pointLights[i].linear * lightFragDistance +
	pointLights[i].quadratic * lightFragDistance * lightFragDistance);
	//	Ambient
	vec4 ambient =  vec4(pointLights[i].ambient, 1.0f) * activeMat.ambient;
	//	diffuse
	vec4 diffuse = vec4(max(dot(normal, lightDir), 0.0) * pointLights[i].diffuse, 1.0f) * activeMat.diffuse;
	//	specular
	vec4 specular = vec4(pow(max(dot(normal, halfWayDir), 0.0), activeMat.shininess) * pointLights[i].specular, 1.0f) * activeMat.specular; 

	return (diffuse + specular + ambient) * attenuation;
}


vec4 CalcSpotLight(int lightIndex, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	int i = lightIndex;
	vec3 lightDir = normalize(spotLights[i].position - FragPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float lightFragDistance = length(spotLights[i].position - FragPos);
	float attenuation = 1.0f / (spotLights[i].constant + spotLights[i].linear * lightFragDistance +
	spotLights[i].quadratic * lightFragDistance * lightFragDistance);
	//	Ambient
	vec4 ambient = vec4(spotLights[i].ambient, 1.0f) * activeMat.ambient;
	//	diffuse
	vec4 diffuse = vec4(max(dot(normal, lightDir), 0.0) * spotLights[i].diffuse, 1.0f) * activeMat.diffuse;
	//	specular
	vec4 specular = vec4(pow(max(dot(normal, halfWayDir), 0.0), activeMat.shininess) * spotLights[i].specular, 1.0f) * activeMat.specular;

	float theta = dot(lightDir, normalize(-spotLights[i].direction));
	float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
	float intensity = clamp((theta - spotLights[i].outerCutOff)/epsilon, 0.0f, 1.0f);

	return ((diffuse + specular) * intensity + ambient) * attenuation;
}