#include "LacunaDX12Renderer.h"
#include "DX12Helpers.h"
#include "system/EntityFactory.h"
#include <game_objects/Entity.h>
#include "game_objects/Camera.h"
#include "game_objects/Components/MeshComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <stack>

#define DX12_CBUFFER_ALIGNMENT (256)
#define DX12_ALIGNED_SIZE(size, alignment) ((size + (alignment - 1) & (~(alignment - 1))))
#define DX12_CONSTANT_BUFFER_ELEMENT_SIZE(elementBytes) DX12_ALIGNED_SIZE(elementBytes, DX12_CBUFFER_ALIGNMENT)

/*
	This file is heavily based upon the DX-Graphics-Sample files provided by Microsoft
	A new version of this is in the works but not yet ready for implementation.

	Rendering currently happens with only 1 object
*/

using namespace lcn::graphics;
using Microsoft::WRL::ComPtr;

struct MVP_CONSTANT_BUFFER {
	glm::mat4 mvp;
};

LacunaDX12Renderer::LacunaDX12Renderer()
	: Renderer()
{
	m_Data = new helpers::DX12Data;
}

LacunaDX12Renderer::~LacunaDX12Renderer()
{

}

bool LacunaDX12Renderer::Initialize(const lcn::platform::specifics::PlatformHandles* a_Handles)
{
	UINT dxgiFactoryFlags = 0;

	// m_Data->viewport.TopLeftX = 0.0f;
	// m_Data->viewport.TopLeftY = 0.0f;
	// m_Data->viewport.Width = GRAPHICS_BUFFER_WIDTH;
	// m_Data->viewport.Height = GRAPHICS_BUFFER_HEIGHT;
	// m_Data->viewport.MaxDepth = 1000.f;
	// m_Data->viewport.MinDepth = -1.000f;
	m_Data->viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, GRAPHICS_BUFFER_WIDTH, GRAPHICS_BUFFER_HEIGHT);

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
		debugController.Reset();
	}
#endif

	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_Data->DXGIFactory)));

	if(USE_WARP_DEVICE)
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(m_Data->DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Data->device)));
	}
	else
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;

		{ // Get the hardware adapter
			IDXGIFactory2* pFactory = m_Data->DXGIFactory.Get();
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
		hardwareAdapter.Reset();
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
	ThrowIfFailed(m_Data->DXGIFactory->CreateSwapChainForHwnd(
		m_Data->commandqueue.Get(),
		a_Handles->hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	ThrowIfFailed(m_Data->DXGIFactory->MakeWindowAssociation(a_Handles->hWnd, DXGI_MWA_NO_ALT_ENTER));

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

		D3D12_DESCRIPTOR_HEAP_DESC depthHeapDesc = {};
		depthHeapDesc.NumDescriptors = NUM_GRAPHICS_BUFFERS;
		depthHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		depthHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(m_Data->device->CreateDescriptorHeap(&depthHeapDesc, IID_PPV_ARGS(&m_Data->dsvHeap)));

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
			&CD3DX12_RESOURCE_DESC::Buffer(DX12_ALIGNED_SIZE(512 * DX12_CONSTANT_BUFFER_ELEMENT_SIZE(sizeof(MVP_CONSTANT_BUFFER)), 64 * 1024)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_Data->constantBuffer)));

		// Map and initialize the constant buffer. We don't unmap this until the
		// app closes. Keeping things mapped for the lifetime of the resource is okay.
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_Data->constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_Data->m_cbvDataBegin)));
	}

	{
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		D3D12_CLEAR_VALUE dsvOptimizedClearValue = {};
		dsvOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		dsvOptimizedClearValue.DepthStencil.Depth = 1.0f;
		dsvOptimizedClearValue.DepthStencil.Stencil = 0.0f;

		m_Data->device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, GRAPHICS_BUFFER_WIDTH, GRAPHICS_BUFFER_HEIGHT, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&dsvOptimizedClearValue,
			IID_PPV_ARGS(&m_Data->dsvBuffer)
		);
		m_Data->dsvHeap->SetName(L"Depth/Stencil Heap");
		m_Data->device->CreateDepthStencilView(m_Data->dsvBuffer.Get(), &dsvDesc, m_Data->dsvHeap->GetCPUDescriptorHandleForHeapStart());
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

void LacunaDX12Renderer::Render(lcn::resources::SceneResource* a_Scene)
{
	PrepareData(a_Scene);

	// // Record all the commands we need to render the scene into the command list.
	PopulateCommandList(a_Scene);

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { m_Data->commandList.Get() };
	m_Data->commandqueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	ThrowIfFailed(m_Data->swapchain->Present(1, 0));

	WaitForPreviousFrame();
}

