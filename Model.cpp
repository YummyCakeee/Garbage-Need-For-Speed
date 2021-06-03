#include "Model.h"

Model::Model(const std::string& directory, const std::string& modelPath, Camera* camera,
	glm::vec3 origOrientation)
{
	this->directory = directory;
	this->modelPath = modelPath;
	this->camera = camera;
	this->origOrientation = origOrientation;
	scale = glm::vec3(1.0f);
	scaleMult = glm::vec3(1.0f);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	position = glm::vec3(0.0f);
	worldPos = glm::vec3(0.0f);
	UpdateModelMatrix();
	LoadModel();
}

Model::Model()
{
	directory = "";
	modelPath = "";
	origOrientation = glm::vec3(1.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f);
	scaleMult = glm::vec3(1.0f);
	rotation = glm::vec3(1.0f, 0.0f, 0.0f);
	position = glm::vec3(0.0f);
	worldPos = glm::vec3(0.0f);
	UpdateModelMatrix();
}


void Model::Draw(const Shader& shader)
{
	UpdateModelMatrix();
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

void Model::Draw()
{
	UpdateModelMatrix();
	for (int i = 0; i < meshes.size(); i++)
	{
		Shader* shader = meshes[i].GetShader();
		if (shader != NULL)
		{
			meshes[i].Draw();
		}
		else
		{
			std::cout << "ERROR::SHADER:: Shader is not set to mesh: " << meshes[i].name << std::endl;
		}
	}
}

void Model::UpdateModelMatrix()
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position + worldPos);
	glm::vec3 crossRes = glm::cross(origOrientation, rotation);

	if (crossRes != glm::vec3(0.0f))
	{
		float angle = glm::acos(glm::dot(origOrientation, rotation));
		glm::quat direction = glm::angleAxis(angle,
			glm::normalize(crossRes));
		model = model * glm::mat4_cast(direction);
	}
	model = glm::scale(model, scale * scaleMult);
	this->model = model;
}

void Model::LoadModel()
{
	Assimp::Importer import;
	std::string path = directory + "\\" + modelPath;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals
		| aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
		!scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	ProcessNode(scene->mRootNode, scene);
	import.FreeScene();
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vertexField;
		if (mesh->HasPositions())
		{
			vertexField.x = mesh->mVertices[i].x;
			vertexField.y = mesh->mVertices[i].y;
			vertexField.z = mesh->mVertices[i].z;
			vertex.SetPosition(vertexField);
		}
		if (mesh->HasNormals())
		{
			vertexField.x = mesh->mNormals[i].x;
			vertexField.y = mesh->mNormals[i].y;
			vertexField.z = mesh->mNormals[i].z;
			vertex.SetNormal(vertexField);
		}
		if (mesh->HasTextureCoords(0))
		{
			vertexField.x = mesh->mTextureCoords[0][i].x;
			vertexField.y = mesh->mTextureCoords[0][i].y;
			vertex.SetTexture(glm::vec2(vertexField.x, vertexField.y));

			vertexField.x = mesh->mTangents[i].x;
			vertexField.y = mesh->mTangents[i].y;
			vertexField.z = mesh->mTangents[i].z;
			vertex.SetTangent(vertexField);

			vertexField.x = mesh->mBitangents[i].x;
			vertexField.y = mesh->mBitangents[i].y;
			vertexField.z = mesh->mBitangents[i].z;
			vertex.SetBitangent(vertexField);
		}
		else vertex.SetTexture(glm::vec2(0.0f, 0.0f));

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::Diffuse);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::Specular);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::Normal);
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, TextureType::Height);
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	Mesh resultMesh = Mesh(this, NULL, vertices, indices, textures, mesh->mName.C_Str());
	Material* resMeshMat = resultMesh.GetMaterial();
	for (auto it = diffuseMaps.begin(); it != diffuseMaps.end(); it++)
	{
		if (it->GetChannelsCount() >= 4) resMeshMat->SetTransparencyStatus(true);
	}
	aiColor4D color;
	aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &color);
	resMeshMat->SetColor(MaterialType::AMBIENT, glm::vec4(color.r, color.g, color.b, color.a));
	if (color.a < 1.0) resMeshMat->SetTransparencyStatus(true);
	aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color);
	resMeshMat->SetColor(MaterialType::DIFFUSE, glm::vec4(color.r, color.g, color.b, color.a));
	aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color);
	resMeshMat->SetColor(MaterialType::SPECULAR, glm::vec4(color.r, color.g, color.b, color.a));
	float shininess;
	aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, NULL);
	if (shininess > 0.0f)
		resMeshMat->SetProperty(MaterialProp::SHININESS, shininess);
	float alpha;
	aiGetMaterialFloatArray(material, AI_MATKEY_OPACITY, &alpha, NULL);
	resMeshMat->SetProperty(MaterialProp::ALPHA, alpha);
	if (alpha < 1.0f) resMeshMat->SetTransparencyStatus(true);
	return resultMesh;
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType aiType, TextureType type)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(aiType); i++)
	{
		aiString path;
		material->GetTexture(aiType, i, &path);
		bool loaded = false;
		for (unsigned int j = 0; j < meshes.size(); j++)
		{
			std::vector<Texture> meshTextures = meshes[j].GetTextures();
			for (int k = 0; k < meshTextures.size(); k++)
			{
				if (std::strcmp(meshTextures[k].GetPath().data(), (directory + "\\" + path.C_Str()).c_str()) == 0)
				{
					textures.push_back(meshTextures[k]);
					loaded = true;
					break;
				}
			}
		}
		if (!loaded)
		{
			Texture texture = Texture::LoadTexture(std::string(directory + "\\" + path.C_Str()));
			texture.SetType(type);
			textures.push_back(texture);
		}
	}
	return textures;
}

