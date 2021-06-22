#include "Shader.h"

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

void Shader::setMatrix4F(const std::string& name, const glm::mat4& m) const
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

void Shader::draw(unsigned int VAO, size_t indicesCount) const
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Shader::clearSamplers() const
{
}

void Shader::clearShaderInfo()
{
}

void Shader::clear()
{
	clearSamplers();
	clearShaderInfo();
}

LightInfo::LightInfo(const std::list<glm::mat4>& lightSpaceMats, unsigned int shadowMapID, SourceType type, float farPlane)
{
	this->lightSpaceMats = lightSpaceMats;
	this->shadowMapID = shadowMapID;
	this->type = type;
	this->farPlane = farPlane;
}

LightInfo::LightInfo(const glm::mat4& lightSpaceMat, unsigned int shadowMapID, SourceType type, float farPlane)
{
	this->lightSpaceMats.push_back(lightSpaceMat);
	this->shadowMapID = shadowMapID;
	this->type = type;
	this->farPlane = farPlane;
}

LightInfo::LightInfo(unsigned int shadowMapID, SourceType type, float farPlane)
{
	this->shadowMapID = shadowMapID;
	this->type = type;
	this->farPlane = farPlane;
}

MaterialShaderInfo::MaterialShaderInfo()
{
	modelMatrix = glm::mat4(1.0f);
	spaceMatrix = glm::mat4(1.0f);
	viewPos = glm::vec3(0.0f);
}

MaterialShader::MaterialShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) :
	Shader(ShaderType::MATERIAL, vertexPath, fragmentPath, geometryPath)
{
	maxMatAndSkyboxTexsCnt = 9;
}

MaterialShader::~MaterialShader()
{
}

void MaterialShader::loadMatrices(const glm::vec3* viewPos, const glm::mat4* spaceMatrix, const glm::mat4* modelMatrix) const
{
	//	Позиция наблюдателя
	if (viewPos != NULL) setVec("viewPos", *viewPos);
	else setVec("viewPos", shaderInfo.viewPos);
	//	Матрица пространства
	glm::mat4 spaceMat = shaderInfo.spaceMatrix;
	if (spaceMatrix != NULL) spaceMat = *spaceMatrix;
	//	Модельная матрица
	glm::mat4 modelMat = shaderInfo.modelMatrix;
	if (modelMatrix != NULL) modelMat = *modelMatrix;
	setMatrix4F("model", modelMat);
	//	Обратная и транспонированная модельная матрица
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMat));
	setMatrix4F("normalMatrix", normalMatrix);
	//	Конечная матрица
	glm::mat4 finalMatrix = spaceMat * modelMat;
	setMatrix4F("finalMatrix", finalMatrix);

}

void MaterialShader::loadMaterial(const Material* material) const
{
	setBool("hasSkybox", false);
	if (material != NULL)
	{
		unsigned int diffuseN = 1;
		unsigned int specularN = 1;
		unsigned int normalN = 1;
		unsigned int heightN = 1;
		unsigned int cubeMapN = 1;
		unsigned int samplerIndex = 0;

		const std::vector<Texture>* textures = material->GetTextures();
		if (textures != NULL)
		{
			for (int i = 0; i < textures->size(); i++)
			{
				std::string name = "material.";
				switch ((*textures)[i].GetType())
				{
				case TextureType::TEXTURE2D:
				{
					switch ((*textures)[i].GetDataType())
					{
					case TextureDataType::DIFFUSE:
						if (diffuseN > 2) continue;
						name += "texture_diffuse" + std::to_string(diffuseN++);
						break;
					case TextureDataType::SPECULAR:
						if (specularN > 2) continue;
						name += "texture_specular" + std::to_string(specularN++);
						break;
					case TextureDataType::NORMAL:
						if (normalN > 2) continue;
						name += "texture_normal" + std::to_string(normalN++);
						break;
					case TextureDataType::HEIGHT:
						if (heightN > 2) continue;
						name += "texture_height" + std::to_string(heightN++);
						break;
					}
				}; break;
				case TextureType::CUBEMAP:
					if (cubeMapN > 1) continue;
					name = "skybox";
					setBool("hasSkybox", true);
					cubeMapN++;
					break;
				default: continue;
				}
				setInt(name, samplerIndex);
				glActiveTexture(GL_TEXTURE0 + samplerIndex);
				samplerIndex++;
				if ((*textures)[i].GetType() == TextureType::CUBEMAP)
				{
					glBindTexture(GL_TEXTURE_CUBE_MAP, (*textures)[i].GetId());
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, (*textures)[i].GetId());
				}
			}
		}
		setInt("material.diffTextCount", diffuseN - 1);
		setInt("material.specTextCount", specularN - 1);
		setInt("material.ambTextCount", heightN - 1);
		setVec("material.diffuse", material->GetColor(MaterialType::DIFFUSE));
		setVec("material.ambient", material->GetColor(MaterialType::AMBIENT));
		setVec("material.specular", material->GetColor(MaterialType::SPECULAR));
		setFloat("material.shininess", material->GetProperty(MaterialProp::SHININESS));
		setFloat("material.alpha", material->GetProperty(MaterialProp::ALPHA));
		setFloat("material.reflectivity", material->GetProperty(MaterialProp::REFLECTIVITY));

	}
	else
	{
		setInt("material.diffTextCount", 0);
		setInt("material.specTextCount", 0);
		setInt("material.ambTextCount", 0);
		setVec("material.diffuse", glm::vec4(1.0f, 0.0f, 0.9f, 1.0f));
		setVec("material.ambient", glm::vec4(1.0f, 0.0f, 0.9f, 1.0f));
		setVec("material.specular", glm::vec4(0.0f));
		setFloat("material.shininess", 32.0f);
		setFloat("material.alpha", 1.0f);
		setFloat("material.reflectivity", 0.0f);
	}
}

