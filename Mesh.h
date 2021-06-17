#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "Texture.h"
#include "Model.h"

class Shader;
class Model;
class Mesh;

enum class MaterialType
{
	AMBIENT, DIFFUSE, SPECULAR
};

enum class MaterialProp
{
	SHININESS, ALPHA, REFLECTIVITY
};

class Vertex
{
private:
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture;
	glm::vec3 tangent;
	glm::vec3 biTangent;
public:
	Vertex();
	Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texture, glm::vec3 tangent, glm::vec3 biTangent);
	glm::vec3 GetPosition();
	glm::vec3 GetNormal();
	glm::vec2 GetTexture();
	glm::vec3 GetTangent();
	glm::vec3 GetBitangent();
	void SetPosition(glm::vec3 position);
	void SetNormal(glm::vec3 normal);
	void SetTexture(glm::vec2 texture);
	void SetTangent(glm::vec3 tangent);
	void SetBitangent(glm::vec3 biTangent);
};

class Material
{
private:
	friend class Mesh;
	glm::vec4 ambientColor;
	glm::vec4 diffuseColor;
	glm::vec4 specularColor;
	std::vector<Texture> textures;
	float shininess;
	float alpha;
	float reflectivity;
	bool hasTransparency;
	Shader* shader = NULL;
public:
	Material();
	Shader* GetShader();
	float GetProperty(MaterialProp property) const;
	glm::vec4 GetColor(MaterialType material) const;
	const std::vector<Texture>* GetTextures() const;
	bool HasTransparency();
	void SetShader(Shader* shader);
	void SetColor(MaterialType material, glm::vec4 color);
	void SetProperty(MaterialProp property, float value);
	void SetTransparencyStatus(bool hasTransparency);
	void AddTexture(const Texture& texture);
};

class Mesh
{
private:
	friend class Model;
	unsigned int VAO, VBO, EBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material material;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 _startRotation;
	glm::vec3 scale;
	glm::mat4 model;
	const Mesh* parent;
	const Model* root;
	void Draw(const Shader& shader);
	void Draw();
public:
	std::string name;
	Mesh(const Model* root, const Mesh* parent, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
		const std::vector<Texture>* textures, const std::string& name);
	std::vector<Vertex>& GetVertices();
	std::vector<unsigned int>& GetIndices();
	Shader* GetShader();
	Material* GetMaterial();
	const glm::mat4& GetModelMatrix() const;
	void SetPosition(glm::vec3 position);
	void SetRotation(glm::vec3 rotation);
	void SetScale(glm::vec3 scale);
	void SetShader(Shader* shader);
	void UpdateModelMatrix();
};