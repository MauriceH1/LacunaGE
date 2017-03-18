#pragma once

#include <wrl.h>

#include "../source/d3dx12.h"

#include "resources/Device.h"
#include "resources/Vertex.h"

namespace lcn { namespace resources
{
	struct DeviceData;
	class DX12Device : public Device
	{
	public:
		DX12Device();
		virtual ~DX12Device() override;

		bool Initialize(Microsoft::WRL::ComPtr<ID3D12Device> a_Device);
		virtual const uint32_t UploadMesh(Vertex* a_Vertices, uint32_t a_NumVertices, const uint32_t* a_Indices, uint32_t a_NumIndices) const override;
	
		D3D12_VERTEX_BUFFER_VIEW GetVertexBuffer(uint32_t a_GUID) const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBuffer(uint32_t a_GUID) const;
	private:
		DeviceData* m_Data;
	};
};}; // lcn::resources