void MaterialShader::loadLightsInfo(const std::list<LightInfo>*) const
{
	int dLightsIndex = 0;
	int pLightsIndex = 0;
	int sLightsIndex = 0;
	int i = maxMatAndSkyboxTexsCnt;
	for (auto it = shaderInfo.lightsInfo.begin(); it != shaderInfo.lightsInfo.end(); it++, i++)
	{
		switch (it->type)
		{
		case SourceType::DIRECTIONAL:
		{
			if (it->lightSpaceMats.size() > 0)
				setMatrix4F("dLightSpaceMatrix[" + std::to_string(dLightsIndex) + "]", it->lightSpaceMats.front());
			else setMatrix4F("dLightSpaceMatrix[" + std::to_string(dLightsIndex) + "]", glm::mat4(1.0f));
			setInt("dLightShadowMaps[" + std::to_string(dLightsIndex) + "]", i);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, it->shadowMapID);
			dLightsIndex++;
		}; break;
		case SourceType::POINT:
		{
			setFloat("pLightFarPlane[" + std::to_string(pLightsIndex) + "]", it->farPlane);
			setInt("pLightShadowMaps[" + std::to_string(pLightsIndex) + "]", i);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_CUBE_MAP, it->shadowMapID);
			pLightsIndex++;
		}; break;
		case SourceType::SPOTLIGHT:
		{
			if (it->lightSpaceMats.size() > 0)
				setMatrix4F("sLightSpaceMatrix[" + std::to_string(sLightsIndex) + "]", it->lightSpaceMats.front());
			else setMatrix4F("sLightSpaceMatrix[" + std::to_string(sLightsIndex) + "]", glm::mat4(1.0f));
			setInt("sLightShadowMaps[" + std::to_string(sLightsIndex) + "]", i);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, it->shadowMapID);
			sLightsIndex++;
		}; break;
		default:
			break;
		}
	}
}

void MaterialShader::setSpaceMatrix(const glm::mat4& spaceMatrix)
{
	shaderInfo.spaceMatrix = spaceMatrix;
}

void MaterialShader::setViewPos(const glm::vec3& position)
{
	shaderInfo.viewPos = position;
}

void MaterialShader::setModelMatrix(const glm::mat4& modelMatrix)
{
	this->shaderInfo.modelMatrix = modelMatrix;
}

void MaterialShader::addLightInfo(const LightInfo& light)
{
	shaderInfo.lightsInfo.push_back(light);
}

void MaterialShader::loadMainInfo(const glm::vec3* viewPos, const glm::mat4* spaceMatrix, const glm::mat4* modelMatrix, const Material* material) const
{
	loadMatrices(viewPos, spaceMatrix, modelMatrix);
	loadMaterial(material);
	loadLightsInfo();
}

void MaterialShader::clearSamplers() const
{
	use();
	setBool("hasSkybox", false);
	//	Очистка текстур материала
	std::string name = "material.";
	for (int i = 1; i <= 2; i++)
	{
		setInt("material.texture_diffuse" + std::to_string(i), 30);
		setInt("material.texture_specular" + std::to_string(i), 30);
		setInt("material.texture_normal" + std::to_string(i), 30);
		setInt("material.texture_height" + std::to_string(i), 30);
	}
	setInt("skybox", 31);
	//	Очистка карт теней
	for (int i = 0; i < 1; i++)
	{
		setInt("dLightShadowMaps[" + std::to_string(i) + "]", 30);
	}
	for (int i = 0; i < 2; i++)
	{
		setInt("pLightShadowMaps[" + std::to_string(i) + "]", 31); 
	}
	for (int i = 0; i < 8; i++)
	{
		setInt("sLightShadowMaps[" + std::to_string(i) + "]", 30);
	}
}

void MaterialShader::clearShaderInfo()
{
	shaderInfo.modelMatrix = glm::mat4(1.0f);
	shaderInfo.spaceMatrix = glm::mat4(1.0f);
	shaderInfo.viewPos = glm::vec3(0.0f);
	shaderInfo.lightsInfo.clear();
}

ShadowMapShaderInfo::ShadowMapShaderInfo()
{
	modelMatrix = glm::mat4(1.0f);
	lightPos = glm::vec3(0.0f);
	farPlane = 1.0f;
	linearizeDepth = false;
}


ShadowMapShader::ShadowMapShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) :
	Shader(ShaderType::SHADOW_MAP, vertexPath, fragmentPath, geometryPath)
{
}