void LacunaDX12Renderer::PrepareData(lcn::resources::SceneResource* a_Scene)
{
	const std::vector<lcn::object::Entity*>* entities = a_Scene->GetEntities();
	lcn::object::Camera* camera = lcn::EntityFactory::GetMainCamera();

	MVP_CONSTANT_BUFFER mvp_cb = {};

	int meshIndex = 0;
	for (size_t i = 0; i < entities->size(); i++)
	{
		std::vector<lcn::object::MeshComponent*> meshes = entities->at(i)->GetComponentsByType<lcn::object::MeshComponent>();
		for (size_t j = 0; j < meshes.size(); j++)
		{
			mvp_cb.mvp = camera->GetProjectionMatrix() * camera->GetViewMatrix() * entities->at(i)->GetWorldMatrix();
			memcpy(m_Data->m_cbvDataBegin + meshIndex * DX12_CONSTANT_BUFFER_ELEMENT_SIZE(sizeof(mvp_cb)), &mvp_cb, sizeof(mvp_cb));
			meshIndex++;
		}
	}
}

void LacunaDX12Renderer::Cleanup()
{
	WaitForPreviousFrame();

	m_Data->swapchain.Reset();
	m_Data->commandAllocator.Reset();
	m_Data->commandqueue.Reset();
	m_Data->commandList.Reset();
	m_Data->rtvHeap.Reset();
	m_Data->dsvHeap.Reset();
	m_Data->cbvHeap.Reset();

	for (int i = 0; i < NUM_GRAPHICS_BUFFERS; i++)
		m_Data->renderTargets[i].Reset();

	m_Data->rootSignature.Reset();
	m_Data->pipelineState.Reset();
	
	m_Data->DXGIFactory.Reset();

	m_Data->dsvBuffer.Reset();
	m_Data->constantBuffer.Reset();

	CloseHandle(m_Data->fenceEvent);
	m_Data->fence.Reset();

	m_Data->device.Reset();

	delete m_Data;
	m_Data = nullptr;

#ifdef _DEBUG
	{
		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL));
		}
	}
#endif
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

void LacunaDX12Renderer::PopulateCommandList(lcn::resources::SceneResource* a_Scene)
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(m_Data->commandAllocator->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(m_Data->commandList->Reset(m_Data->commandAllocator.Get(), m_Data->myDevice.GetPipelineState(0).Get()));

	// Set necessary state.
	m_Data->commandList->SetGraphicsRootSignature(m_Data->myDevice.GetRootSignature().Get());

	ID3D12DescriptorHeap* ppHeaps[] = { m_Data->cbvHeap.Get() };
	m_Data->commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	m_Data->commandList->RSSetViewports(1, &m_Data->viewport);
	m_Data->commandList->RSSetScissorRects(1, &m_Data->scissorRect);

	// Indicate that the back buffer will be used as a render target.
	m_Data->commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Data->renderTargets[m_Data->frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_Data->rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_Data->frameIndex, m_Data->rtvDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_Data->dsvHeap->GetCPUDescriptorHandleForHeapStart());
	m_Data->commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// Record commands.
	const float clearColor[] = {0.1f, 0.1f, 0.1f, 1.0f};
	m_Data->commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_Data->commandList->ClearDepthStencilView(m_Data->dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0.0f, 0, nullptr);

	m_Data->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const std::vector<lcn::object::Entity*>* entities = a_Scene->GetEntities();
	int meshIndex = 0;
	for (size_t i = 0; i < entities->size(); i++)
	{
		std::vector<lcn::object::MeshComponent*> meshes = entities->at(i)->GetComponentsByType<lcn::object::MeshComponent>();
		for (size_t j = 0; j < meshes.size(); j++)
		{
			size_t inc = DX12_CONSTANT_BUFFER_ELEMENT_SIZE(sizeof(MVP_CONSTANT_BUFFER)) * meshIndex;
			m_Data->commandList->SetGraphicsRootConstantBufferView(0, m_Data->constantBuffer->GetGPUVirtualAddress() + inc);
			meshIndex++;
			m_Data->commandList->IASetVertexBuffers(0, 1, &m_Data->myDevice.GetVertexBuffer(meshes.at(j)->GetMeshGUID()));
			m_Data->commandList->IASetIndexBuffer(&m_Data->myDevice.GetIndexBuffer(meshes.at(j)->GetMeshGUID()));
			m_Data->commandList->DrawIndexedInstanced(meshes.at(j)->GetMeshIndexCount(), 1, 0, 0, 0);
		}
	}

	// Indicate that the back buffer will now be used to present.
	m_Data->commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Data->renderTargets[m_Data->frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_Data->commandList->Close());
}