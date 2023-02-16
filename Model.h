#pragma once

#include <vector>
#include <string>

#pragma warning(push, 0)
#include <assimp\Importer.hpp>
// holds the scene data
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#pragma warning(pop)

#include "Mesh.h"
#include "Texture.h"

// or a group of meshes, each model has multiple nodes
// which will have the same functions 
class Model
{
public:
	Model();
	Model(const char* name);

	// take the filename of the model which we load
	void loadModel(const std::string& fileName);
	void renderModel();
	void clearModel();
	std::vector<Mesh*> getMeshes();
	~Model();

private:

	void loadNode(aiNode* node, const aiScene* scene);
	void loadMesh(aiMesh* mesh, const aiScene* scene);
	void loadMaterials(const aiScene* scene);

	// nodes as children
	std::vector<Mesh*> meshList;
	std::vector<Texture*> textureList;
	// create one of each texture and map them to the 
	// meshes we actually have 
	std::vector<unsigned int> meshToTex;
	char* name;
};

