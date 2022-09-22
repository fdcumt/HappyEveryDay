#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include "d3dx12.h"
#include "HAL/Platform.h"
#include <DirectXMath.h>



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

	// PSO containing a pipeline state
	ID3D12PipelineState *PipelineStateObject = nullptr;

	// root signature defines data shader will access
	ID3D12RootSignature *RootSignature = nullptr;

	// area that output from rasterizer will be stretched to
	D3D12_VIEWPORT Viewport;

	// the area to draw in. pixel outside that area will not be draw
	D3D12_RECT ScissorRect;

	// a default buffer in GPU memory that will we will load the vertex data for our triangle into
	ID3D12Resource *VertexBuffer = nullptr;

	// a structure containing a pointer to the vertex data in GPU memory
	// the total size of the buffer
	// the size of each element(vertex)
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;

	ID3D12Resource *IndexBuffer = nullptr; // a default buffer in GPU memory that we will load index data for our triangle into
	D3D12_INDEX_BUFFER_VIEW IndexBufferView; // a struct holding information about the index buffer

	struct FVertex
	{
		FVertex(float x, float y, float z, float r, float g, float b, float a)
		 : pos(x, y, z)
		 , color(r, g, b, a)
		{
		}
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
	};

};
