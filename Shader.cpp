#include "Shader.h"

SceneInfo::SceneInfo()
{
}

SceneInfo::~SceneInfo()
{
}

unsigned int Shader::ID() const
{
	return programID;
}

Shader::Shader(ShaderType type, const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	this->type = type;
	const char* vShaderCode;
	const char* fShaderCode;
	const char* gShaderCode;
	std::string vCode, fCode, gCode;
	try
	{
		vCode = readShaderFromFile(vertexPath);
		vShaderCode = vCode.c_str();
		fCode = readShaderFromFile(fragmentPath);
		fShaderCode = fCode.c_str();
		
		if (geometryPath != NULL)
		{
			gCode = readShaderFromFile(geometryPath);
			gShaderCode = gCode.c_str();
		}
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		return;
	}

	unsigned int vertex;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	unsigned int fragment;
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	unsigned int geometry;
	if (geometryPath != NULL)
	{
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}

	programID = glCreateProgram();
	glAttachShader(programID, vertex);
	glAttachShader(programID, fragment);
	if (geometryPath != NULL)
		glAttachShader(programID, geometry);
	glLinkProgram(programID);
	checkCompileErrors(programID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != NULL)
		glDeleteShader(geometry);
}

Shader::~Shader()
{
	glDeleteProgram(programID);
}

std::string Shader::readShaderFromFile(const char* path)
{
	std::string tmpCode;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::stringstream shaderStream;
	shaderFile.open(path);
	shaderStream << shaderFile.rdbuf();
	shaderFile.close();
	tmpCode = shaderStream.str();
	
	return tmpCode;
}

void Shader::use() const
{
	glUseProgram(programID);
}

void Shader::free()
{
	glDeleteProgram(programID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setVec(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
}


void Shader::setVec(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}

void Shader::setVec(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
}

void Shader::setVec(const std::string& name, glm::vec2 vector) const
{
	setVec(name, vector.x, vector.y);
}

void Shader::setVec(const std::string& name, glm::vec3 vector) const
{
	setVec(name, vector.x, vector.y, vector.z);
}

void Shader::setVec(const std::string& name, glm::vec4 vector) const
{
	setVec(name, vector.x, vector.y, vector.z, vector.w);
}

void Shader::setMatrix4F(const std::string& name, glm::mat4& m) const
{
	glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(m));
}

ShaderType Shader::GetType() const
{
	return type;
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << '\n' << infoLog << "\n -- -------------";
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << '\n' << infoLog << "\n -- -------------";
		}
	}
}

MaterialShader::MaterialShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) :
	Shader(ShaderType::MATERIAL, vertexPath, fragmentPath, geometryPath)
{
	camera = NULL;
	modelMatrix = NULL;
}

MaterialShader::~MaterialShader()
{
}

void MaterialShader::loadCameraAndModelMatrix(const Camera* camera, const glm::mat4* modelMatrix) const
{
	const Camera* cam = this->camera;
	if (camera != NULL) cam = camera;
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	if (cam != NULL)
	{
		setVec("viewPos", cam->GetPosition());
		projection = cam->GetProjectionMatrix();
		view = cam->GetViewMatrix();
	}
	else
	{
		std::cout << "WARNING:: No camera set to object which is using shader ID: " << programID << std::endl;
		setVec("viewPos", glm::vec3(0.0f));
	}
	const glm::mat4* modelMat = this->modelMatrix;
	if (modelMatrix != NULL) modelMat = modelMatrix;
	glm::mat4 model;
	if (modelMat == NULL)

	{
		model = glm::mat4(1.0f);
		std::cout << "WARNING: No model matrix set to object which is using shader ID: " << programID << std::endl;
	}
	else model = *modelMat;
	glm::mat4 finalMatrix = projection * view * model;
	setMatrix4F("finalMatrix", finalMatrix);
	setMatrix4F("model", model);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));
	setMatrix4F("normalMatrix", normalMatrix);
	//	TEST
	glm::mat4 lsMat = lightSpaceMat;
	setMatrix4F("lightSpaceMatrix", lsMat);
	//	~TEST
}

