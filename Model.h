#pragma once
#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <filesystem>
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"

class Mesh;

class Model
{
private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::string modelPath;
	glm::vec3 origOrientation;
	glm::vec3 scale;
	glm::vec3 scaleMult;
	glm::vec3 rotation;
	glm::vec3 position;
	glm::vec3 worldPos;
	glm::mat4 model;
	Camera* camera = NULL;
	Model();
	void LoadModel();
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType aiType, TextureType type);
	void AddMesh(Mesh mesh);
public:
	Model(const std::string& directory, const std::string& modelPath, Camera* camera,
		glm::vec3 origOrientation = glm::vec3(1.0f, 0.0f, 0.0f));
	void Draw(const Shader& shader);
	void Draw();
	void UpdateModelMatrix();
	Mesh* GetMesh(int index);
	Mesh* GetMesh(const std::string& name);
	std::vector<Mesh>* GetMeshes();
	glm::vec3 GetOrigOrientation() const;
	const glm::mat4& GetModelMatrix() const;
	Camera* GetCamera() const;
	void SetOrigOrientation(glm::vec3 orientation);
	void SetScale(glm::vec3 scale);
	void SetRotation(glm::vec3 rotation);
	void SetLocalPosition(glm::vec3 position);
	void SetWorldPosition(glm::vec3 position);
	void SetCamera(Camera* camera);
	void SetGlobalShader(Shader* shader);
	void SetScaleMultiplicator(glm::vec3 multiplicator);
	static Model* CreatePlane(float width, float length, const std::string& name,
		std::vector<Texture>* textures = NULL, bool loadTextures = false);
	static Model* CreateCube(glm::vec3 scale, const std::string& name,
		std::vector<Texture>* texturesToLoad = NULL, bool loadTextures = false);
	static Model* CreateSkybox(const std::vector<std::string>& textures);
};