ShadowMapShader::~ShadowMapShader()
{
}


void ShadowMapShader::loadMatrices(const std::list<glm::mat4>* lightSpaceMatrices, const glm::mat4* modelMatrix) const
{
	//	Модельная матрица
	glm::mat4 modelMat = shaderInfo.modelMatrix;
	if (modelMatrix != NULL) modelMat = *modelMatrix;
	setMatrix4F("model", modelMat);
	//	Пространственная(-ые) матрица(-ы) света
	const std::list<glm::mat4>* lightSpaceMats;
	if (lightSpaceMatrices != NULL) lightSpaceMats = lightSpaceMatrices;
	else lightSpaceMats = &shaderInfo.lightSpaceMatrices;
	//	Одна матрица
	if (lightSpaceMats->size() == 1)
	{
		//	Конечная матрица
		glm::mat4 finalMatrix = lightSpaceMats->front() * modelMat;
		setMatrix4F("finalMatrix", finalMatrix);
	}
	//	Шесть матриц
	else if (lightSpaceMats->size() >= 6)
	{
		int index = 0;
		for (auto it = lightSpaceMats->begin(); it != lightSpaceMats->end(); it++)
		{
			glm::mat4 mat = *it;
			setMatrix4F("lightSpaceMatrices[" + std::to_string(index++) + "]", mat);
		}
	}
}

void ShadowMapShader::loadLightPosAndFarPlane(const glm::vec3* lightPos, float farPlane) const
{
	//	Включить линеаризацию значений
	setBool("linearize", shaderInfo.linearizeDepth);
	if (!shaderInfo.linearizeDepth) return;
	//	Позиция источника света
	if (lightPos != NULL) setVec("lightPos", *lightPos);
	else setVec("lightPos", shaderInfo.lightPos);
	//	Установка расстояния до дальней плоскости
	if (farPlane != 0.0f) setFloat("farPlane", farPlane);
	else setFloat("farPlane", shaderInfo.farPlane);
}

void ShadowMapShader::loadMaterial(const Material* material) const
{
	setBool("material.useDiffMap", false);
	if (material != NULL)
	{
		const std::vector<Texture>* textures = material->GetTextures();
		if (textures != NULL)
		{
			for (int i = 0; i < textures->size(); i++)
			{
				if ((*textures)[i].GetType() == TextureType::TEXTURE2D)
				{
					if ((*textures)[i].GetDataType() == TextureDataType::DIFFUSE)
					{
						setInt("material.texture_diffuse", 0);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, (*textures)[i].GetId());
						setBool("material.useDiffMap", true);
						break;
					}
				}
			}
		}
		setVec("material.diffuse", material->GetColor(MaterialType::DIFFUSE));
	}
	else setVec("material.diffuse", glm::vec4(1.0f, 0.0f, 0.9f, 1.0f));
}

void ShadowMapShader::loadMainInfo(const std::list<glm::mat4>* lightSpaceMatrices, const glm::mat4* modelMatrix,
	const glm::vec3* lightPos, float farPlane, const Material* material) const
{
	loadMatrices(lightSpaceMatrices, modelMatrix);
	loadLightPosAndFarPlane(lightPos, farPlane);
	loadMaterial(material);
}

void ShadowMapShader::setLightSpaceMatrix(const glm::mat4& lightSpaceMatrix)
{
	shaderInfo.lightSpaceMatrices.clear();
	shaderInfo.lightSpaceMatrices.push_back(lightSpaceMatrix);
}

void ShadowMapShader::setLightSpaceMatrices(std::list<glm::mat4>& lightSpaceMatrices)
{
	shaderInfo.lightSpaceMatrices = lightSpaceMatrices;
}

void ShadowMapShader::setModelMatrix(const glm::mat4& modelMatrix)
{
	this->shaderInfo.modelMatrix = modelMatrix;
}

void ShadowMapShader::setLightPos(const glm::vec3& lightPos)
{
	shaderInfo.lightPos = lightPos;
}
void ShadowMapShader::setFarPlane(float farPlane)
{
	shaderInfo.farPlane = farPlane;
}

void ShadowMapShader::enableLinearDepth(bool enable)
{
	shaderInfo.linearizeDepth = enable;
}

void ShadowMapShader::clearSamplers() const
{
	use();
	setBool("material.useDiffMap", false);
	setInt("material.texture_diffuse", 30);
}

void ShadowMapShader::clearShaderInfo()
{
	shaderInfo.lightPos = glm::vec3(0.0f);
	shaderInfo.lightSpaceMatrices.clear();
	shaderInfo.modelMatrix = glm::mat4(1.0f);
	shaderInfo.farPlane = 1.0f;
	shaderInfo.linearizeDepth = false;
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

void LightsUBO::LoadInfo(const std::vector<const LightSource*>& lights) const
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

int LightsUBO::GetDirLightsCount() const
{
	return dirLightsCnt;
}

int LightsUBO::GetPointLightsCount() const
{
	return pntLightsCnt;
}

int LightsUBO::GetSpotLightsCount() const
{
	return sptLightsCnt;
}