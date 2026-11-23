#pragma once
#include "Api.h"
#include <cstdint>
#include <string>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

namespace rd
{
	class RD_API Window
	{
	public:
		bool Initialize(uint32_t clientWidth, uint32_t clientHeight, const std::string& title);
		void Terminate();

		bool ProcessMessage();

		void SetTitle(const std::string& title);
		void SetFullscreen(bool isFullscreen);

		HWND GetHWnd() const { return mHWnd; }
		HINSTANCE GetHInstance() const { return mHInstance; }
		uint32_t GetClientWidth() const { return mClientWidth; }
		uint32_t GetClientHeight() const { return mClientHeight; }
		bool IsFullscreen() const { return mIsFullscreen; }

	private:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		bool LoadPlacement();
		void SavePlacement();

		HWND mHWnd = nullptr;
		HINSTANCE mHInstance = nullptr;
		uint32_t mClientWidth = 0;
		uint32_t mClientHeight = 0;
		bool mIsFullscreen = false;

		WINDOWPLACEMENT mSavedPlacement = {};
	};
}
