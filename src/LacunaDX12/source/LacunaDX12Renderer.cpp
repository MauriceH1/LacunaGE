#include "LacunaDX12Renderer.h"
#include "DX12Helpers.h"
#include "system/EntityFactory.h"
#include "game_objects/Camera.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

using namespace lcn::graphics;
using Microsoft::WRL::ComPtr;

struct MVP_CONSTANT_BUFFER {
	glm::mat4 mvp;
};

static MVP_CONSTANT_BUFFER mvp_cb;
static glm::vec3 m_Rotation;

LacunaDX12Renderer::LacunaDX12Renderer()
	: Renderer()
{
	m_Data = new helpers::DX12Data;
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
	m_Data->viewport.MaxDepth = 1000.f;
	m_Data->viewport.MinDepth = 0.001f;

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

		// Constant buffer view heap
		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		ThrowIfFailed(m_Data->device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_Data->cbvHeap)));
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
	m_Data->commandAllocator->SetName(L"Allocator");

	CreateObjects();

	return 0;
}

void LacunaDX12Renderer::CreateObjects()
{
	// Create the command list.
	ThrowIfFailed(m_Data->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_Data->commandAllocator.Get(), m_Data->pipelineState.Get(), IID_PPV_ARGS(&m_Data->commandList)));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(m_Data->commandList->Close());

	{ // Create constant buffer
		ThrowIfFailed(m_Data->device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_Data->constantBuffer)));

		// Describe and create a constant buffer view.
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_Data->constantBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = (sizeof(MVP_CONSTANT_BUFFER) + 255) & ~255;	// CB size is required to be 256-byte aligned.
		m_Data->device->CreateConstantBufferView(&cbvDesc, m_Data->cbvHeap->GetCPUDescriptorHandleForHeapStart());

		std::cout << "Buffer size:\t" << 1024 * 64 << ";\n";
		std::cout << "buff-object:\t" << cbvDesc.SizeInBytes << ";\n";
		std::cout << "Buffer left:\t" << 1024 * 64 - cbvDesc.SizeInBytes << ";\n";

		// Map and initialize the constant buffer. We don't unmap this until the
		// app closes. Keeping things mapped for the lifetime of the resource is okay.
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_Data->constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_Data->m_cbvDataBegin)));


		// m_Rotation = glm::vec3(0.f, 0.f, 0.f);
		// mvp = glm::rotate(mvp, glm::length(m_Rotation), glm::normalize(m_Rotation));
	}
	
	//ID3D12CommandList* ppCommandLists[] = { m_Data->commandList.Get() };
	//m_Data->commandqueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

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

void LacunaDX12Renderer::Render(lcn::object::Entity* a_RootEntity)
{
	PrepareData();

	lcn::object::Camera* camera = lcn::EntityFactory::GetMainCamera();
	// glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
	// glm::mat4 viewMatrix = camera->GetViewMatrix();
	// SEE IF THIS WOULD BE AN OPTIMIZATION

	// Transform data
	MVP_CONSTANT_BUFFER buf = {};
	// glm::perspectiveFovRH<float>(glm::radians(80.f), 1280.f, 720, 0.001f, 1000.f)
	buf.mvp = camera->GetProjectionMatrix() * camera->GetViewMatrix() * a_RootEntity->GetWorldMatrix();// *glm::perspectiveFovLH<float>(glm::radians(80.f), 1280.f, 720, 0.001f, 1000.f);

	memcpy(m_Data->m_cbvDataBegin, &buf, sizeof(buf));

	// EndPreparation();

	// // Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { m_Data->commandList.Get() };
	m_Data->commandqueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	ThrowIfFailed(m_Data->swapchain->Present(1, 0));

	WaitForPreviousFrame();
}

void LacunaDX12Renderer::PrepareData()
{

}

void LacunaDX12Renderer::OldRender()
{
	m_Rotation.y += 15.f / 60.f;
	m_Rotation.z += 5.f / 60.f;
	m_Rotation.x -= 5.f / 60.f;
	if (m_Rotation.z > 360)
		m_Rotation.z - 360;
	if (m_Rotation.y > 360)
		m_Rotation.y - 360;
	if (m_Rotation.x < -360)
		m_Rotation.x + 360;
	glm::mat4 mvp = glm::perspectiveFovLH<float>(glm::radians(80.f), 1280.f, 720, 0.001f, 1000.f);
	mvp = glm::translate(mvp, glm::vec3(-1.2f, -1.5f, 4.0f));
	mvp = glm::rotate(mvp, glm::length(m_Rotation), glm::normalize(m_Rotation));
	//memcpy(m_Data->m_cbvDataBegin, &mvp_cb, sizeof(mvp_cb));
	
	mvp_cb.mvp = mvp;
	
	memcpy(m_Data->m_cbvDataBegin, &mvp_cb, sizeof(mvp_cb));
	// 
	// // Record all the commands we need to render the scene into the command list.
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
	// ThrowIfFailed(m_Data->commandList->Reset(m_Data->commandAllocator.Get(), m_Data->pipelineState.Get()));
	ThrowIfFailed(m_Data->commandList->Reset(m_Data->commandAllocator.Get(), m_Data->myDevice.GetPipelineState(0).Get()));

	// Set necessary state.
	// m_Data->commandList->SetGraphicsRootSignature(m_Data->rootSignature.Get());
	m_Data->commandList->SetGraphicsRootSignature(m_Data->myDevice.GetRootSignature().Get());

	ID3D12DescriptorHeap* ppHeaps[] = { m_Data->cbvHeap.Get() };
	m_Data->commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// TODO: HERE WE ARE
	m_Data->commandList->SetGraphicsRootDescriptorTable(0, m_Data->cbvHeap->GetGPUDescriptorHandleForHeapStart());

	m_Data->commandList->RSSetViewports(1, &m_Data->viewport);
	m_Data->commandList->RSSetScissorRects(1, &m_Data->scissorRect);

	// Indicate that the back buffer will be used as a render target.
	m_Data->commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Data->renderTargets[m_Data->frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_Data->rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_Data->frameIndex, m_Data->rtvDescriptorSize);
	m_Data->commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Record commands.
	const float clearColor[] = {0.1f, 0.1f, 0.1f, 1.0f};
	m_Data->commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_Data->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);// D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Data->commandList->IASetVertexBuffers(0, 1, &m_Data->myDevice.GetVertexBuffer(0));
	m_Data->commandList->IASetIndexBuffer(&m_Data->myDevice.GetIndexBuffer(0));
	m_Data->commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

	// Indicate that the back buffer will now be used to present.
	m_Data->commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Data->renderTargets[m_Data->frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_Data->commandList->Close());
}