#include "DX12Device.h"
#include "../DX12Helpers.h"

#include <iostream>
#include <vector>

namespace lcn { namespace resources
{
	struct DeviceData
	{
		Microsoft::WRL::ComPtr<ID3D12Device> device;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexBuffers;
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> indexBuffers;
		std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferViews;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> shaders;
	};
};}; // namespace lcn::resources

using namespace lcn::resources;

DX12Device::DX12Device()
{
	m_Data = new DeviceData;
}

DX12Device::~DX12Device()
{
	delete m_Data->device.ReleaseAndGetAddressOf();
}

bool DX12Device::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> a_Device)
{
	m_Data->device = a_Device;

	return true;
}

const uint32_t DX12Device::UploadMesh(Vertex* a_Vertices, uint32_t a_NumVertices, const uint32_t* a_Indices, uint32_t a_NumIndices) const
{
	{ // Upload Vertices
		const uint32_t vertexBufferSize = sizeof(Vertex) * a_NumVertices;
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
		ThrowIfFailed(m_Data->device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertexBuffer)
		));

		uint8_t* copyDestVertexData;
		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&copyDestVertexData)));
		memcpy(copyDestVertexData, a_Vertices, sizeof(Vertex) * a_NumVertices);
		vertexBuffer->Unmap(0, nullptr);

		D3D12_VERTEX_BUFFER_VIEW view;
		view.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		view.StrideInBytes = sizeof(Vertex);
		view.SizeInBytes = vertexBufferSize;

		m_Data->vertexBuffers.push_back(vertexBuffer);
		m_Data->vertexBufferViews.push_back(view);
	}

	{ // Upload Indices
		const uint32_t indexBufferSize = sizeof(uint32_t) * a_NumIndices;
		ID3D12Resource* indexBuffer;
		ThrowIfFailed(m_Data->device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&indexBuffer)
		));

		uint8_t* copyDestIndexData;
		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(indexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&copyDestIndexData)));
		memcpy(copyDestIndexData, a_Indices, sizeof(uint32_t) * a_NumIndices);
		indexBuffer->Unmap(0, nullptr);

		D3D12_INDEX_BUFFER_VIEW view;
		view.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		view.Format = DXGI_FORMAT_R32_UINT;
		view.SizeInBytes = sizeof(uint32_t) * a_NumIndices;

		m_Data->indexBuffers.push_back(indexBuffer);
		m_Data->indexBufferViews.push_back(view);
	}

	return (uint32_t)m_Data->vertexBuffers.size();
}

D3D12_VERTEX_BUFFER_VIEW DX12Device::GetVertexBuffer(uint32_t a_GUID) const
{
	return m_Data->vertexBufferViews.at(a_GUID);
}

D3D12_INDEX_BUFFER_VIEW DX12Device::GetIndexBuffer(uint32_t a_GUID) const
{
	return m_Data->indexBufferViews.at(a_GUID);
}