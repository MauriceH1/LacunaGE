#pragma once

#define NOMINMAX

#include "DX12Device.h"
#include "d3dx12.h"
#include <dxgi1_5.h>
#include <d3dcompiler.h>

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

#include <exception>

#define USE_WARP_DEVICE false
#define NUM_GRAPHICS_BUFFERS 2
#define GRAPHICS_BUFFER_WIDTH 1280
#define GRAPHICS_BUFFER_HEIGHT 720

namespace lcn::platform::specifics
{
	struct PlatformHandles
	{
		HINSTANCE hInstance;
		HWND hWnd;
	};
}; // namespace lcn::platform::specifics

namespace lcn::graphics::helpers
{
	struct DX12Data
	{
		Microsoft::WRL::ComPtr<ID3D12Device> device;
		lcn::resources::DX12Device myDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> swapchain;
		Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[NUM_GRAPHICS_BUFFERS];
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandqueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvHeap;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		UINT64 fenceValue;
		HANDLE fenceEvent;

		Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;

		Microsoft::WRL::ComPtr<ID3D12Resource> dsvBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;

		uint8_t* m_cbvDataBegin = nullptr;

		UINT frameIndex = 0;
		UINT rtvDescriptorSize;



		CD3DX12_VIEWPORT viewport;
		CD3DX12_RECT scissorRect;
	};
};// namespace lcn::graphics::helpers

inline void ThrowIfFailed(HRESULT hr)
{
	if(hr < 0) { throw std::exception(); }
}