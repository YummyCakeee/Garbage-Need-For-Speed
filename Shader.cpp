#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

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

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	sceneInfo = new SceneInfo();
	const char* vShaderCode;
	const char* fShaderCode;

	std::string vTempString;
	std::string fTempString;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);


	try
	{
		std::stringstream vShaderStream;
		vShaderFile.open(vertexPath);
		vShaderStream << vShaderFile.rdbuf();
		vShaderFile.close();
		vTempString = vShaderStream.str();
		vShaderCode = vTempString.c_str();

		std::stringstream fShaderStream;
		fShaderFile.open(fragmentPath);
		fShaderStream << fShaderFile.rdbuf();
		fShaderFile.close();
		fTempString = fShaderStream.str();
		fShaderCode = fTempString.c_str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
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

	programID = glCreateProgram();
	glAttachShader(programID, vertex);
	glAttachShader(programID, fragment);
	glLinkProgram(programID);
	checkCompileErrors(programID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	sceneInfo = NULL;
	camera = NULL;
	modelMatrix = NULL;
}
Shader::~Shader()
{
	glDeleteProgram(programID);
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

void Shader::loadCameraAndModelMatrix(const Camera* camera, const glm::mat4* modelMatrix) const
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
}

void Shader::loadMainInfo(const Camera* camera, const glm::mat4* modelMatrix) const
{
	loadCameraAndModelMatrix(camera, modelMatrix);
}

void Shader::setCameraAndModelMatrix(const Camera* camera, const glm::mat4* modelMatrix)
{
	this->camera = camera;
	this->modelMatrix = modelMatrix;
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

LightsUBO::LightsUBO()
{
	uboBuf = 0;
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
	glGenBuffers(1, &uboBuf);
	glBindBuffer(GL_UNIFORM_BUFFER, uboBuf);
	//	Память на три переменные-счётчики числа источников света + источники направленного + точечного + прожекторного
	glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(int) + dirLightsCnt * 4 * sizeof(glm::vec3) +
		pntLightsCnt * (4 * sizeof(glm::vec3) + 3 * sizeof(float)) +
		sptLightsCnt * (5 * sizeof(glm::vec4)), NULL, GL_STATIC_DRAW);

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
	glBufferSubData(GL_UNIFORM_BUFFER, countersOffsets[0], sizeof(int), &zero);
	glBufferSubData(GL_UNIFORM_BUFFER, countersOffsets[1], sizeof(int), &zero);
	glBufferSubData(GL_UNIFORM_BUFFER, countersOffsets[2], sizeof(int), &zero);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//	Привязка буфера к точке
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboBuf);
}

void LightsUBO::LoadInfo(const std::vector<const LightSource*>& lights)
{
	glBindBuffer(GL_UNIFORM_BUFFER, uboBuf);
	int loadDirLghtCnt = 0;
	int loadPntLghtCnt = 0;
	int loadSptLghtCnt = 0;

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
	glBufferSubData(GL_UNIFORM_BUFFER, countersOffsets[0], sizeof(int), &loadDirLghtCnt);
	glBufferSubData(GL_UNIFORM_BUFFER, countersOffsets[1], sizeof(int), &loadPntLghtCnt);
	glBufferSubData(GL_UNIFORM_BUFFER, countersOffsets[2], sizeof(int), &loadSptLghtCnt);

	//	Отвязка буфера
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}