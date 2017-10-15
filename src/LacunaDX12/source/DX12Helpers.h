#pragma once

#define NOMINMAX

#pragma comment(lib, "dxguid.lib")

#include "DX12Device.h"
#include "d3dx12.h"
#include <dxgi1_5.h>
#include <d3dcompiler.h>
#include <wincodec.h>

#include <dxgidebug.h>

#include <exception>

#define USE_WARP_DEVICE false
#define NUM_GRAPHICS_BUFFERS 2
#define GRAPHICS_BUFFER_WIDTH 1280
#define GRAPHICS_BUFFER_HEIGHT 720

#define DX12_CBUFFER_ALIGNMENT (256)
#define DX12_ALIGNED_SIZE(size, alignment) ((size + (alignment - 1) & (~(alignment - 1))))
#define DX12_CONSTANT_BUFFER_ELEMENT_SIZE(elementBytes) DX12_ALIGNED_SIZE(elementBytes, DX12_CBUFFER_ALIGNMENT)

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
		Microsoft::WRL::ComPtr<IDXGIFactory4> DXGIFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> swapchain;
		Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[NUM_GRAPHICS_BUFFERS];
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandqueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvHeap;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		UINT64 fenceValue;
		HANDLE fenceEvent;

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