Mesh* Model::GetMesh(int index)
{
	if (index >= meshes.size())
		return nullptr;
	return &meshes[index];
}

Mesh* Model::GetMesh(const std::string& name)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		if (meshes[i].name == name) return &meshes[i];
	}
	return nullptr;
}

std::vector<Mesh>* Model::GetMeshes()
{
	return &meshes;
}

glm::vec3 Model::GetOrigOrientation() const
{
	return origOrientation;
}

const glm::mat4& Model::GetModelMatrix() const
{
	return model;
}

Camera* Model::GetCamera() const
{
	return camera;
}

void Model::SetOrigOrientation(glm::vec3 orientation)
{
	if (orientation != glm::vec3(0.0f))
		origOrientation = glm::normalize(orientation);
}

void Model::SetGlobalShader(Shader* shader)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].SetShader(shader);
	}
}

void Model::SetScaleMultiplicator(glm::vec3 multiplicator)
{
	if (multiplicator.x > 0 && multiplicator.y > 0 && multiplicator.z > 0)
		scaleMult = multiplicator;
	else scaleMult = glm::vec3(1.0f);
}

void Model::SetScale(glm::vec3 scale)
{
	if (scale.x >= 0.0f && scale.y >= 0.0f && scale.z >= 0.0f)
		this->scale = scale;
	else this->scale = glm::vec3(1.0f);
}

void Model::SetRotation(glm::vec3 rotation)
{
	if (glm::length(rotation) != 0.0f)
		this->rotation = glm::normalize(rotation);
	else this->rotation = glm::vec3(1.0f, 0.0f, 0.0f);
}

void Model::SetLocalPosition(glm::vec3 position)
{
	this->position = position;
}

void Model::SetWorldPosition(glm::vec3 position)
{
	this->worldPos = position;
}

void Model::SetCamera(Camera* camera)
{
	this->camera = camera;
}

void Model::AddMesh(Mesh mesh)
{
	meshes.push_back(mesh);
}

