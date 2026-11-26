#pragma once
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

namespace rd
{
	class Window;

	/// <summary>
	/// DirectX12の基盤
	/// </summary>
	class GraphicsDevice
	{
	public:
		bool Initialize(const Window& window);
		void Shutdown();

		void BeginFrame();
		void EndFrame();

		bool Resize(uint32_t width, uint32_t height);

		ID3D12Device* GetDevice() const { return mDevice.Get(); }
		ID3D12GraphicsCommandList* GetCommandList() const { return mCommandList.Get(); }

	private:
		static constexpr uint32_t kBackBufferCount = 2;

		bool CreateDevice();
		bool CreateCommands();
		bool CreateSwapChain(const Window& window);
		bool CreateRtvHeap();
		bool CreateRenderTargets();
		bool CreateFence();

		void WaitForGpu();

		D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle(uint32_t index) const;

		Microsoft::WRL::ComPtr<IDXGIFactory7> mFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
		Microsoft::WRL::ComPtr<ID3D12Resource> mBackBuffers[kBackBufferCount];
		uint32_t mBackBufferIndex = 0;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
		uint32_t mRtvDescriptorSize = 0;

		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		uint64_t mFenceValue = 0;
		HANDLE mFenceEvent = nullptr;
	};
}
