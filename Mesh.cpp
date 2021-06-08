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
	reflectivity = 0.25f;
	hasTransparency = false;
}

Shader* Material::GetShader()
{
	return shader;
}

float Material::GetProperty(MaterialProp property)
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

glm::vec4 Material::GetColor(MaterialType material)
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

bool Material::HasTransparency()
{
	return hasTransparency;
}

void Material::SetTransparencyStatus(bool hasTransparency)
{
	this->hasTransparency = hasTransparency;
}

//	Класс Mesh

Mesh::Mesh(const Model* root, const Mesh* parent, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
	const std::vector<Texture>& textures, const std::string& name = "mesh")
{
	this->root = root;
	this->parent = parent;
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
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
	unsigned int diffuseN = 1;
	unsigned int specularN = 1;
	unsigned int normalN = 1;
	unsigned int heightN = 1;
	shader.use();
	UpdateModelMatrix();
	glm::mat4 modelMat = root->GetModelMatrix();
	for (const Mesh* mesh = this; mesh != NULL; mesh = mesh->parent)
	{
		modelMat = modelMat * mesh->model;
	}
	shader.loadMainInfo(root->GetCamera(), &modelMat);
	shader.setBool("hasSkybox", false);
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string name = "material.";
		switch (textures[i].GetType())
		{
		case TextureType::Diffuse:
			name += "texture_diffuse" + std::to_string(diffuseN++);
			break;
		case TextureType::Specular:
			name += "texture_specular" + std::to_string(specularN++);
			break;
		case TextureType::Normal:
			name += "texture_normal" + std::to_string(normalN++);
			break;
		case TextureType::Height:
			name += "texture_height" + std::to_string(heightN++);
			break;
		case TextureType::CubeMap:
			name = "skybox";
			shader.setBool("hasSkybox", true);
			break;
		default: continue;
		}
		shader.setInt(name, i);
		glActiveTexture(GL_TEXTURE0 + i);
		if (textures[i].GetType() == TextureType::CubeMap)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i].GetId());
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, textures[i].GetId());
		}
	}
	shader.setInt("material.diffTextCount", diffuseN - 1);
	shader.setInt("material.specTextCount", specularN - 1);
	shader.setInt("material.ambTextCount", heightN - 1);
	shader.setVec("material.diffuse", material.GetColor(MaterialType::DIFFUSE));
	shader.setVec("material.ambient", material.GetColor(MaterialType::AMBIENT));
	shader.setVec("material.specular", material.GetColor(MaterialType::SPECULAR));
	shader.setFloat("material.shininess", material.GetProperty(MaterialProp::SHININESS));
	shader.setFloat("material.alpha", material.GetProperty(MaterialProp::ALPHA));
	shader.setFloat("material.reflectivity", material.GetProperty(MaterialProp::REFLECTIVITY));
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);

	diffuseN = 1;
	specularN = 1;
	normalN = 1;
	heightN = 1;
	shader.setBool("hasSkybox", false);
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string name = "material.";
		switch (textures[i].GetType())
		{
		case TextureType::Diffuse:
			name += "texture_diffuse" + std::to_string(diffuseN++);
			break;
		case TextureType::Specular:
			name += "texture_specular" + std::to_string(specularN++);
			break;
		case TextureType::Normal:
			name += "texture_normal" + std::to_string(normalN++);
			break;
		case TextureType::Height:
			name += "texture_height" + std::to_string(heightN++);
			break;
		case TextureType::CubeMap:
			name = "skybox";
			shader.setInt(name, 14);
			continue;
			break;
		default: continue;
		}
		shader.setInt(name, 15);
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

std::vector<Texture>& Mesh::GetTextures()
{
	return textures;
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
