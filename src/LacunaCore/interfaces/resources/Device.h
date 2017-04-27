#pragma once

#include <limits>
#include "Vertex.h"

namespace lcn::resources
{
	struct PipelineParams
	{
		uint32_t VertexShader = std::numeric_limits<uint32_t>::max();
		uint32_t PixelShader = std::numeric_limits<uint32_t>::max();
	};

	enum EShaderTypes {
		EShaderTypes_VS,
		EShaderTypes_PS
	};

	struct PipelineParams;
	class Device
	{
	public:
		Device() {};
		virtual ~Device() {};

		virtual const uint32_t CreatePipelineState(const PipelineParams* const a_Params) const = 0;

		virtual const uint32_t UploadShader(const char* a_AbsPath) const = 0;
		virtual const uint32_t UploadAndCompileShader(const char* a_AbsPath, const char* a_EntryPoint, EShaderTypes a_ShaderType) const = 0;

		virtual const uint32_t UploadMesh(Vertex* a_Vertices, uint32_t a_NumVertices, const uint32_t* a_Indices, uint32_t a_NumIndices) const = 0;
	};
}; // namespace lcn::resources