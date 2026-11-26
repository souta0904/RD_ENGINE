#include "rhi/device/GraphicsDevice.h"
#include "core/Log.h"
#include "core/utility/StringUtility.h"
#include "platform/Window.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace rd
{
	namespace
	{
		constexpr float kClearColor[4] = { 0.1f, 0.25f, 0.5f, 1.0f };

		bool Check(HRESULT hr, const char* what)
		{
			if (SUCCEEDED(hr)) return true;

			Log::Error("GraphicsDevice: {} failed: 0x{:08X}", what, static_cast<uint32_t>(hr));
			return false;
		}

		// バリアを作成するヘルパー関数
		D3D12_RESOURCE_BARRIER MakeTransition(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
		{
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = resource;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = before;
			barrier.Transition.StateAfter = after;
			return barrier;
		}
	}

	bool GraphicsDevice::Initialize(const Window& window)
	{
		if (!CreateDevice()) return false;
		if (!CreateCommands()) return false;
		if (!CreateSwapChain(window)) return false;
		if (!CreateRtvHeap()) return false;
		if (!CreateRenderTargets()) return false;
		if (!CreateFence()) return false;

		Log::Info("GraphicsDevice: Initialized");
		return true;
	}

	void GraphicsDevice::Shutdown()
	{
		if (mFenceEvent)
		{
			WaitForGpu();
			CloseHandle(mFenceEvent);
			mFenceEvent = nullptr;
		}

		mFence.Reset();
		mRtvHeap.Reset();
		for (auto& backBuffer : mBackBuffers)
		{
			backBuffer.Reset();
		}
		mSwapChain.Reset();
		mCommandList.Reset();
		mCommandAllocator.Reset();
		mCommandQueue.Reset();
		mDevice.Reset();
		mFactory.Reset();
	}

	void GraphicsDevice::BeginFrame()
	{
		mCommandAllocator->Reset();
		mCommandList->Reset(mCommandAllocator.Get(), nullptr);

		mBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
		ID3D12Resource* backBuffer = mBackBuffers[mBackBufferIndex].Get();

		D3D12_RESOURCE_BARRIER barrier = MakeTransition(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mCommandList->ResourceBarrier(1, &barrier);

		D3D12_CPU_DESCRIPTOR_HANDLE rtv = GetRtvHandle(mBackBufferIndex);
		mCommandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
		mCommandList->ClearRenderTargetView(rtv, kClearColor, 0, nullptr);
	}

	void GraphicsDevice::EndFrame()
	{
		D3D12_RESOURCE_BARRIER barrier = MakeTransition(mBackBuffers[mBackBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		mCommandList->ResourceBarrier(1, &barrier);

		mCommandList->Close();
		ID3D12CommandList* lists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(1, lists);

		mSwapChain->Present(1, 0);

		WaitForGpu();
	}

	bool GraphicsDevice::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0) return true;

		// 使い終わるまで待つ
		WaitForGpu();

		for (auto& backBuffer : mBackBuffers)
		{
			backBuffer.Reset();
		}

		HRESULT hr = mSwapChain->ResizeBuffers(kBackBufferCount, width, height, DXGI_FORMAT_UNKNOWN, 0);
		if (!Check(hr, "ResizeBuffers")) return false;

		if (!CreateRenderTargets()) return false;

		Log::Info("GraphicsDevice: Resized ({}x{})", width, height);
		return true;
	}

	bool GraphicsDevice::CreateDevice()
	{
		UINT factoryFlags = 0;

#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> debug;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
		{
			// デバッグレイヤーを有効にする
			debug->EnableDebugLayer();
		}
		factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		HRESULT hr = CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&mFactory));
		if (!Check(hr, "CreateDXGIFactory2")) return false;

		// 高性能なアダプタから試す
		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
		for (UINT i = 0;
			mFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
			++i)
		{
			DXGI_ADAPTER_DESC1 desc = {};
			adapter->GetDesc1(&desc);

			// ソフトウェアアダプタは使わない
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

			hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mDevice));
			if (SUCCEEDED(hr))
			{
				Log::Info("GraphicsDevice: Adapter {}", StringUtility::ToUtf8(desc.Description));
				break;
			}
		}

		if (!mDevice)
		{
			Log::Error("GraphicsDevice: No DX12 adapter found");
			return false;
		}

		return true;
	}

	bool GraphicsDevice::CreateCommands()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		HRESULT hr = mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
		if (!Check(hr, "CreateCommandQueue")) return false;

		hr = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
		if (!Check(hr, "CreateCommandAllocator")) return false;

		hr = mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mCommandList));
		if (!Check(hr, "CreateCommandList")) return false;

		mCommandList->Close();// 閉じておく
		return true;
	}

	bool GraphicsDevice::CreateSwapChain(const Window& window)
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = window.GetClientWidth();
		desc.Height = window.GetClientHeight();
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = kBackBufferCount;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		HRESULT hr = mFactory->CreateSwapChainForHwnd(mCommandQueue.Get(), window.GetHWnd(), &desc, nullptr, nullptr, &swapChain1);
		if (!Check(hr, "CreateSwapChainForHwnd")) return false;

		hr = swapChain1.As(&mSwapChain);
		if (!Check(hr, "QueryInterface(IDXGISwapChain4)")) return false;

		// Alt+EnterはWindowで処理する
		mFactory->MakeWindowAssociation(window.GetHWnd(), DXGI_MWA_NO_ALT_ENTER);
		return true;
	}

	bool GraphicsDevice::CreateRtvHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = kBackBufferCount;
		HRESULT hr = mDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mRtvHeap));
		if (!Check(hr, "CreateDescriptorHeap")) return false;

		mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		return true;
	}

	bool GraphicsDevice::CreateRenderTargets()
	{
		for (uint32_t i = 0; i < kBackBufferCount; ++i)
		{
			HRESULT hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mBackBuffers[i]));
			if (!Check(hr, "GetBuffer")) return false;

			mDevice->CreateRenderTargetView(mBackBuffers[i].Get(), nullptr, GetRtvHandle(i));
		}

		return true;
	}

	bool GraphicsDevice::CreateFence()
	{
		HRESULT hr = mDevice->CreateFence(mFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
		if (!Check(hr, "CreateFence")) return false;

		mFenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (!mFenceEvent)
		{
			Log::Error("GraphicsDevice: CreateEventW failed: {}", GetLastError());
			return false;
		}

		return true;
	}

	void GraphicsDevice::WaitForGpu()
	{
		++mFenceValue;
		mCommandQueue->Signal(mFence.Get(), mFenceValue);

		if (mFence->GetCompletedValue() < mFenceValue)
		{
			mFence->SetEventOnCompletion(mFenceValue, mFenceEvent);
			WaitForSingleObject(mFenceEvent, INFINITE);
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDevice::GetRtvHandle(uint32_t index) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = mRtvHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += static_cast<SIZE_T>(index) * mRtvDescriptorSize;
		return handle;
	}
}
