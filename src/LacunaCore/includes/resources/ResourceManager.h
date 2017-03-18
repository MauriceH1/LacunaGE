#pragma once

#include <cstdint>
#include "resources\Device.h"

namespace lcn { namespace resources
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
		ResourceManager();
		~ResourceManager();

		bool Initialize(const Device* a_Device);

		GUID LoadModel(const char* a_RelPath);

	private:
		ResourceData* m_Data;

		void ImportMeshes(const void *a_Scene);
		void ProcessImportedNodes(const void *a_Node, const void *a_Scene);
	};
};}; // lcn::resources