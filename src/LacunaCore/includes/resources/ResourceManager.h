#pragma once

#include <cstdint>
#include "resources\Device.h"

namespace lcn::object
{
	class Entity;
}; // namespace lcn::object

namespace lcn::resources
{
	struct GUID
	{
	public:
		GUID(uint32_t a_ID) { ID = a_ID; }
		uint32_t ID;
	};

	struct ResourceData;
	class ResourceManager
	{
	public:
		ResourceManager(const char* a_Argv0);
		~ResourceManager();

		bool Initialize(const Device* a_Device);
		GUID CreatePipeline(const PipelineParams a_Params);
		lcn::object::Entity* LoadModel(const char* a_RelPath);

		GUID LoadShader(const char* a_RelPath);
		GUID LoadAndCompileShader(const char* a_RelPath, const char* a_EntryPoint, EShaderTypes a_ShaderType);

	private:
		ResourceData* m_Data;

		void ImportMeshes(const void *a_Scene, void* a_NodeLinker);
		lcn::object::Entity* ProcessImportedNodes(const void *a_Node, const void *a_Scene, void* a_NodeLinker, lcn::object::Entity* a_ParentEntity = nullptr);
	};
}; // namespace lcn::resources