#pragma once

#define NOMINMAX

#include <wrl.h>

#include "../source/d3dx12.h"

#include "resources/Device.h"
#include "resources/Vertex.h"

namespace lcn::resources
{
	struct DeviceData;
	class DX12Device : public Device
	{
	public:
		DX12Device();
		virtual ~DX12Device() override;

		bool Initialize(Microsoft::WRL::ComPtr<ID3D12Device> a_Device);

		virtual const uint32_t CreatePipelineState(const PipelineParams* const a_Params) const override;
		
		virtual const uint32_t UploadShader(const char* a_AbsPath) const override;
		virtual const uint32_t UploadAndCompileShader(const char* a_AbsPath, const char* a_EntryPoint, EShaderTypes a_ShaderType) const override;

		virtual const uint32_t UploadMesh(Vertex* a_Vertices, uint32_t a_NumVertices, const uint32_t* a_Indices, uint32_t a_NumIndices) const override;
	
		virtual const uint32_t UploadTexture(uint8_t* a_ImageData, uint32_t a_Width, uint32_t a_Height, uint32_t a_BPP) const override;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState(uint32_t a_GUID) const;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const;

		D3D12_VERTEX_BUFFER_VIEW GetVertexBuffer(uint32_t a_GUID) const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBuffer(uint32_t a_GUID) const;
	private:
		DeviceData* m_Data;

		uint32_t CreateNormalPipelineState(const PipelineParams* const a_Params) const;
		uint32_t CreateRootSignature() const;
	};
}; // lcn::resources