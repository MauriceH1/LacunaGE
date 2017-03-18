#pragma once

#include "Vertex.h"

namespace lcn { namespace resources
{
	class Device
	{
	public:
		Device() {};
		virtual ~Device() {};

		virtual const uint32_t UploadMesh(Vertex* a_Vertices, uint32_t a_NumVertices, const uint32_t* a_Indices, uint32_t a_NumIndices) const = 0;
	};
};}; // namespace lcn::resources