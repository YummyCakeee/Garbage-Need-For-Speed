#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

SceneInfo::SceneInfo()
{
	lights = NULL;
}

SceneInfo::~SceneInfo()
{
}

void SceneInfo::SetSceneLights(const std::vector<const LightSource*>* lights)
{
	this->lights = lights;
}

const std::vector<const LightSource*>* SceneInfo::GetSceneLights() const
{
	return lights;
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

void Shader::loadLights() const
{
	auto lights = sceneInfo->GetSceneLights();
	if (lights == NULL)
	{
		setInt("dirLgtsCnt", 0);
		setInt("pntLgtsCnt", 0);
		setInt("sptLgtsCnt", 0);
		return;
	}
	int pntLgtsCnt = 0;
	int sptLgtsCnt = 0;
	int dirLgtsCnt = 0;
	std::string name;
	std::string index;
	for (int i = 0; i < lights->size(); i++)
	{
		const LightSource* light = (*lights)[i];
		if (!light->IsEnabled()) continue;
		switch (light->GetType())
		{
		case SourceType::POINT:
		{
			PointLight* pl = (PointLight*)light;
			name = std::string("pointLight[");
			index = std::to_string(pntLgtsCnt);
			setVec((name + index + "].ambient").c_str(), pl->GetAmbient());
			setVec((name + index + "].diffuse").c_str(), pl->GetDiffuse());
			setVec((name + index + "].specular").c_str(), pl->GetSpecular());
			setVec((name + index + "].position").c_str(), pl->GetPosition());
			setFloat((name + index + "].constant").c_str(), pl->GetConstant());
			setFloat((name + index + "].linear").c_str(), pl->GetLinear());
			setFloat((name + index + "].quadratic").c_str(), pl->GetQuadratic());
			pntLgtsCnt++;
		}
		break;
		case SourceType::SPOTLIGHT:
		{
			SpotLight* sl = (SpotLight*)light;
			name = std::string("spotLight[");
			index = std::to_string(sptLgtsCnt);
			setVec((name + index + "].ambient").c_str(), sl->GetAmbient());
			setVec((name + index + "].diffuse").c_str(), sl->GetDiffuse());
			setVec((name + index + "].specular").c_str(), sl->GetSpecular());
			setVec((name + index + "].position").c_str(), sl->GetPosition());
			setVec((name + index + "].direction").c_str(), sl->GetDirection());
			setFloat((name + index + "].cutOff").c_str(), sl->GetCutOff());
			setFloat((name + index + "].outerCutOff").c_str(), sl->GetOuterCutOff());
			setFloat((name + index + "].constant").c_str(), sl->GetConstant());
			setFloat((name + index + "].linear").c_str(), sl->GetLinear());
			setFloat((name + index + "].quadratic").c_str(), sl->GetQuadratic());
			sptLgtsCnt++;
		}
		break;
		case SourceType::DIRECTIONAL:
		{
			DirLight* dl = (DirLight*)light;
			name = std::string("dirLight[");
			index = std::to_string(dirLgtsCnt);
			setVec((name + index + "].ambient").c_str(), dl->GetAmbient());
			setVec((name + index + "].diffuse").c_str(), dl->GetDiffuse());
			setVec((name + index + "].specular").c_str(), dl->GetSpecular());
			setVec((name + index + "].direction").c_str(), dl->GetDirection());
			dirLgtsCnt++;
		}
		break;
		default:
			break;
		}
	}
	setInt("dirLgtsCnt", dirLgtsCnt);
	setInt("pntLgtsCnt", pntLgtsCnt);
	setInt("sptLgtsCnt", sptLgtsCnt);
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
	loadLights();
	loadCameraAndModelMatrix(camera, modelMatrix);
}

void Shader::setSceneInfo(const SceneInfo* const sceneInfo)
{
	if (this->sceneInfo != NULL)
		delete this->sceneInfo;
	this->sceneInfo = new SceneInfo(*sceneInfo);
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
	delete[] dirLghtOffsets;
	delete[] pntLghtOffsets;
	delete[] sptLghtOffsets;
}

void LightsUBO::GenerateBuffer(const Shader& shader)
{
	glGenBuffers(1, &uboBuf);
	glBindBuffer(GL_UNIFORM_BUFFER, uboBuf);
	glBufferData(GL_UNIFORM_BUFFER, dirLightsCnt * 4 * sizeof(glm::vec3) +
		pntLightsCnt * (4 * sizeof(glm::vec3) + 3 * sizeof(float)) +
		sptLightsCnt * (5 * sizeof(glm::vec4)), NULL, GL_STATIC_DRAW);
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
}

void LightsUBO::LoadInfo(const std::vector<const LightSource*>& lights)
{
	int loadDirLghtCnt = 0;
	int loadPntLghtCnt = 0;
	int loadSptLghtCnt = 0;
	for (int i = 0; i < lights.size(); i++)
	{
		lights[i]->GetType();
	}
}