#include "D3D12Manager.h"
#include <dxgi1_4.h>
#include "Common/RHICommonDefine.h"
#include "Misc/CoreGlobals.h"
#include <d3d12sdklayers.h>
#include <d3dcompiler.h>

#define SAFE_RELEASE(p) {if (p) {(p)->Release(); (p) = nullptr; }}

// width and height of the window
int Width = 800;
int Height = 600;

const bool bFullWindow = false;


bool FD3D12Manager::Initialize(HWND InHWnd)
{
	HRESULT hr;
	IDXGIFactory4* pDxgiFactory = nullptr;
	{ // create factory
		UINT FactoryFlags = 0;
#if defined(_DEBUG)
		ID3D12Debug *DebugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
		{
			DebugController->EnableDebugLayer();
			FactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
		DebugController->Release();
#endif

		hr = CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&pDxgiFactory));
		if (FAILED(hr))
		{
			ErrorLog(RHILog, "FD3D12Manager::Initialize CreateDXGIFactory1 Failed");
			return false;
		}
	}


	IDXGIAdapter1* pAdapter = nullptr;
	{ // create adapter
		int32 AdapterIndex = 0;
		bool bAdapterFound = false;

		while (pDxgiFactory->EnumAdapters1(AdapterIndex, &pAdapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 Desc;
			pAdapter->GetDesc1(&Desc);

			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}

			// try create a null device
			hr = D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
			if (SUCCEEDED(hr))
			{
				bAdapterFound = true;
				break;
			}
		}

		if (!bAdapterFound)
		{
			ErrorLog(RHILog, "FD3D12Manager::Initialize CreateDXGIFactory1 Failed");
			return false;
		}

		// create the device
		hr = D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device));
		if (FAILED(hr))
		{
			ErrorLog(RHILog, "FD3D12Manager::Initialize D3D12CreateDevice Failed");
			return false;
		}
	}

	{ // create a direct command queue
		D3D12_COMMAND_QUEUE_DESC CommandQueueDesc = {};
		CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // direct means the GPU can directly execute the command queue

		hr = Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&CommandQueue));
		if (FAILED(hr))
		{
			ErrorLog(RHILog, "FD3D12Manager::Initialize CreateCommandQueue Failed");
			return false;
		}
	}

	DXGI_SAMPLE_DESC SampleDesc = {};
	SampleDesc.Count = 1; // multisample count. no multisample, so we just put 1, since we still need 1 sample.

	{ // create the Swap Chain (double or triple buffering)
		DXGI_MODE_DESC BackBufferDesc = {};
		BackBufferDesc.Width = Width;
		BackBufferDesc.Height = Height;
		BackBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format the buffer (rgba 32 bit, 8 bit for each channel)



		DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
		SwapChainDesc.BufferCount = FrameBufferCount; // use triple buffer
		SwapChainDesc.BufferDesc = BackBufferDesc;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // the pipeline will render to the swap chain
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer(data) after we call persent
		SwapChainDesc.OutputWindow = InHWnd;
		SwapChainDesc.SampleDesc = SampleDesc;
		SwapChainDesc.Windowed = !bFullWindow;

		IDXGISwapChain *TempSwapChain = nullptr;
		hr = pDxgiFactory->CreateSwapChain(CommandQueue, &SwapChainDesc, &TempSwapChain);
		if (FAILED(hr))
		{
			BreakLog(RHILog, "FD3D12Manager::Initialize CreateSwapChain Failed");
			ErrorLog(RHILog, "FD3D12Manager::Initialize CreateSwapChain Failed");
			return false;
		}

		SwapChain = static_cast<IDXGISwapChain3*>(TempSwapChain);

		FrameIndex = SwapChain->GetCurrentBackBufferIndex();

		// create the back buffers(render target views) descriptor heap
		// describe an render target view descriptor heap and create
		D3D12_DESCRIPTOR_HEAP_DESC RenderTargetViewHeapDesc = {};
		RenderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		RenderTargetViewHeapDesc.NumDescriptors = FrameBufferCount;

		// this heap will not be directly referenced by the shader(not shader visible),
		// as this will store the output from the pipeline
		// otherwise we would set the heap`s flag to D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		RenderTargetViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		hr = Device->CreateDescriptorHeap(&RenderTargetViewHeapDesc, IID_PPV_ARGS(&RenderTargetViewDescriptorHeap));
		if (FAILED(hr))
		{
			ErrorLog(RHILog, "FD3D12Manager::Initialize CreateDescriptorHeap Failed");
			return false;
		}

		// get the size of descriptor in this heap
		// this is render target view descriptor should be store in it
		// descriptor sizes may vary from device to device, which is why there is no set size and we must ask
		// the device to give use the size. we will use this size to increase descriptor handle offset
		RenderTargetViewDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// get a handle to the first descriptor in the descriptor heap.
		// a handle is basically a pointer, but we cannot literally us it like a c++ pointer
		CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTargetViewHandle(RenderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		for (int32 i=0; i<FrameBufferCount; ++i)
		{
			hr = SwapChain->GetBuffer(i, IID_PPV_ARGS(&RenderTargets[i]));
			if (FAILED(hr))
			{
				ErrorLog(RHILog, "FD3D12Manager::Initialize SwapChain GetBuffer [%d] Failed", i);
				return false;
			}

			// the we "create" a render target view which binds the swap chain buffer(ID3D12Resource[n]) to the render target view handle
			Device->CreateRenderTargetView(RenderTargets[i], nullptr, RenderTargetViewHandle);

			// we increment the render target view handle by the render target view descriptor size we got above
			RenderTargetViewHandle.Offset(1, RenderTargetViewDescriptorSize);
		}
	}

	{ // create the command allocators
		for (int32 i=0; i<FrameBufferCount; ++i)
		{
			hr = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator[i]));
			if (FAILED(hr))
			{
				ErrorLog(RHILog, "FD3D12Manager::Initialize CreateCommandAllocator [%d] Failed", i);
				return false;
			}
		}
	}

	{ // create a command list
		hr = Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator[0], nullptr, IID_PPV_ARGS(&CommandList));
		if (FAILED(hr))
		{
			ErrorLog(RHILog, "FD3D12Manager::Initialize CreateCommandList Failed");
			return false;
		}

	}

	{ // create fence
		for (int32 i=0; i<FrameBufferCount; ++i)
		{
			hr = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence[i]));
		}
		
		FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (FenceEvent == nullptr)
		{
			ErrorLog(RHILog, "FD3D12Manager::Initialize CreateEvent Failed");
			return false;
		}
	}

	{ // create root signature
		CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc;
		// 使用InputAssembler, 指定InputLayout
		RootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		ID3DBlob *Signature;
		hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, nullptr);
		if (FAILED(hr))
		{
			ErrorLog(RHILog, "FD3D12Manager::Initialize D3D12SerializeRootSignature Failed");
			return false;
		}

		hr = Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(&RootSignature));
		if (FAILED(hr))
		{
			ErrorLog(RHILog, "FD3D12Manager::Initialize CreateRootSignature Failed");
			return false;
		}
	}

	{ // create vertex and pixel shader
		// when debugging, we can compile the shader files at runtime.
		// but for release version, we can compile the hlsl shaders with fxc.exe to create .cso files, which contain the shader bytecode.
		// we can load the .cso files at runtime to get the shader bytecode. which of course is faster than compiling them at runtime

		// compile vertex shader
		ID3DBlob *VertexShader = nullptr; // d3d blob for holding vertex shader bytecode
		ID3DBlob *ErrorBuffer = nullptr; // a buffer hold the error data if any

		hr = D3DCompileFromFile(L"VertexShader.hlsl", 
			nullptr, 
			nullptr, 
			"main", 
			"vs_5_0",
			D3DCOMPILE_DEBUG|D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&VertexShader,
			&ErrorBuffer);
		if (FAILED(hr))
		{
			if (ErrorBuffer)
			{
				ErrorLog(RHILog, "FD3D12Manager::Initialize D3DCompileFromFile[VertexShader.hlsl] Failed, error[%s]", (char*)ErrorBuffer->GetBufferPointer());
			}
			else
			{
				ErrorLog(RHILog, "FD3D12Manager::Initialize D3DCompileFromFile not find file:[VertexShader.hlsl]");
			}
			return false;
		}

		// fill out a shader bytecode structure, which is basically just a pointer
		// to the shader bytecode and the size of the shader bytecode
		D3D12_SHADER_BYTECODE VertexShaderBytecode = {};
		VertexShaderBytecode.BytecodeLength = VertexShader->GetBufferSize();
		VertexShaderBytecode.pShaderBytecode = VertexShader->GetBufferPointer();

		// compile the pixel shader
		ID3DBlob *PixelShader;
		hr = D3DCompileFromFile(L"PixelShader.hlsl",
			nullptr,
			nullptr,
			"main",
			"ps_5_0",
			D3DCOMPILE_DEBUG|D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&PixelShader,
			&ErrorBuffer);
		if (FAILED(hr))
		{
			if (ErrorBuffer)
			{
				ErrorLog(RHILog, "FD3D12Manager::Initialize D3DCompileFromFile[PixelShader.hlsl] Failed, error[%s]", (char*)ErrorBuffer->GetBufferPointer());
			}
			else
			{
				ErrorLog(RHILog, "FD3D12Manager::Initialize D3DCompileFromFile not find file:[PixelShader.hlsl]");
			}
			return false;
		}

		// fill out shader bytecode structure for pixel shader
		D3D12_SHADER_BYTECODE PixelShaderBytecode = {};
		PixelShaderBytecode.BytecodeLength = PixelShader->GetBufferSize();
		PixelShaderBytecode.pShaderBytecode = PixelShader->GetBufferPointer();

		// create input layout
		// the input layout is used by input assembler so that it knows how to read the vertex data bound to it
		D3D12_INPUT_ELEMENT_DESC InputLayout[] = 
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		D3D12_INPUT_LAYOUT_DESC InputLayoutDesc = {};
		InputLayoutDesc.NumElements = sizeof(InputLayout)/sizeof(D3D12_INPUT_ELEMENT_DESC);
		InputLayoutDesc.pInputElementDescs = InputLayout;

		// Create a pipeline state object(PSO)
		// in a real application, you will need many PSO
		// for each different shader or different combination of shaders, different blend or different rasterizer states,
		// different topology types(point, line, triangle, patch), or a different number of render targets you will need a pso

		// VS is the only required shader for a pos.
		D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc = {};
		PSODesc.InputLayout = InputLayoutDesc; // the structure describing our input layout
		PSODesc.pRootSignature = RootSignature; // the root signature that describes the input data this pos needs.
		PSODesc.VS = VertexShaderBytecode; // structure describing where to find the vertex shader bytecode and how large it is
		PSODesc.PS = PixelShaderBytecode; // same as pixel shader but for pixel shader
		PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
		PSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of render target
		PSODesc.SampleDesc = SampleDesc; // our multi-sampling desc
		PSODesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
		PSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state
		PSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blend state
		PSODesc.NumRenderTargets = 1; // we are only binding one render target

		// create the pso
		hr = Device->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(&PipelineStateObject));
		if (FAILED(hr))
		{
			ErrorLog(RHILog, "FD3D12Manager::Initialize CreateGraphicsPipelineState Failed, error[%s]", (char*)ErrorBuffer->GetBufferPointer());
			return false;
		}
	}

	{ // create vertex buffer

		// a triangle
		FVertex vList[] =
		{
			{0.0f, 0.5f, 0.5f, 1.f, 0.f, 0.f, 1.f},
			{0.5f, -0.5f, 0.5f, 0.f, 1.f, 0.f, 1.f},
			{-0.5f, -0.5f, 0.5f, 0.f, 0.f, 1.f, 1.f},
		};

		int32 vBufferSize = sizeof(vList);


		// create default heap
		// default heap is memory on the GPU. Only the GPU has access to this memory.
		// To Get data into this heap, we will have to upload the data using an upload heap
		Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
			D3D12_HEAP_FLAG_NONE, // no flag
			&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
			// we will start this heap in the copy destination state since we will copy data from the upload heap to this heap
			D3D12_RESOURCE_STATE_COPY_DEST, 
			nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
			IID_PPV_ARGS(&VertexBuffer)
		);

		// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
		VertexBuffer->SetName(L"Vertex Buffer Resource Heap");

		// create upload heap
		// upload heaps are used to upload data to the GPU. CPU can write to it and GPU can read from it
		// we will use Upload Heap to upload the data to Default Heap
		ID3D12Resource *vBufferUploadHeap;
		Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap,
			D3D12_HEAP_FLAG_NONE, // no flag
			&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
			D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy it`s content to default heap
			nullptr,
			IID_PPV_ARGS(&vBufferUploadHeap) // GPUwill read form this buffer and copy its contents to the default heap
		);

		vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");
		
		// store vertex buffer in upload heap
		D3D12_SUBRESOURCE_DATA VertexData = {};
		VertexData.pData = reinterpret_cast<BYTE*>(vList); // point to vertex array
		VertexData.RowPitch = vBufferSize; // size of our triangle vertex
		VertexData.SlicePitch = vBufferSize; // also size of our triangle vertex

		// now we create command with the command list to copy the data from Upload Heap to Default Heap
		UpdateSubresources(CommandList, VertexBuffer, vBufferUploadHeap, 0, 0, 1, &VertexData);

		// transition the vertex buffer data from copy destination state to vertex buffer state
		CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(VertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		// now we execute the command list to upload the initial assets (triangle data)
		CommandList->Close();
		ID3D12CommandList *ppCommandLists[] = { CommandList };
		CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);


		// increment the fence value now, otherwise the buffer might not be uploaded by the time we start drawing
		++FenceValue[FrameIndex];
		hr = CommandQueue->Signal(Fence[FrameIndex], FenceValue[FrameIndex]);
		if (FAILED(hr))
		{
			BreakLog(RHILog, "FD3D12Manager::Initialize  CommandQueue->Signal failed, frameIndex[%d]", FrameIndex);
			return false;
		}

		// create a vertex buffer view for the triangle. we get the GPU memory address to the vertex pointer using GetGPUVirtualAddress() method
		VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
		VertexBufferView.StrideInBytes = sizeof(FVertex);
		VertexBufferView.SizeInBytes = vBufferSize;

		// fill out the viewport
		Viewport.TopLeftX = 0;
		Viewport.TopLeftY = 0;
		Viewport.Width = Width;
		Viewport.Height = Height;
		Viewport.MinDepth = 0.f;
		Viewport.MaxDepth = 1.f;

		// fill out the scissor rect
		ScissorRect.left = 0;
		ScissorRect.top = 0;
		ScissorRect.right = Width;
		ScissorRect.bottom = Height;

	}

	DebugLog(RHILog, "FD3D12Manager::Initialize Succeed");
	return true;
}

void FD3D12Manager::UpdatePipeline()
{
	// we have to wait for the GPU to finish with the command allocator before we reset it
	WaitPreviousFrame();

	HRESULT hr;

	// clean the allocator content, and not change it size
	// so we can re-use it as new and don`t reallocate the memory
	hr = CommandAllocator[FrameIndex]->Reset();
	if (FAILED(hr))
	{
		BreakLog(RHILog, "FD3D12Manager::UpdatePipeline CommandAllocator Reset failed");
		GIsRequestingExit = true;
	}

	hr = CommandList->Reset(CommandAllocator[FrameIndex], PipelineStateObject);
	if (FAILED(hr))
	{
		BreakLog(RHILog, "FD3D12Manager::UpdatePipeline CommandList Reset failed");
		GIsRequestingExit = true;
	}

	// transition the "frameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	D3D12_RESOURCE_BARRIER ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[FrameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CommandList->ResourceBarrier(1, &ResourceBarrier);

	// here we again get the handle to our current render target view so we can set it as the render target in the output merge stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTargetViewHandle(RenderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), FrameIndex, RenderTargetViewDescriptorSize);

	// set the render target for the output merge stage (the output of the pipeline)
	CommandList->OMSetRenderTargets(1, &RenderTargetViewHandle, FALSE, nullptr);

	// clear the render target by using the ClearRenderTargetView command
	const float ClearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
	CommandList->ClearRenderTargetView(RenderTargetViewHandle, ClearColor, 0, nullptr);

	// draw triangle
	CommandList->SetGraphicsRootSignature(RootSignature);
	CommandList->RSSetViewports(1, &Viewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->IASetVertexBuffers(0, 1, &VertexBufferView); // set the vertex buffer(using the buffer view)
	CommandList->DrawInstanced(3, 1, 0, 0);

	// transition the "FrameIndex" render target from the render target state to the present state.
	// if the debug layer is enabled, you will receive a warning if present is called on the render target 
	// when it is not in the present state
	// 如果在present时候, 传入一个没在 present state 的 render target, 会报警告
	D3D12_RESOURCE_BARRIER ResourceBarrier1 = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[FrameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	CommandList->ResourceBarrier(1, &ResourceBarrier1);



	hr = CommandList->Close();
	if (FAILED(hr))
	{
		BreakLog(RHILog, "FD3D12Manager::UpdatePipeline CommandList ResourceBarrier failed");
		GIsRequestingExit = true;
	}
}

void FD3D12Manager::Render()
{
	HRESULT hr;

	// update the pipeline by sending commands to the command queue
	UpdatePipeline();

	// create an array of command list (only one command list here)
	ID3D12CommandList *ppCommandList[] = {CommandList};

	// 表明cpu端已经填充完成Commandlist, 通知GPU端执行
	CommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

	// this command goes in at the end of our command queue.
	// we will know when our command queue has finished because the fence value will be set to "FenceValue" from the GPU 
	// since the queue is being executed on the GPU
	// 向GPU端添加一条指令, 执行设定FenceValue的值为FenceValue[FrameIndex]
	hr = CommandQueue->Signal(Fence[FrameIndex], FenceValue[FrameIndex]);
	if (FAILED(hr))
	{
		BreakLog(RHILog, "FD3D12Manager::Render CommandQueue Signal failed");
		GIsRequestingExit = true;
	}

	// present the current buffer
	hr = SwapChain->Present(0, 0);
	if (FAILED(hr))
	{
		BreakLog(RHILog, "FD3D12Manager::Render SwapChain Present failed");
		GIsRequestingExit = true;
	}
}

void FD3D12Manager::Cleanup()
{
	WaitPreviousFrame();

	// get swap chain out of full screen before exiting 
	BOOL bFullScreen = false;
	if (SwapChain->GetFullscreenState(&bFullScreen, NULL))
	{
		SwapChain->SetFullscreenState(false, nullptr);
	}

	SAFE_RELEASE(Device);
	SAFE_RELEASE(SwapChain);
	SAFE_RELEASE(CommandQueue);
	SAFE_RELEASE(RenderTargetViewDescriptorHeap);
	SAFE_RELEASE(CommandList);
	
	for (int32 i=0; i<FrameBufferCount; ++i)
	{
		SAFE_RELEASE(RenderTargets[i]);
		SAFE_RELEASE(CommandAllocator[i]);
		SAFE_RELEASE(Fence[i]);
	}

	SAFE_RELEASE(PipelineStateObject);
	SAFE_RELEASE(RootSignature);
	SAFE_RELEASE(VertexBuffer);

	CloseHandle(FenceEvent);
}

void FD3D12Manager::WaitPreviousFrame()
{
	// if the current fence value is less than "FenceValue", then we know the GPU has not finished Executing.
	// the command queue since it has not reached the "commandQueue->Signal(Fence, FenceValue)" command
	UINT64 CurrentCompleteValue = Fence[FrameIndex]->GetCompletedValue();
	if (CurrentCompleteValue<FenceValue[FrameIndex])
	{
		// we have the fence create an event which is signaled once the fence`s current value is "FenceValue"
		HRESULT hr = Fence[FrameIndex]->SetEventOnCompletion(FenceValue[FrameIndex], FenceEvent);
		if (FAILED(hr))
		{
			BreakLog(RHILog, "FD3D12Manager::WaitPreviousFrame SetEventOnCompletion for FrameIndex[%d] Error", FrameIndex);
			GIsRequestingExit = true;
		}

		// we will wait until the fence has triggered the event that it`s current value has reached "FenceValue"
		// once it`s value has reached "FenceValue", we know the command queue has finished executing
		WaitForSingleObject(FenceEvent, INFINITE);
	}

	{ // Render new frame
	// increase fence value for next frame
		++FenceValue[FrameIndex];
		FrameIndex = SwapChain->GetCurrentBackBufferIndex();
	}
}
