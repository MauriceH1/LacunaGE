#pragma once

#include "DX12Device.h"
#include "d3dx12.h"
#include <dxgi1_5.h>
#include <d3dcompiler.h>

#include <exception>

#define USE_WARP_DEVICE true
#define NUM_GRAPHICS_BUFFERS 2
#define GRAPHICS_BUFFER_WIDTH 1280
#define GRAPHICS_BUFFER_HEIGHT 720

namespace lcn { namespace platform { namespace specifics
{
	struct PlatformHandles
	{
		HINSTANCE hInstance;
		HWND hWnd;
	};
};};}; // namespace lcn::platform::specifics

namespace lcn { namespace graphics { namespace helpers
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

		UINT frameIndex = 0;
		UINT rtvDescriptorSize;

		CD3DX12_VIEWPORT viewport;
		CD3DX12_RECT scissorRect;
	};
};};}; // namespace lcn::graphics::helpers

inline void ThrowIfFailed(HRESULT hr)
{
	if(hr < 0) { throw std::exception(); }
}