void MaterialShader::setCameraAndModelMatrix(const Camera* camera, const glm::mat4* modelMatrix)
{
	this->camera = camera;
	this->modelMatrix = modelMatrix;
}
void MaterialShader::loadMainInfo(const Camera* camera, const glm::mat4* modelMatrix) const
{
	loadCameraAndModelMatrix(camera, modelMatrix);
}

ShadowMapShader::ShadowMapShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) :
	Shader(ShaderType::SHADOW_MAP, vertexPath, fragmentPath, geometryPath)
{
	lightSpaceMatrix = NULL;
	modelMatrix = NULL;
}

ShadowMapShader::~ShadowMapShader()
{
}


void ShadowMapShader::loadLightSpaceAndModelMatrix(const glm::mat4* lightSpaceMatrix, const glm::mat4* modelMatrix) const
{
	const glm::mat4* lightSpaceMat = this->lightSpaceMatrix;
	if (lightSpaceMatrix != NULL) lightSpaceMat = lightSpaceMatrix;
	glm::mat4 lsMat = glm::mat4(1.0f);
	if (lightSpaceMat != NULL) lsMat = *lightSpaceMat;
	else
	{
		std::cout << "WARNING:: No light space matrix set to object which is using shader ID: " << programID << std::endl;
	}
	const glm::mat4* modelMat = this->modelMatrix;
	if (modelMatrix != NULL) modelMat = modelMatrix;
	glm::mat4 model = glm::mat4(1.0f);
	if (modelMat == NULL)
	{
		std::cout << "WARNING: No model matrix set to object which is using shader ID: " << programID << std::endl;
	}
	else model = *modelMat;
	glm::mat4 finalMatrix = lsMat * model;
	setMatrix4F("finalMatrix", finalMatrix);
}

void ShadowMapShader::loadMainInfo(const glm::mat4* lightSpaceMatrix, const glm::mat4* modelMatrix) const
{
	loadLightSpaceAndModelMatrix(lightSpaceMatrix, modelMatrix);
}

void ShadowMapShader::setLightSpaceAndModelMatrix(const glm::mat4* lightSpaceMatrix, const glm::mat4* modelMatrix)
{
	this->lightSpaceMatrix = lightSpaceMatrix;
	this->modelMatrix = modelMatrix;
}



ScreenShader::ScreenShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) :
	Shader(ShaderType::SCREEN, vertexPath, fragmentPath, geometryPath)
{
	playerSpeed = glm::dvec3(0.0f);
}

ScreenShader::~ScreenShader()
{
}

void ScreenShader::loadPlayerSpeed(const glm::dvec3* playerSpeed) const
{
	glm::dvec3 plrSpeed = this->playerSpeed;
	if (playerSpeed != NULL) plrSpeed = *playerSpeed;
	setVec("playerSpeed", (glm::vec3)plrSpeed);
}

void ScreenShader::loadMainInfo(const glm::dvec3* playerSpeed) const
{
	loadPlayerSpeed(playerSpeed);
}

void ScreenShader::setPlayerSpeed(glm::dvec3 playerSpeed)
{
	this->playerSpeed = playerSpeed;
}

LightsUBO::LightsUBO()
{
	lightsUBO = 0;
	dirLightsCnt = 0;
	pntLightsCnt = 0;
	sptLightsCnt = 0;
	dirLghtOffsets = NULL;
	pntLghtOffsets = NULL;
	sptLghtOffsets = NULL;
}

