#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include "d3dx12.h"
#include "HAL/Platform.h"



// use triple buffer
#define FrameBufferCount 3 



class FD3D12Manager
{
public:
	bool Initialize(HWND InHWnd);
	void UpdatePipeline();

	void Render();

	void Cleanup();

	void WaitPreviousFrame();

protected:
	ID3D12Device* Device = nullptr; // direct3d device
	ID3D12CommandQueue* CommandQueue = nullptr; // container for command lists
	IDXGISwapChain3* SwapChain = nullptr;
	int32 FrameIndex = 0;
	ID3D12DescriptorHeap* RenderTargetViewDescriptorHeap = nullptr; // a descriptor heap to hold resources like the render targets

	int32 RenderTargetViewDescriptorSize = 0; // size of the render target view descriptor on the device(all front and back buffers will be the same size)
	ID3D12Resource* RenderTargets[FrameBufferCount] = {0}; // number of render targets equal to buffer count

	 // we want enough allocators for each buffer * number of threads (we only have one thread)
	ID3D12CommandAllocator* CommandAllocator[FrameBufferCount] = {0};

	// a command list we can record commands into, then execute them to render the frame
	ID3D12GraphicsCommandList* CommandList = nullptr; 

	// this value is increase each frame. each fence will have its own value.
	UINT64 FenceValue[FrameBufferCount] = {0};

	// an object that is locked while our command list is being executed by the gpu. 
	// We need as many as we have allocators (more if we want to know when the gpu is finished with an asset)
	ID3D12Fence* Fence[FrameBufferCount] = { 0 };

	// a handle to an event when our fence is unlocked by the GPU
	HANDLE FenceEvent; 


};
