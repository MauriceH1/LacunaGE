#include "DX12Shader.h"
#include "../DX12Helpers.h"

#include <string>

namespace lcn::resources
{
	struct DX12ShaderData
	{
		Microsoft::WRL::ComPtr<ID3DBlob> ShaderBlob;
	};
};

using namespace lcn::resources;

DX12Shader::DX12Shader(char* a_ShaderPath, char* a_EntryPoint, EShaderType a_Type, bool a_Compile)
	: Shader(a_ShaderPath, a_EntryPoint, a_Type, a_Compile)
{
	wchar_t path[1024];
	GetModuleFileNameW(NULL, path, sizeof(path));
	
	if (a_Compile)
	{
#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif
		std::string shader_target;
		switch (a_Type)
		{
			case EShaderType::EShaderType_VS:
			{
				shader_target = "vs_5_0";
			}; break;
			
			case EShaderType::EShaderType_PS:
			{
				shader_target = "ps_5_0";
			}; break;
			
			case EShaderType::EShaderType_GS:
			{
				shader_target = "gs_5_0";
			}; break;
			
			case EShaderType::EShaderType_HS:
			{
				shader_target = "hs_5_0";
			}; break;
			
			default:
			{
			}; return;
		};

		ThrowIfFailed(D3DCompileFromFile(path, nullptr, nullptr, a_EntryPoint, shader_target.data(), compileFlags, 0, &m_Data->ShaderBlob, nullptr));
	}
	else
	{
		ThrowIfFailed(D3DReadFileToBlob(path, &m_Data->ShaderBlob));
	}
	delete[] path;
}

DX12Shader::~DX12Shader()
{
	m_Data->ShaderBlob.ReleaseAndGetAddressOf();
	delete m_Data;
}