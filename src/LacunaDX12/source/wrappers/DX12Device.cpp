#include "DX12Device.h"
#include "../DX12Helpers.h"
#include <iostream>
#include <vector>
#include <string>

namespace lcn::resources
{
	struct DeviceData
	{
		Microsoft::WRL::ComPtr<ID3D12Device> device;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexBuffers;
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> indexBuffers;
		std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferViews;
		std::vector<Microsoft::WRL::ComPtr<ID3DBlob>> shaders;
		std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelines;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	};
}; // namespace lcn::resources

using namespace lcn::resources;

DX12Device::DX12Device()
{
	m_Data = new DeviceData;
}

DX12Device::~DX12Device()
{
	for (size_t i = 0; i < m_Data->pipelines.size(); i++)
	{
		m_Data->pipelines.at(i).Reset();
	}
	for (size_t i = 0; i < m_Data->shaders.size(); i++)
	{
		m_Data->shaders.at(i).Reset();
	}
	for (size_t i = 0; i < m_Data->indexBuffers.size(); i++)
	{
		m_Data->indexBuffers.at(i).Reset();
	}
	for (size_t i = 0; i < m_Data->vertexBuffers.size(); i++)
	{
		m_Data->vertexBuffers.at(i).Reset();
	}

	m_Data->rootSignature.Reset();

	m_Data->pipelines.clear();
	m_Data->shaders.clear();
	m_Data->indexBuffers.clear();
	m_Data->indexBufferViews.clear();
	m_Data->vertexBuffers.clear();
	m_Data->vertexBufferViews.clear();
	
	m_Data->device.Reset();

	delete m_Data;
	m_Data = nullptr;

}

bool DX12Device::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> a_Device)
{
	a_Device->SetName(L"Device");
	m_Data->device = a_Device;

	return true;
}

const uint32_t DX12Device::CreatePipelineState(const PipelineParams* const a_Params) const
{
	return CreateNormalPipelineState(a_Params);
}

const uint32_t DX12Device::UploadShader(const char* a_AbsPath) const
{
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;

	const size_t path_size = strlen(a_AbsPath) + 1;
	wchar_t* uni_path = new wchar_t[path_size];
	mbstowcs(uni_path, a_AbsPath, path_size);

	ThrowIfFailed(D3DReadFileToBlob(uni_path, &shaderBlob));
	m_Data->shaders.push_back(shaderBlob);
	return m_Data->shaders.size();
}

const uint32_t DX12Device::UploadAndCompileShader(const char* a_AbsPath, const char* a_EntryPoint, EShaderTypes a_ShaderType) const
{
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	const size_t path_size = strlen(a_AbsPath) + 1;
	wchar_t* uni_path = new wchar_t[path_size];
	mbstowcs(uni_path, a_AbsPath, path_size);

	std::string shaderTarget;

	switch (a_ShaderType)
	{
		case EShaderTypes::EShaderTypes_VS:
		{
			shaderTarget = "vs_5_0";
		} break;

		case EShaderTypes::EShaderTypes_PS:
		{
			shaderTarget = "ps_5_0";
		} break;

		default:
		{
			assert(false);
		} break;
	}

	ID3DBlob* err = nullptr;

	D3DCompileFromFile(uni_path, nullptr, nullptr, a_EntryPoint, shaderTarget.c_str(), compileFlags, 0, &shaderBlob, &err);
	if (shaderBlob == nullptr)
	{
		char* msg = (char*)err->GetBufferPointer();
		std::cout << msg << std::endl;

		ThrowIfFailed(1);
	}

	m_Data->shaders.push_back(shaderBlob);
	return m_Data->shaders.size() - 1;
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

		vertexBuffer->SetName(L"VertexBuffer");

		m_Data->vertexBuffers.push_back(vertexBuffer);
		m_Data->vertexBufferViews.push_back(view);
	}

	{ // Upload Indices
		const uint32_t indexBufferSize = sizeof(uint32_t) * a_NumIndices;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
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

		indexBuffer->SetName(L"IndexBuffer");

		m_Data->indexBuffers.push_back(indexBuffer);
		m_Data->indexBufferViews.push_back(view);
	}

	return (uint32_t)m_Data->vertexBuffers.size() - 1;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> DX12Device::GetPipelineState(uint32_t a_GUID) const
{
	return m_Data->pipelines.at(a_GUID);
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> DX12Device::GetRootSignature() const
{
	return m_Data->rootSignature;
}

D3D12_VERTEX_BUFFER_VIEW DX12Device::GetVertexBuffer(uint32_t a_GUID) const
{
	return m_Data->vertexBufferViews.at(a_GUID);
}

D3D12_INDEX_BUFFER_VIEW DX12Device::GetIndexBuffer(uint32_t a_GUID) const
{
	return m_Data->indexBufferViews.at(a_GUID);
}

uint32_t DX12Device::CreateNormalPipelineState(const PipelineParams* const a_Params) const
{
	CreateRootSignature();

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.AntialiasedLineEnable = true;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.MultisampleEnable = false;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_Data->rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_Data->shaders.at(a_Params->VertexShader).Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_Data->shaders.at(a_Params->PixelShader).Get());
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> state;
	ThrowIfFailed(m_Data->device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&state)));
	m_Data->pipelines.push_back(state);
	return m_Data->pipelines.size() - 1;
}

uint32_t DX12Device::CreateRootSignature() const
{
	// Create an empty root signature.

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

	// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (FAILED(m_Data->device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	// CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
	CD3DX12_ROOT_PARAMETER1 rootParameters[1];

	// ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
	// rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

	// Allow input layout and deny unnecessary access to certain pipeline stages.
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
	ThrowIfFailed(m_Data->device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_Data->rootSignature)));

	return 0;
}