#include "Mesh.h"

//	Класс Vertex

Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texture,
	glm::vec3 tangent = glm::vec3(0.0f), glm::vec3 biTangent = glm::vec3(0.0f))
{
	this->position = position;
	this->normal = normal;
	this->texture = texture;
	this->tangent = tangent;
	this->biTangent = biTangent;
}

Vertex::Vertex()
{
	position = glm::vec3(0.0f);
	normal = glm::vec3(0.0f);
	texture = glm::vec2(0.0f);
	tangent = glm::vec3(0.0f);
	biTangent = glm::vec3(0.0f);
}

glm::vec3 Vertex::GetPosition()
{
	return position;
}

glm::vec3 Vertex::GetNormal()
{
	return normal;
}

glm::vec2 Vertex::GetTexture()
{
	return texture;
}

glm::vec3 Vertex::GetTangent()
{
	return tangent;
}
glm::vec3 Vertex::GetBitangent()
{
	return biTangent;
}

void Vertex::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void Vertex::SetNormal(glm::vec3 normal)
{
	this->normal = normal;
}

void Vertex::SetTexture(glm::vec2 texture)
{
	this->texture = texture;
}

void Vertex::SetTangent(glm::vec3 tangent)
{
	this->tangent = tangent;
}
void Vertex::SetBitangent(glm::vec3 biTangent)
{
	this->biTangent = biTangent;
}

//	Класс Material

Material::Material()
{
	ambientColor = glm::vec4(1.0f);
	diffuseColor = glm::vec4(1.0f);
	specularColor = glm::vec4(1.0f);
	shininess = 32.0f;
	alpha = 1.0f;
	reflectivity = 0.15f;
	hasTransparency = false;
}

Shader* Material::GetShader()
{
	return shader;
}

float Material::GetProperty(MaterialProp property) const
{
	switch (property)
	{
	case MaterialProp::SHININESS:
		return shininess;
		break;
	case MaterialProp::ALPHA:
		return alpha;
		break;
	case MaterialProp::REFLECTIVITY:
		return reflectivity;
		break;
	default:
		return 0.0f;
		break;
	}
}

glm::vec4 Material::GetColor(MaterialType material) const
{
	switch (material)
	{
	case MaterialType::AMBIENT:
		return ambientColor;
		break;
	case MaterialType::DIFFUSE:
		return diffuseColor;
		break;
	case MaterialType::SPECULAR:
		return specularColor;
		break;
	default:
		return glm::vec4(0.0f);
		break;
	}
}

bool Material::HasTransparency()
{
	return hasTransparency;
}

void Material::SetShader(Shader* shader)
{
	this->shader = shader;
}

void Material::SetColor(MaterialType material, glm::vec4 color)
{
	switch (material)
	{
	case MaterialType::AMBIENT:
		ambientColor = color;
		break;
	case MaterialType::DIFFUSE:
		diffuseColor = color;
		break;
	case MaterialType::SPECULAR:
		specularColor = color;
		break;
	default:
		break;
	}
}

void Material::SetProperty(MaterialProp property, float value)
{
	switch (property)
	{
	case MaterialProp::SHININESS:
		shininess = glm::clamp(value, 0.1f, 1024.0f);
		break;
	case MaterialProp::ALPHA:
		alpha = glm::clamp(value, 0.0f, 1.0f);
		if (alpha < 1.0f) hasTransparency = true;
		break;
	case MaterialProp::REFLECTIVITY :
		reflectivity = glm::clamp(value, 0.0f, 1.0f);
		break;
	default:
		break;
	}
}

void Material::SetTransparencyStatus(bool hasTransparency)
{
	this->hasTransparency = hasTransparency;
}

void Material::AddTexture(const Texture& texture)
{
	textures.push_back(texture);
}

//	Класс Mesh

Mesh::Mesh(const Model* root, const Mesh* parent, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
	const std::vector<Texture>* textures, const std::string& name = "mesh")
{
	this->root = root;
	this->parent = parent;
	this->vertices = vertices;
	this->indices = indices;
	if (textures != NULL)
		this->material.textures = *textures;
	this->name = name;
	position = glm::vec3(0.0f);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	_startRotation = rotation;
	scale = glm::vec3(1.0f);
	model = glm::mat4(1.0f);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glBindVertexArray(0);
}
void Mesh::Draw(const Shader& shader)
{
	shader.use();
	//	Обновление модельной матрицы меша
	UpdateModelMatrix();
	glm::mat4 modelMat = root->GetModelMatrix();
	for (const Mesh* mesh = this; mesh != NULL; mesh = mesh->parent)
	{
		modelMat = modelMat * mesh->model;
	}
	//	Установка свойств для шейдера
	switch (shader.GetType())
	{
	case ShaderType::MATERIAL:
	{
		const MaterialShader* matShader = (const MaterialShader*)(&shader);
		matShader->loadMainInfo(root->GetCamera(), &modelMat, &material);
		matShader->draw(VAO, indices.size());
		matShader->clearSamplers();
	}; break;
	case ShaderType::SHADOW_MAP:
	{
		const ShadowMapShader* shdMapShader = (const ShadowMapShader*)(&shader);
		shdMapShader->loadMainInfo(NULL, &modelMat);
		shdMapShader->draw(VAO, indices.size());
	}; break;
	default: break;
	}
}

void Mesh::Draw()
{
	if (material.GetShader() != NULL)
	{
		Draw(*(material.GetShader()));
	}
	else
	{
		std::cout << "ERROR::SHADER:: Shader is not set to mesh: " << name << std::endl;
	}
}

std::vector<Vertex>& Mesh::GetVertices()
{
	return vertices;
}

std::vector<unsigned int>& Mesh::GetIndices()
{
	return indices;
}

const std::vector<Texture>* Material::GetTextures() const
{
	return &textures;
}

Shader* Mesh::GetShader()
{
	return material.GetShader();
}

Material* Mesh::GetMaterial()
{
	return &material;
}

const glm::mat4& Mesh::GetModelMatrix() const
{
	return model;
}

void Mesh::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void Mesh::SetRotation(glm::vec3 rotation)
{
	if (glm::length(rotation) != 0.0f)
	{
		this->rotation = glm::normalize(rotation);
	}
	else this->rotation = glm::vec3(1.0f, 0.0, 0.0f);
}

void Mesh::SetScale(glm::vec3 scale)
{
	if (scale.x >= 0 && scale.y >= 0 && scale.z >= 0)
		this->scale = scale;
	else this->scale = glm::vec3(1.0f);
}

void Mesh::SetShader(Shader* shader)
{
	material.SetShader(shader);
}

void Mesh::UpdateModelMatrix()
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	glm::vec3 crossRes = glm::cross(_startRotation, rotation);

	if (crossRes != glm::vec3(0.0f))
	{
		float angle = glm::acos(glm::dot(_startRotation, rotation));
		glm::quat direction = glm::angleAxis(angle,
			glm::normalize(crossRes));
		model = model * glm::mat4_cast(direction);
	}
	model = glm::scale(model, scale);
	this->model = model;
}
