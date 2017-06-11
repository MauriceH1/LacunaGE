#include "resources\ResourceManager.h"
#include "system\EntityFactory.h"
#include "game_objects\Entity.h"
#include "game_objects\Components\MeshComponent.h"
//#include "resources\Device.h"

#include "resources\Vertex.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <glm\geometric.hpp>

#include <vector>
#include <iostream>

using namespace lcn::resources;

namespace lcn::resources
{
	struct ResourceData {
		const Device* device;
		std::vector<uint32_t> m_Meshes;
		std::vector<uint32_t> m_MeshIndexCounts;
		std::string path;
	};
}; // namespace lcn::resources

ResourceManager::ResourceManager(const char* a_Argv0)
{
	m_Data = new ResourceData;

	std::string filepath(a_Argv0);
	size_t last_sep_idx = filepath.find_last_of('\\');
	if (last_sep_idx == std::string::npos)
		last_sep_idx = filepath.find_last_of('/');
	last_sep_idx++;

	char* directory = new char[filepath.substr(0, last_sep_idx).length()];
	strcpy(directory, filepath.substr(0, last_sep_idx).c_str());
	m_Data->path = std::string(directory);
}

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Initialize(const Device* a_Device)
{
	m_Data->device = a_Device;

	return true;
}

GUID ResourceManager::CreatePipeline(const PipelineParams a_Params)
{
	return m_Data->device->CreatePipelineState(&a_Params);
}

lcn::object::Entity* ResourceManager::LoadModel(const char* a_RelPath)
{
	Assimp::Importer importer;

	std::string assetPath = m_Data->path;
	assetPath.append(a_RelPath);

	lcn::object::Entity* a_ModelRoot = nullptr;

	const aiScene* scene = importer.ReadFile(assetPath.c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode)
	{
		std::vector<uint32_t> nodeLinker;
		ImportMeshes(scene, &nodeLinker);
		a_ModelRoot = ProcessImportedNodes(scene->mRootNode, scene, &nodeLinker);
	}
#ifdef _DEBUG
	else
	{
		std::cout << "Error: Model could not get imported:\n\t" << importer.GetErrorString() << std::endl;
	}
#endif

	return a_ModelRoot;
}

GUID ResourceManager::LoadShader(const char* a_RelPath)
{
	return 0;
}

GUID ResourceManager::LoadAndCompileShader(const char* a_RelPath, const char* a_EntryPoint, EShaderTypes a_ShaderType)
{
	std::string shaderPath = m_Data->path;
	shaderPath.append(a_RelPath);
	return m_Data->device->UploadAndCompileShader(shaderPath.c_str(), a_EntryPoint, a_ShaderType);
}

void ResourceManager::ImportMeshes(const void* a_Scene, void* a_NodeLinker)
{
	std::vector<uint32_t>* nodeLinker = (std::vector<uint32_t>*)a_NodeLinker;

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
				vertexArray[v].texCoord = *( glm::vec2* )&mesh->mTextureCoords[0][v]; // TexCoords store in vertex texcoords

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

		uint32_t meshGUID = m_Data->device->UploadMesh(vertexArray, mesh->mNumVertices, indexArray.data(), (uint32_t)indexArray.size());
		m_Data->m_Meshes.push_back(meshGUID);
		m_Data->m_MeshIndexCounts.push_back(indexArray.size());
		nodeLinker->push_back(meshGUID);
	}
}

lcn::object::Entity* ResourceManager::ProcessImportedNodes(const void* a_Node, const void* a_Scene, void* a_NodeLinker, lcn::object::Entity* a_ParentEntity)
{
	const aiNode* node = (aiNode*)a_Node;
	std::vector<uint32_t>* nodeLinker = (std::vector<uint32_t>*)a_NodeLinker;

	lcn::object::Entity* entity = EntityFactory::CreateEntity();
	entity->SetLocalMatrix(*(glm::mat4x4*)&node->mTransformation);

	if(a_ParentEntity)
		a_ParentEntity->AddChild(entity);

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		lcn::object::MeshComponent* meshComp = new lcn::object::MeshComponent();
		meshComp->SetMeshGUID(nodeLinker->at(node->mMeshes[i]));
		meshComp->SetMeshIndexCount(m_Data->m_MeshIndexCounts.at(nodeLinker->at(node->mMeshes[i])));
		entity->AddComponent(meshComp);
	}

	unsigned int a = node->mNumChildren;
	for (unsigned int n = 0; n < a; n++)
	{
		ProcessImportedNodes(node->mChildren[n], a_Scene, a_NodeLinker, entity);
	}

	return entity;
}