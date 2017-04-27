#pragma once

#include "resources\Shader.h"

namespace lcn::resources
{
	struct DX12ShaderData;
	class DX12Shader : public Shader
	{
	public:
		DX12Shader(char* a_ShaderPath, char* a_EntryPoint, EShaderType a_Type, bool a_Compile = false);
		~DX12Shader();
	private:
		DX12ShaderData* m_Data;
	};
}; // namespace lcn::resources