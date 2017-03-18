#include "LacunaDX12Renderer.h"
#include "DX12Helpers.h"
#include <iostream>

using namespace lcn::graphics;
using Microsoft::WRL::ComPtr;

LacunaDX12Renderer::LacunaDX12Renderer()
	: Renderer()
{
	m_Data = new helpers::DX12Data;
}

LacunaDX12Renderer::LacunaDX12Renderer(std::wstring path)
	: LacunaDX12Renderer()
{
	m_Path = path;
}

LacunaDX12Renderer::~LacunaDX12Renderer()
{
	delete m_Data;
	m_Data = nullptr;
}

bool LacunaDX12Renderer::Initialize(const lcn::platform::specifics::PlatformHandles* a_Handles)
{
	UINT dxgiFactoryFlags = 0;

	m_Data->viewport.TopLeftX = 0.0f;
	m_Data->viewport.TopLeftY = 0.0f;
	m_Data->viewport.Width = GRAPHICS_BUFFER_WIDTH;
	m_Data->viewport.Height = GRAPHICS_BUFFER_HEIGHT;

	m_Data->scissorRect.left = 0;
	m_Data->scissorRect.top = 0;
	m_Data->scissorRect.right = GRAPHICS_BUFFER_WIDTH;
	m_Data->scissorRect.bottom = GRAPHICS_BUFFER_HEIGHT;

#if defined(_DEBUG)
	{ // EnableDebug Layer
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			
			// Enable additional debug layers
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	ComPtr<IDXGIFactory4> DXGIFactory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&DXGIFactory)));

	if(USE_WARP_DEVICE)
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Data->device)));
	}
	else
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;

		{ // Get the hardware adapter
			IDXGIFactory2* pFactory = DXGIFactory.Get();
			IDXGIAdapter1** ppAdapter = &hardwareAdapter;

			ComPtr<IDXGIAdapter1> adapter;
			*ppAdapter = nullptr;

			for(UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see if the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if(SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}

			*ppAdapter = adapter.Detach();
		}

		ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Data->device)));
	}
	m_Data->myDevice.Initialize(m_Data->device);

	// Create command queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(m_Data->device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_Data->commandqueue)));

	// Create the swapchain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = NUM_GRAPHICS_BUFFERS;
	swapChainDesc.Width = GRAPHICS_BUFFER_WIDTH;
	swapChainDesc.Height = GRAPHICS_BUFFER_HEIGHT;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed(DXGIFactory->CreateSwapChainForHwnd(
		m_Data->commandqueue.Get(),
		a_Handles->hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	ThrowIfFailed(DXGIFactory->MakeWindowAssociation(a_Handles->hWnd, DXGI_MWA_NO_ALT_ENTER));
	
	ThrowIfFailed(swapChain.As(&m_Data->swapchain));
	m_Data->frameIndex = m_Data->swapchain->GetCurrentBackBufferIndex();

	// Descriptor Heaps
	{
		// Setup Render target view heap
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = NUM_GRAPHICS_BUFFERS;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(m_Data->device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_Data->rtvHeap)));
		m_Data->rtvDescriptorSize = m_Data->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_Data->rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// create RTV per frame
		for(UINT i = 0; i < NUM_GRAPHICS_BUFFERS; i++)
		{
			ThrowIfFailed(m_Data->swapchain->GetBuffer(i, IID_PPV_ARGS(&m_Data->renderTargets[i])));
			m_Data->device->CreateRenderTargetView(m_Data->renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_Data->rtvDescriptorSize);
		}
	}

	ThrowIfFailed(m_Data->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_Data->commandAllocator)));

	UploadCube();

	return 0;
}