LightsUBO::LightsUBO(const Shader& shader, int dirLightsCnt, int pntLightsCnt, int sptLightsCnt)
{
	this->dirLightsCnt = glm::max(0, dirLightsCnt);
	this->pntLightsCnt = glm::max(0, pntLightsCnt);
	this->sptLightsCnt = glm::max(0, sptLightsCnt);
	this->dirLghtOffsets = new GLint[this->dirLightsCnt * 4];
	this->pntLghtOffsets = new GLint[this->pntLightsCnt * (4 + 3)];
	this->sptLghtOffsets = new GLint[this->sptLightsCnt * (5 + 5)];
	GenerateBuffer(shader);
}

LightsUBO::~LightsUBO()
{
	/*
	delete[] dirLghtOffsets;
	delete[] pntLghtOffsets;
	delete[] sptLghtOffsets;
	*/
}

void LightsUBO::GenerateBuffer(const Shader& shader)
{
	//	Создание буфера для источников
	glGenBuffers(1, &lightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	//	Память на три переменную-счётчик числа источников света + источники света
	int blockSize;
	glGetActiveUniformBlockiv(shader.ID(), 0,
		GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	//	Получение сдвигов для счётчиков источников света
	std::string uniformNamesTmp[] =
	{
		"dirLigtsCnt",
		"pntLigtsCnt",
		"sptLigtsCnt",
	};
	const char* uniformNames[] =
	{
		uniformNamesTmp[0].c_str(),
		uniformNamesTmp[1].c_str(),
		uniformNamesTmp[2].c_str()
	};
	GLuint indices[3];
	glGetUniformIndices(shader.ID(), 3, uniformNames, indices);
	glGetActiveUniformsiv(shader.ID(), 3, indices, GL_UNIFORM_OFFSET, countersOffsets);

	//	Получение сдвигов для направленных источников света
	for (int i = 0; i < dirLightsCnt; i++)
	{
		std::string uniformNamesTmp[] =
		{
			"dirLights[" + std::to_string(i) + "].ambient",
			"dirLights[" + std::to_string(i) + "].diffuse",
			"dirLights[" + std::to_string(i) + "].specular",
			"dirLights[" + std::to_string(i) + "].direction"
		};
		const char* uniformNames[] =
		{
			uniformNamesTmp[0].c_str(),
			uniformNamesTmp[1].c_str(),
			uniformNamesTmp[2].c_str(),
			uniformNamesTmp[3].c_str()
		};
		GLuint indices[4];
		glGetUniformIndices(shader.ID(), 4, uniformNames, indices);
		glGetActiveUniformsiv(shader.ID(), 4, indices, GL_UNIFORM_OFFSET, &dirLghtOffsets[i * 4]);
	}
	//	Получение сдвигов для точечных источников света
	for (int i = 0; i < pntLightsCnt; i++)
	{
		std::string uniformNamesTmp[] =
		{
			"pointLights[" + std::to_string(i) + "].ambient",
			"pointLights[" + std::to_string(i) + "].diffuse",
			"pointLights[" + std::to_string(i) + "].specular",
			"pointLights[" + std::to_string(i) + "].position",
			"pointLights[" + std::to_string(i) + "].constant",
			"pointLights[" + std::to_string(i) + "].linear",
			"pointLights[" + std::to_string(i) + "].quadratic"
		};
		const char* uniformNames[] =
		{
			uniformNamesTmp[0].c_str(),
			uniformNamesTmp[1].c_str(),
			uniformNamesTmp[2].c_str(),
			uniformNamesTmp[3].c_str(),
			uniformNamesTmp[4].c_str(),
			uniformNamesTmp[5].c_str(),
			uniformNamesTmp[6].c_str()
		};
		GLuint indices[7];
		glGetUniformIndices(shader.ID(), 7, uniformNames, indices);
		glGetActiveUniformsiv(shader.ID(), 7, indices, GL_UNIFORM_OFFSET, &pntLghtOffsets[i * 7]);
	}
	//	Получение сдвигов для прожекторных источников света
	for (int i = 0; i < sptLightsCnt; i++)
	{
		std::string uniformNamesTmp[] =
		{
			"spotLights[" + std::to_string(i) + "].ambient",
			"spotLights[" + std::to_string(i) + "].diffuse",
			"spotLights[" + std::to_string(i) + "].specular",
			"spotLights[" + std::to_string(i) + "].position",
			"spotLights[" + std::to_string(i) + "].direction",
			"spotLights[" + std::to_string(i) + "].cutOff",
			"spotLights[" + std::to_string(i) + "].outerCutOff",
			"spotLights[" + std::to_string(i) + "].constant",
			"spotLights[" + std::to_string(i) + "].linear",
			"spotLights[" + std::to_string(i) + "].quadratic"
		};
		const char* uniformNames[] =
		{
			uniformNamesTmp[0].c_str(),
			uniformNamesTmp[1].c_str(),
			uniformNamesTmp[2].c_str(),
			uniformNamesTmp[3].c_str(),
			uniformNamesTmp[4].c_str(),
			uniformNamesTmp[5].c_str(),
			uniformNamesTmp[6].c_str(),
			uniformNamesTmp[7].c_str(),
			uniformNamesTmp[8].c_str(),
			uniformNamesTmp[9].c_str()
		};
		GLuint indices[10];
		glGetUniformIndices(shader.ID(), 10, uniformNames, indices);
		glGetActiveUniformsiv(shader.ID(), 10, indices, GL_UNIFORM_OFFSET, &sptLghtOffsets[i * 10]);
	}

	//	Задание начальных значений для числа источников света
	int zero = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, countersOffsets[0], sizeof(int), &zero);
	glBufferSubData(GL_UNIFORM_BUFFER, countersOffsets[1], sizeof(int), &zero);
	glBufferSubData(GL_UNIFORM_BUFFER, countersOffsets[2], sizeof(int), &zero);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//	Привязка буфера к точке
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsUBO);
}

