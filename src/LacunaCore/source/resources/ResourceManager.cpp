#include "resources\ResourceManager.h"
#include "resources\Device.h"

#include "resources\Vertex.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <glm\geometric.hpp>

#include <vector>
#include <iostream>

using namespace lcn::resources;

namespace lcn { namespace resources
{
	struct ResourceData {
		const Device* device;
		std::vector<uint32_t> m_Meshes;
	};
};}; // namespace lcn::resources

ResourceManager::ResourceManager()
{}

ResourceManager::~ResourceManager()
{}

bool ResourceManager::Initialize(const Device* a_Device)
{
	m_Data = new ResourceData;

	m_Data->device = a_Device;

	return true;
}

GUID ResourceManager::LoadModel(const char* a_RelPath)
{
	Vertex cubeVerts[] = {
		{glm::vec3(-1.f,-1.f,-1.f),glm::normalize(glm::vec3(-1.f,-1.f,-1.f))},
		{glm::vec3( 1.f,-1.f,-1.f),glm::normalize(glm::vec3( 1.f,-1.f,-1.f))},
		{glm::vec3(-1.f, 1.f,-1.f),glm::normalize(glm::vec3(-1.f, 1.f,-1.f))},
		{glm::vec3( 1.f, 1.f,-1.f),glm::normalize(glm::vec3( 1.f, 1.f,-1.f))},
		{glm::vec3(-1.f,-1.f, 1.f),glm::normalize(glm::vec3(-1.f,-1.f, 1.f))},
		{glm::vec3( 1.f,-1.f, 1.f),glm::normalize(glm::vec3( 1.f,-1.f, 1.f))},
		{glm::vec3(-1.f, 1.f, 1.f),glm::normalize(glm::vec3(-1.f, 1.f, 1.f))},
		{glm::vec3( 1.f, 1.f, 1.f),glm::normalize(glm::vec3( 1.f, 1.f, 1.f))}
	};

	uint32_t cubeIndices[] = {
		0, 1, 2,
		1, 3, 2,
		4, 6, 5,
		6, 7, 5
	};

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(a_RelPath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode)
	{

		// m_Data->device->UploadMesh(cubeVerts, 8, cubeIndices, 6);
		ImportMeshes(scene);
		ProcessImportedNodes(scene->mRootNode, scene);
	}
#ifdef _DEBUG
	else
	{
		std::cout << "Error: Model could not get imported:\n\t" << importer.GetErrorString() << std::endl;
	}
#endif

	return 0;
}

void ResourceManager::ImportMeshes(const void *a_Scene)
{
	const aiScene* scene = (aiScene*)a_Scene;
	for (unsigned int m = 0; m < scene->mNumMeshes; m++)
	{
		const aiMesh *mesh = scene->mMeshes[m];

		Vertex* vertexArray = new Vertex[mesh->mNumVertices];
		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			vertexArray[v].position = *(glm::vec3*)&mesh->mVertices[v]; // Vertices store in vertex position
			vertexArray[v].normal = *(glm::vec3*)&mesh->mNormals[v]; // Normals store in vertex normal
			if (mesh->HasTangentsAndBitangents())
			{
				vertexArray[v].tangent = *(glm::vec3*)&mesh->mTangents[0][v]; // Tangents store in vertex tangent
				vertexArray[v].biTangent = *(glm::vec3*)&mesh->mBitangents[0][v]; // biTangents store in vertex biTangent
			}
			if (mesh->HasTextureCoords(0))
			{
				vertexArray[v].texCoord = *( glm::vec2* )&mesh->mTextureCoords[0][v]; // TexCoords store in vertex texcoord

				switch ( mesh->mNumUVComponents[0] )
				{
				case 1:
					vertexArray[v].texCoord = glm::vec2( mesh->mTextureCoords[0][v].x, 0.0f );
					break;
				case 2:
					vertexArray[v].texCoord = glm::vec2( mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y );
				default:
					break;
				}
			}
		}

		std::vector<uint32_t> indexArray;
		for (unsigned int f = 0; f < mesh->mNumFaces; f++)
		{
			const aiFace face = mesh->mFaces[f];
			for (unsigned int i = 0; i < face.mNumIndices; i++)
			{
				indexArray.push_back(face.mIndices[i]); // Store in index array
			}
		}

		m_Data->m_Meshes.push_back(m_Data->device->UploadMesh(vertexArray, mesh->mNumVertices, indexArray.data(), indexArray.size()));
	}
}

void ResourceManager::ProcessImportedNodes(const void *a_Node, const void *a_Scene)
{
	const aiNode* node = (aiNode*)a_Node;
	unsigned int a = node->mNumChildren;
	for (unsigned int n = 0; n < a; n++)
	{
		ProcessImportedNodes(node->mChildren[n], a_Scene);
	}
}