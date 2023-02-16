#include "Model.h"

Model::Model()
{
	name = (char*)"";
}

//make sure the names match
Model::Model(const char* name)
{
	this->name = (char*)name;
}

void Model::renderModel()
{
	// iterate thru all the meshes and use the corresponding
	// material index 
	for (size_t i = 0; i < meshList.size(); i++)
	{
		// will be the same index
		unsigned int materialIndex = meshToTex[i];
		// check if the number is inside our range and check if there is an actual texture
		if (materialIndex < textureList.size() && textureList[materialIndex])
		{
			textureList[materialIndex]->useTexture();
		}

		meshList[i]->renderMesh();
	}
}

void Model::loadModel(const std::string& fileName)
{
	Assimp::Importer my_importer;
	// load into the scene with the flags            model defined with triangles                    smooth normals generation (like in main)      (self explanatory)
	const aiScene* scene = my_importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs    | aiProcess_GenSmoothNormals    | aiProcess_JoinIdenticalVertices);

	// check if scene was succesfully imported
	if (!scene)
	{
		#pragma warning( suppress : 6284)
		printf("Model (%s) failed to load: %s", fileName,my_importer.GetErrorString());
		return;
	}
	// get the first node of the scene and load materials
	loadNode(scene->mRootNode, scene);
	loadMaterials(scene);
}

void Model::loadNode(aiNode* node, const aiScene* scene)
{
	// for each of the meshes, load the mesh
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		// mesh is stored in scene but node->mMeshes[i] stores the id of it
		loadMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		loadNode(node->mChildren[i], scene);
	}
}

void Model::loadMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;

	// loop for each vertex in the mesh
	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		// rebuilding the array from the main function
		// insert the xyz coordinates for the vert
		vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
		// check for texture coordinates and add them if they exist
		if (mesh->mTextureCoords[0]) // can have multiple textures, check if the first one exists
		{
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
		}
		else {
			vertices.insert(vertices.end(), { 0.0f, 0.0f });
		}
		// normals will always exist because of the flag
		// because of the normalize(direction) in the shader, we import the normals 
		// with a negative sign. Could change shader if need be and import with positive
		// sign
		vertices.insert(vertices.end(), { -mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z });
	}

	// go through the faces to add the indices
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// add the indices to our indices vector
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	Mesh* newMesh = new Mesh();
	// vectors are basically contiguous arrays in memory, can treat them as one
	// by taking the address of the first element
	newMesh->createMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
	meshList.push_back(newMesh);
	// add the material index
	meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::loadMaterials(const aiScene* scene)
{
	// run through all the materials, using the same index as the scene
	// and use them accordingly
	// We have to resize it because it has other things like specular
	// and stuff we don't need
	textureList.resize(scene->mNumMaterials);

	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];

		// we will place the required texture at the same index
		// the material was found
		textureList[i] = nullptr;
		// get the number of textures, can be many types, ours are diffuse
		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				// erase everything up until the very last backslash
				// to mitigate erroneus absolute paths
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				// our path is in the textures folder
				std::string texPath = std::string("Textures/") + name + "/" + filename; 

				textureList[i] = new Texture(texPath.c_str());

				// check if the texture has loaded correctly
				if (!textureList[i]->loadTextureA())
				{
					printf("Failed to load texture at: %s\n", texPath.c_str());
					delete textureList[i];
					textureList[i] = nullptr;
				}
			}
		}

		if (!textureList[i])
		{
			// if there is no texture, use the plain one
			textureList[i] = new Texture("Textures/plain.png");
			textureList[i]->loadTextureA();
		}
	}
}

void Model::clearModel()
{
	// go through all the meshes and delete it
	// in a more complicated project, one would check
	// that there are no more references to that mesh/texture
	for (size_t i = 0; i < meshList.size(); i++)
	{
		if (meshList[i])
		{
			delete meshList[i];
			meshList[i] = nullptr;
		}
	}

	for (size_t i = 0; i < textureList.size(); i++)
	{
		if (textureList[i])
		{
			delete textureList[i];
			textureList[i] = nullptr;
		}
	}
}

std::vector<Mesh*> Model::getMeshes()
{
	return this->meshList;
}

Model::~Model()
{
}

