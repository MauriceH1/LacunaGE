#pragma once

#include <cstdint>

namespace lcn::resources
{
	enum EShaderType {
		EShaderType_NONE,
		EShaderType_VS,
		EShaderType_PS,
		EShaderType_GS,
		EShaderType_HS/*,
		EShaderType_CS*/
	};
	class Shader
	{
	public:
		Shader(char* a_ShaderPath, char* a_EntryPoint, EShaderType a_Type, bool a_Compile = false);
		virtual ~Shader() = 0;

	private:
		uint32_t m_Guid;
	};
}; // namespace lcn::resources