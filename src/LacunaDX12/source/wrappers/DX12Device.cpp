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
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> textureBuffers;

		std::vector<Microsoft::WRL::ComPtr<ID3DBlob>> shaders;
		std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelines;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
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
	for (size_t i = 0; i < m_Data->textureBuffers.size(); i++)
	{
		m_Data->textureBuffers.at(i).Reset();
	}

	m_Data->rootSignature.Reset();

	m_Data->pipelines.clear();
	m_Data->shaders.clear();
	m_Data->indexBuffers.clear();
	m_Data->indexBufferViews.clear();
	m_Data->vertexBuffers.clear();
	m_Data->vertexBufferViews.clear();
	
	m_Data->commandList.Reset();
	m_Data->commandAllocator.Reset();
	m_Data->commandQueue.Reset();

	m_Data->device.Reset();

	delete m_Data;
	m_Data = nullptr;

}

bool DX12Device::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> a_Device)
{
	a_Device->SetName(L"Device");
	m_Data->device = a_Device;

	// Create command queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(m_Data->device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_Data->commandQueue)));
	m_Data->commandQueue->SetName(L"Upload Queue");
	
	ThrowIfFailed(m_Data->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_Data->commandAllocator)));
	m_Data->commandAllocator->SetName(L"Upload Allocator");

	Microsoft::WRL::ComPtr<ID3D12PipelineState> state;
	m_Data->pipelines.push_back(state);

	ThrowIfFailed(m_Data->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_Data->commandAllocator.Get(), GetPipelineState(0).Get(), IID_PPV_ARGS(&m_Data->commandList)));
	m_Data->commandList->SetName(L"Upload CList");
	ThrowIfFailed(m_Data->commandList->Close());

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

DXGI_FORMAT GetFormat(const uint32_t a_BPP)
{
	switch (a_BPP)
	{
		case 8:
		{
			return DXGI_FORMAT_R8G8B8A8_UINT;
		} break;

		case 16:
		{
			return DXGI_FORMAT_R16G16B16A16_UINT;
		} break;

		case 32:
		{
			return DXGI_FORMAT_R32G32B32A32_UINT;
		} break;
	};

	return DXGI_FORMAT_UNKNOWN;
}

const uint32_t DX12Device::UploadTexture(uint8_t* a_ImageData, uint32_t a_Width, uint32_t a_Height, uint32_t a_BPP) const
{
	if(false) { // Create resources
		D3D12_RESOURCE_DESC textureDescription = {};
		textureDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureDescription.Alignment = 0;
		textureDescription.Width = a_Width;
		textureDescription.Height = a_Height;
		textureDescription.DepthOrArraySize = 1;
		textureDescription.MipLevels = 1;
		textureDescription.Format = GetFormat(a_BPP);
		textureDescription.SampleDesc.Count = 1;
		textureDescription.SampleDesc.Quality = 0;
		textureDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		textureDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
		ThrowIfFailed(m_Data->device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&textureDescription,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&buffer)
		));
		buffer->SetName(L"Texture Buffer Resource Heap");

		Microsoft::WRL::ComPtr<ID3D12Resource> uploadHeap;
		ThrowIfFailed(m_Data->device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(DX12_ALIGNED_SIZE(a_Width * a_BPP, 256) * (a_Height - 1) + (a_Width * a_BPP)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadHeap)
		));
		uploadHeap->SetName(L"Texture Upload Buffer Resource Heap");

		D3D12_SUBRESOURCE_DATA texData = {};
		texData.pData = a_ImageData;
		texData.RowPitch = a_Width;
		texData.SlicePitch = a_Width * a_Height;

		// memcpy(m_Data->m_cbvDataBegin + meshIndex * DX12_CONSTANT_BUFFER_ELEMENT_SIZE(sizeof(mvp_cb)), &mvp_cb, sizeof(mvp_cb));
		//UpdateSubresources()
	}

	return 0;
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
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

	// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (FAILED(m_Data->device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	// CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
	CD3DX12_ROOT_PARAMETER1 rootParameters[2];

	// ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[1].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);

	// Allow input layout and deny unnecessary access to certain pipeline stages.
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
	ThrowIfFailed(m_Data->device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_Data->rootSignature)));

	return 0;
}