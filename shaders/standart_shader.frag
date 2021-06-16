#version 450 core

#define NR_DIR_LIGHTS 1
#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 8

out vec4 FragColor;
in VS_OUT
{
	vec3 Normal;
	vec3 FragPos;
	vec4 FragPosLightSpace;
	vec2 TextureCoords;
}fs_in;

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

uniform vec3 viewPos;
uniform Material material;
uniform samplerCube skybox;
uniform bool hasSkybox;
uniform sampler2D shadowMap[10];

vec4 CalcDirLight(int lightIndex, vec3 normal, vec3 viewDir);
vec4 CalcPointLight(int lightIndex, vec3 normal, vec3 viewDir);
vec4 CalcSpotLight(int lightIndex, vec3 normal, vec3 viewDir);
float CalcShadow(vec4 fragPos, vec3 normal, vec3 lightDir);

ActiveMaterial activeMat = ActiveMaterial(vec4(0.0), vec4(0.0), vec4(0.0), 0.0f, 0.0f);

void main()
{
	//	MATERIAL PREPARING 
	FragColor = vec4(vec3(0.0f), 1.0f);
	if (material.ambTextCount <= 0)
	{
		activeMat.ambient = material.ambient;
	}
	else 
	{
		activeMat.ambient = texture2D(material.texture_diffuse1, fs_in.TextureCoords);
	}
	if (material.diffTextCount <= 0)
	{
		activeMat.diffuse = material.diffuse;
		activeMat.diffuse.a = material.alpha;
	}
	else 
	{
		activeMat.diffuse = texture2D(material.texture_diffuse1, fs_in.TextureCoords);
	}
	if (material.specTextCount <= 0)
	{
		activeMat.specular = material.specular;
	}
	else
	{
		activeMat.specular = texture2D(material.texture_specular1, fs_in.TextureCoords);
	}
	
	activeMat.shininess = material.shininess;
	activeMat.alpha = material.alpha;
	activeMat.ambient = activeMat.diffuse;
	activeMat.ambient.a = 0.0f;
	activeMat.specular.a = 0.0f;

	//	COLOR CALCULATING

	vec3 normal = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);

	//	Light Calculating
	
	//	Directional lights
	int dLightsC = min(dirLigtsCnt, NR_DIR_LIGHTS);
	for (int i = 0; i < dLightsC; i++)
	{
		FragColor += CalcDirLight(i, normal, viewDir);
	}

	//	Point lights
	int pLightsC = min(pntLigtsCnt, NR_POINT_LIGHTS);
	for (int i = 0; i < pLightsC; i++)
	{
		FragColor += CalcPointLight(i, normal, viewDir);
	}

	//	Spot lights
	int sLightsC = min(sptLigtsCnt, NR_SPOT_LIGHTS);
	for (int i = 0; i < sLightsC; i++)
	{
		FragColor += CalcSpotLight(i, normal, viewDir) / max(pow(length(viewPos - spotLights[i].position) + 1.0f, 2.0f) / 100.0f, 1.0f);	
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

vec4 CalcDirLight(int lightIndex, vec3 normal, vec3 viewDir)
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
	//	shadow
	float shadow = CalcShadow(fs_in.FragPosLightSpace, normal, lightDir);

	return ambient + (diffuse + specular) * (1.0f - shadow);
}

vec4 CalcPointLight(int lightIndex, vec3 normal, vec3 viewDir)
{
	int i = lightIndex;
	vec3 lightDir = normalize(pointLights[i].position - fs_in.FragPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float lightFragDistance = length(pointLights[i].position - fs_in.FragPos);
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


vec4 CalcSpotLight(int lightIndex, vec3 normal, vec3 viewDir)
{
	int i = lightIndex;
	vec3 lightDir = normalize(spotLights[i].position - fs_in.FragPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float lightFragDistance = length(spotLights[i].position - fs_in.FragPos);
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

float CalcShadow(vec4 fragPos, vec3 normal, vec3 lightDir)
{
	vec3 projCoords = fragPos.xyz / fragPos.w;
	projCoords = projCoords * 0.5f + 0.5f;
	if (projCoords.z > 1.0f) return 0.0;
	float currentDepth = projCoords.z;
	float bias = max(0.01 * (1.0f - dot(normal, lightDir)), 0.005f);
	float shadow = 0.0f;
	vec2 texelSize = 1.0f / textureSize(shadowMap[0], 0);
	for (int x = -1; x <= 1; ++x)
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap[0], projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
		}
	return shadow / 9.0f;
}