Model* Model::CreatePlane(float width, float length, const std::string& name, std::vector<Texture>* textures, bool loadTextures)
{
	Model* plane = new Model();
	plane->SetOrigOrientation(glm::vec3(1.0f, 0.0f, 0.0f));
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> modelTextures;
	Vertex vertex;
	//	1 вершина
	vertex.SetPosition(glm::vec3(-width / 2.0f, 0.0f, -length / 2.0f));
	vertex.SetNormal(glm::vec3(0.0f, 1.0f, 0.0f));
	vertex.SetTexture(glm::vec2(0.0f, 0.0f));
	vertex.SetTangent(glm::vec3(0.0f));
	vertex.SetBitangent(glm::vec3(0.0f));
	vertices.push_back(vertex);
	//	2 вершина
	vertex.SetPosition(glm::vec3(-width / 2.0f, 0.0f, length / 2.0f));
	vertex.SetTexture(glm::vec2(0.0f, 1.0f));
	vertices.push_back(vertex);
	//	3 вершина
	vertex.SetPosition(glm::vec3(width / 2.0f, 0.0f, length / 2.0f));
	vertex.SetTexture(glm::vec2(1.0f, 1.0f));
	vertices.push_back(vertex);
	//	4 вершина
	vertex.SetPosition(glm::vec3(width / 2.0f, 0.0f, -length / 2.0f));
	vertex.SetTexture(glm::vec2(1.0f, 0.0f));
	vertices.push_back(vertex);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);

	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	if (textures != NULL)
	{
		for (int i = 0; i < textures[0].size(); i++)
		{
			Texture texture;
			if (loadTextures)
			{
				texture = Texture::LoadTexture(textures[0][i].GetPath());
				texture.SetType(textures[0][i].GetType());
			}
			else
				texture = Texture(textures[0][i].GetId(), textures[0][i].GetType(), textures[0][i].GetPath());
			modelTextures.push_back(texture);
		}
	}
	Mesh mesh = Mesh(plane, NULL, vertices, indices, modelTextures, name);
	plane->AddMesh(mesh);
	return plane;
}

Model* Model::CreateCube(glm::vec3 scale, const std::string& name, std::vector<Texture>* textures, bool loadTextures)
{
	Model* cube = new Model();
	const int verts = 36;
	const int posCoords = 3;
	const int normalCoords = 3;
	const int texCoords = 2;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> modelTextures;

	float polygon[verts * (posCoords + normalCoords + texCoords)] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,

	};

	for (int i = 0; i < verts; i++)
	{
		Vertex vertex;
		float* vInfo = &polygon[i * (posCoords + normalCoords + texCoords)];
		vertex.SetPosition(glm::vec3(vInfo[0], vInfo[1], vInfo[2]));
		vertex.SetNormal(glm::vec3(vInfo[3], vInfo[4], vInfo[5]));
		vertex.SetTexture(glm::vec2(vInfo[6], vInfo[7]));
		vertices.push_back(vertex);
	}
	const int faces = 6;
	const int faceParts = 2;
	const int points = 3;

	for (int i = 0; i < faces * faceParts * points; i++)
	{
		indices.push_back(i);
	}

	if (textures != NULL)
	{
		for (int i = 0; i < textures[0].size(); i++)
		{
			Texture texture;
			if (loadTextures)
			{
				texture = Texture::LoadTexture(textures[0][i].GetPath());
				texture.SetType(textures[0][i].GetType());
			}
			else
				texture = Texture(textures[0][i].GetId(), textures[0][i].GetType(), textures[0][i].GetPath());
			modelTextures.push_back(texture);
		}
	}
	Mesh mesh = Mesh(cube, NULL, vertices, indices, modelTextures, name);
	cube->AddMesh(mesh);
	cube->SetScale(scale);
	return cube;
}

Model* Model::CreateSkybox(const std::vector<std::string>& textures)
{
	Model* skybox = new Model();
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> modelTextures;
	const int verts = 36;
	const int posCoords = 3;
	float skyboxVertices[verts * posCoords] = {

		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	for (int i = 0; i < verts; i++)
	{
		Vertex vertex;
		float* vInfo = &skyboxVertices[i * posCoords];
		vertex.SetPosition(glm::vec3(vInfo[0], vInfo[1], vInfo[2]));
		vertices.push_back(vertex);
	}

	const int faces = 6;
	const int faceParts = 2;
	const int points = 3;
	for (int i = 0; i < faces * faceParts * points; i++)
	{
		indices.push_back(i);
	}
	unsigned int id = Texture::LoadCubeMap(textures);
	modelTextures.push_back(Texture(id, TextureType::CubeMap, textures[0].c_str()));
	Mesh mesh = Mesh(skybox, NULL, vertices, indices, modelTextures, "skybox");
	skybox->AddMesh(mesh);

	return skybox;
}