void LacunaDX12Renderer::UploadCube()
{
	// Create an empty root signature.
	{
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
		ThrowIfFailed(m_Data->device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_Data->rootSignature)));
	}

	// Create the pipeline state, which includes compiling and loading shaders.
	{
		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif
		std::wstring a(m_Path);
		a.append(L"shaders.hlsl");

		ThrowIfFailed(D3DCompileFromFile(a.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
		ThrowIfFailed(D3DCompileFromFile(a.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};		// Define the vertex input layout.
		

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
		psoDesc.pRootSignature = m_Data->rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(m_Data->device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_Data->pipelineState)));
	}

	// Create the command list.
	ThrowIfFailed(m_Data->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_Data->commandAllocator.Get(), m_Data->pipelineState.Get(), IID_PPV_ARGS(&m_Data->commandList)));

	// Create the vertex buffer.
	{
		// Define the geometry for a triangle.
		TVertex triangleVertices[] =
		{
			// VERTEX				COLOR
			{{-1.0f,  1.0f, 0.0f},{1.0f, 0.0f, 0.0f, 1.0f}},
			{{ 1.0f, -1.0f, 0.0f},{0.0f, 1.0f, 0.0f, 1.0f}},
			{{-1.0f, -1.0f, 0.0f},{0.0f, 0.0f, 1.0f, 1.0f}},
			{{ 1.0f,  1.0f, 0.0f},{0.0f, 0.0f, 0.0f, 1.0f}}
		};

		//TVertex triangleVertices[] = {
		//	{ { -1.f,-1.f,-1.f },{-1.f,-1.f,-1.f,1.0f } },
		//	{ {  1.f,-1.f,-1.f },{ 1.f,-1.f,-1.f,1.0f } },
		//	{ { -1.f, 1.f,-1.f },{-1.f, 1.f,-1.f,1.0f } },
		//	{ {  1.f, 1.f,-1.f },{ 1.f, 1.f,-1.f,1.0f } },
		//	{ { -1.f,-1.f, 1.f },{-1.f,-1.f, 1.f,1.0f } },
		//	{ {  1.f,-1.f, 1.f },{ 1.f,-1.f, 1.f,1.0f } },
		//	{ { -1.f, 1.f, 1.f },{-1.f, 1.f, 1.f,1.0f } },
		//	{ {  1.f, 1.f, 1.f },{ 1.f, 1.f, 1.f,1.0f } }
		//};

		const UINT vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		ThrowIfFailed(m_Data->device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_Data->vertexBuffer)));

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_Data->vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_Data->vertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_Data->vertexBufferView.BufferLocation = m_Data->vertexBuffer->GetGPUVirtualAddress();
		m_Data->vertexBufferView.StrideInBytes = sizeof(TVertex);
		m_Data->vertexBufferView.SizeInBytes = vertexBufferSize;
	}

	
	ComPtr<ID3D12Resource> indexBufferUploadHeap;
	{ // Create Index Buffer
		const UINT indices[] = {0, 1, 2, 0, 3, 1};
		//const UINT indices[] = {
		//	0, 1, 2,
		//	1, 3, 2,
		//	4, 6, 5,
		//	6, 7, 5
		//};
		const UINT indexBufferSize = sizeof(indices);
	
		ThrowIfFailed(m_Data->device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_Data->indexBuffer)
		));

		ThrowIfFailed(m_Data->device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&indexBufferUploadHeap)
		));
	
		//UINT32* pIndexDataBegin;
		//CD3DX12_RANGE readRange(0, 0);
		//ThrowIfFailed(m_Data->indexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin)));
		//memcpy(pIndexDataBegin, indices, sizeof(UINT));
		//m_Data->indexBuffer->Unmap(0, nullptr);
		//
		//m_Data->commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Data->indexBuffer.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_INDEX_BUFFER));

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = &indices;
		indexData.RowPitch = indexBufferSize;

		UpdateSubresources<1>(m_Data->commandList.Get(), m_Data->indexBuffer.Get(), indexBufferUploadHeap.Get(), 0, 0, 1, &indexData);
		m_Data->commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Data->indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

		m_Data->indexBufferView.BufferLocation = m_Data->indexBuffer->GetGPUVirtualAddress();
		m_Data->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_Data->indexBufferView.SizeInBytes = indexBufferSize;
	}

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(m_Data->commandList->Close());
	ID3D12CommandList* ppCommandLists[] = { m_Data->commandList.Get() };
	m_Data->commandqueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		ThrowIfFailed(m_Data->device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Data->fence)));
		m_Data->fenceValue = 1;

		// Create an event handle to use for frame synchronization.
		m_Data->fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if(m_Data->fenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		WaitForPreviousFrame();
	}
}

void LacunaDX12Renderer::Render()
{
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = {m_Data->commandList.Get()};
	m_Data->commandqueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	ThrowIfFailed(m_Data->swapchain->Present(1, 0));

	WaitForPreviousFrame();
}

void LacunaDX12Renderer::Cleanup()
{
	WaitForPreviousFrame();

	CloseHandle(m_Data->fenceEvent);
}

const lcn::resources::Device* LacunaDX12Renderer::GetDevice() const
{
	return &m_Data->myDevice;
}

void LacunaDX12Renderer::WaitForPreviousFrame()
{
	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
	// sample illustrates how to use fences for efficient resource usage and to
	// maximize GPU utilization.

	// Signal and increment the fence value.
	const UINT64 fence = m_Data->fenceValue;
	ThrowIfFailed(m_Data->commandqueue->Signal(m_Data->fence.Get(), fence));
	m_Data->fenceValue++;

	// Wait until the previous frame is finished.
	if(m_Data->fence->GetCompletedValue() < fence)
	{
		ThrowIfFailed(m_Data->fence->SetEventOnCompletion(fence, m_Data->fenceEvent));
		WaitForSingleObject(m_Data->fenceEvent, INFINITE);
	}

	m_Data->frameIndex = m_Data->swapchain->GetCurrentBackBufferIndex();
}

void LacunaDX12Renderer::PopulateCommandList()
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(m_Data->commandAllocator->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(m_Data->commandList->Reset(m_Data->commandAllocator.Get(), m_Data->pipelineState.Get()));

	// Set necessary state.
	m_Data->commandList->SetGraphicsRootSignature(m_Data->rootSignature.Get());
	m_Data->commandList->RSSetViewports(1, &m_Data->viewport);
	m_Data->commandList->RSSetScissorRects(1, &m_Data->scissorRect);

	// Indicate that the back buffer will be used as a render target.
	m_Data->commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Data->renderTargets[m_Data->frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_Data->rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_Data->frameIndex, m_Data->rtvDescriptorSize);
	m_Data->commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Record commands.
	const float clearColor[] = {0.1f, 0.1f, 0.1f, 1.0f};
	m_Data->commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_Data->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//m_Data->commandList->IASetVertexBuffers(0, 1, &m_Data->myDevice.GetVertexBuffer(0));
	//m_Data->commandList->IASetIndexBuffer(&m_Data->myDevice.GetIndexBuffer(0));
	m_Data->commandList->IASetIndexBuffer(&m_Data->indexBufferView);
	m_Data->commandList->IASetVertexBuffers(0, 1, &m_Data->vertexBufferView);
	//m_Data->commandList->DrawInstanced(4, 1, 0, 0);
	m_Data->commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

	// Indicate that the back buffer will now be used to present.
	m_Data->commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Data->renderTargets[m_Data->frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_Data->commandList->Close());
}