void LightsUBO::LoadInfo(const std::vector<const LightSource*>& lights)
{
	int loadDirLghtCnt = 0;
	int loadPntLghtCnt = 0;
	int loadSptLghtCnt = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	//	Загрузка в буфер данных источников света
	for (int i = 0; i < lights.size(); i++)
	{
		if (!lights[i]->IsEnabled()) continue;
		switch (lights[i]->GetType())
		{
		case SourceType::DIRECTIONAL:
		{
			if (loadDirLghtCnt >= dirLightsCnt)
			{
				//std::cout << "LIGHTS_UBO:: WARNING:: Maximum of Directed Lights is set!" << std::endl;
				break;
			}
			const DirLight* light = (const DirLight*)lights[i];
			glBufferSubData(GL_UNIFORM_BUFFER, dirLghtOffsets[loadDirLghtCnt * 4], sizeof(glm::vec3), &(light->GetAmbient()));
			glBufferSubData(GL_UNIFORM_BUFFER, dirLghtOffsets[loadDirLghtCnt * 4 + 1], sizeof(glm::vec3), &(light->GetDiffuse()));
			glBufferSubData(GL_UNIFORM_BUFFER, dirLghtOffsets[loadDirLghtCnt * 4 + 2], sizeof(glm::vec3), &(light->GetSpecular()));
			glBufferSubData(GL_UNIFORM_BUFFER, dirLghtOffsets[loadDirLghtCnt * 4 + 3], sizeof(glm::vec3), &(light->GetDirection()));
			loadDirLghtCnt++;
		};break;
		case SourceType::POINT:
		{
			if (loadPntLghtCnt >= pntLightsCnt)
			{
				//std::cout << "LIGHTS_UBO:: WARNING:: Maximum of Point Lights is set!" << std::endl;
				break;
			}
			const PointLight* light = (const PointLight*)lights[i];
			float props[] = { light->GetConstant(), light->GetLinear(), light->GetQuadratic() };
			glBufferSubData(GL_UNIFORM_BUFFER, pntLghtOffsets[loadPntLghtCnt * 7], sizeof(glm::vec3), &(light->GetAmbient()));
			glBufferSubData(GL_UNIFORM_BUFFER, pntLghtOffsets[loadPntLghtCnt * 7 + 1], sizeof(glm::vec3), &(light->GetDiffuse()));
			glBufferSubData(GL_UNIFORM_BUFFER, pntLghtOffsets[loadPntLghtCnt * 7 + 2], sizeof(glm::vec3), &(light->GetSpecular()));
			glBufferSubData(GL_UNIFORM_BUFFER, pntLghtOffsets[loadPntLghtCnt * 7 + 3], sizeof(glm::vec3), &(light->GetPosition()));
			glBufferSubData(GL_UNIFORM_BUFFER, pntLghtOffsets[loadPntLghtCnt * 7 + 4], sizeof(float), &props[0]);
			glBufferSubData(GL_UNIFORM_BUFFER, pntLghtOffsets[loadPntLghtCnt * 7 + 5], sizeof(float), &props[1]);
			glBufferSubData(GL_UNIFORM_BUFFER, pntLghtOffsets[loadPntLghtCnt * 7 + 6], sizeof(float), &props[2]);
			loadPntLghtCnt++;
		};break;
		case SourceType::SPOTLIGHT:
		{
			if (loadSptLghtCnt >= sptLightsCnt)
			{
				//std::cout << "LIGHTS_UBO:: WARNING:: Maximum of Spot Lights is set!" << std::endl;
				break;
			}
			const SpotLight* light = (const SpotLight*)lights[i];
			float props[] = { light->GetCutOff(), light->GetOuterCutOff(), light->GetConstant(), light->GetLinear(), light->GetQuadratic() };
			glBufferSubData(GL_UNIFORM_BUFFER, sptLghtOffsets[loadSptLghtCnt * 10], sizeof(glm::vec3), &(light->GetAmbient()));
			glBufferSubData(GL_UNIFORM_BUFFER, sptLghtOffsets[loadSptLghtCnt * 10 + 1], sizeof(glm::vec3), &(light->GetDiffuse()));
			glBufferSubData(GL_UNIFORM_BUFFER, sptLghtOffsets[loadSptLghtCnt * 10 + 2], sizeof(glm::vec3), &(light->GetSpecular()));
			glBufferSubData(GL_UNIFORM_BUFFER, sptLghtOffsets[loadSptLghtCnt * 10 + 3], sizeof(glm::vec3), &(light->GetPosition()));
			glBufferSubData(GL_UNIFORM_BUFFER, sptLghtOffsets[loadSptLghtCnt * 10 + 4], sizeof(glm::vec3), &(light->GetDirection()));
			glBufferSubData(GL_UNIFORM_BUFFER, sptLghtOffsets[loadSptLghtCnt * 10 + 5], sizeof(float), &props[0]);
			glBufferSubData(GL_UNIFORM_BUFFER, sptLghtOffsets[loadSptLghtCnt * 10 + 6], sizeof(float), &props[1]);
			glBufferSubData(GL_UNIFORM_BUFFER, sptLghtOffsets[loadSptLghtCnt * 10 + 7], sizeof(float), &props[2]);
			glBufferSubData(GL_UNIFORM_BUFFER, sptLghtOffsets[loadSptLghtCnt * 10 + 8], sizeof(float), &props[3]);
			glBufferSubData(GL_UNIFORM_BUFFER, sptLghtOffsets[loadSptLghtCnt * 10 + 9], sizeof(float), &props[4]);
			loadSptLghtCnt++;
		};break;
		default: break;
		}
	}

	//	Загрузка в буфер информации о числе источников света
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, countersOffsets[0], sizeof(int), &loadDirLghtCnt);
	//glBindBuffer(GL_UNIFORM_BUFFER, pntLightsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, countersOffsets[1], sizeof(int), &loadPntLghtCnt);
	//glBindBuffer(GL_UNIFORM_BUFFER, sptLightsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, countersOffsets[2], sizeof(int), &loadSptLghtCnt);

	//	Отвязка буфера
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}