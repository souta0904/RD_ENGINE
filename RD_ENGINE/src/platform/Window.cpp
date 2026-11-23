#include "platform/Window.h"
#include "core/Log.h"
#include "utility/JsonUtility.h"
#include "utility/StringUtility.h"

namespace rd
{
	namespace
	{
		constexpr const wchar_t* kClassName = L"RdWindowClass";
		constexpr const char* kPlacementPath = "saved/Window.json";

		// リピート入力か
		bool IsKeyRepeat(LPARAM lParam)
		{
			return (lParam & (1 << 30)) != 0;
		}
	}

	bool Window::Initialize(uint32_t clientWidth, uint32_t clientHeight, const std::string& title)
	{
		// DPIスケーリングに対応
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		mHInstance = GetModuleHandleW(nullptr);

		WNDCLASSEXW wc = {};
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.lpfnWndProc = WndProc;
		wc.hInstance = mHInstance;
		wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		wc.lpszClassName = kClassName;
		if (!RegisterClassExW(&wc))
		{
			Log::Error("Window: RegisterClassExW failed: {}", GetLastError());
			return false;
		}

		RECT rect = { 0, 0, LONG(clientWidth), LONG(clientHeight) };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		// ウィンドウを作成
		std::wstring wideTitle = StringUtility::ToWide(title);
		mHWnd = CreateWindowExW(
			0,
			kClassName,
			wideTitle.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			nullptr,
			nullptr,
			mHInstance,
			this);
		if (!mHWnd)
		{
			Log::Error("Window: CreateWindowExW failed: {}", GetLastError());
			UnregisterClassW(kClassName, mHInstance);
			return false;
		}

		if (!LoadPlacement())
		{
			ShowWindow(mHWnd, SW_SHOW);
		}

		// クライアント領域のサイズを取得
		RECT client = {};
		GetClientRect(mHWnd, &client);
		mClientWidth = uint32_t(client.right - client.left);
		mClientHeight = uint32_t(client.bottom - client.top);

		Log::Info("Window: Created ({}x{})", mClientWidth, mClientHeight);
		return true;
	}

	void Window::Terminate()
	{
		if (mHWnd)
		{
			DestroyWindow(mHWnd);
		}

		UnregisterClassW(kClassName, mHInstance);
	}

	bool Window::ProcessMessage()
	{
		MSG msg = {};
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) return true;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		return false;
	}

	void Window::SetTitle(const std::string& title)
	{
		SetWindowTextW(mHWnd, StringUtility::ToWide(title).c_str());
	}

	void Window::SetFullscreen(bool isFullscreen)
	{
		if (isFullscreen == mIsFullscreen) return;

		if (isFullscreen)
		{
			// 現在の配置状態を保存
			mSavedPlacement.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(mHWnd, &mSavedPlacement);

			// モニター情報を取得
			MONITORINFO mi = { sizeof(MONITORINFO) };
			GetMonitorInfoW(MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST), &mi);

			// フルスクリーン化
			SetWindowLongPtrW(mHWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
			SetWindowPos(mHWnd, HWND_TOP,
				mi.rcMonitor.left,
				mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_FRAMECHANGED);
		}
		else
		{
			// 復元
			SetWindowLongPtrW(mHWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
			SetWindowPlacement(mHWnd, &mSavedPlacement);
			SetWindowPos(mHWnd, nullptr, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}

		mIsFullscreen = isFullscreen;
	}

	// Windows APIとWindowオブジェクトの橋渡し
	LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_NCCREATE)
		{
			// ウィンドウとWindowオブジェクトを紐づけ
			auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
			auto* window = static_cast<Window*>(create->lpCreateParams);
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			window->mHWnd = hWnd;
		}

		auto* window = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
		if (window) return window->HandleMessage(msg, wParam, lParam);

		return DefWindowProcW(hWnd, msg, wParam, lParam);
	}

	LRESULT Window::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_SIZE:
			if (wParam != SIZE_MINIMIZED)
			{
				mClientWidth = LOWORD(lParam);
				mClientHeight = HIWORD(lParam);
			}
			return 0;

		case WM_KEYDOWN:
			// F11でフルスクリーン
			if (wParam == VK_F11 && !IsKeyRepeat(lParam))
			{
				SetFullscreen(!mIsFullscreen);
				return 0;
			}
			break;

		case WM_SYSKEYDOWN:
			// Alt+Enterでフルスクリーン
			if (wParam == VK_RETURN && (lParam & (1 << 29)) && !IsKeyRepeat(lParam))
			{
				SetFullscreen(!mIsFullscreen);
				return 0;
			}
			break;

		case WM_DPICHANGED:
			if (!mIsFullscreen)
			{
				const RECT* rect = reinterpret_cast<const RECT*>(lParam);// 推奨サイズ
				SetWindowPos(mHWnd, nullptr,
					rect->left,
					rect->top,
					rect->right - rect->left,
					rect->bottom - rect->top,
					SWP_NOZORDER | SWP_NOACTIVATE);
			}
			return 0;

		case WM_DESTROY:
			SavePlacement();
			PostQuitMessage(0);
			return 0;

		case WM_NCDESTROY:
		{
			HWND hWnd = mHWnd;
			mHWnd = nullptr;
			// WM_NCCREATEで紐づけたポインタを削除
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
			return DefWindowProcW(hWnd, msg, wParam, lParam);
		}
		}

		return DefWindowProcW(mHWnd, msg, wParam, lParam);
	}

	bool Window::LoadPlacement()
	{
		Json json;
		if (!JsonUtility::LoadFile(kPlacementPath, json)) return false;

		int32_t x = 0;
		int32_t y = 0;
		int32_t width = 0;
		int32_t height = 0;
		bool isMaximized = false;
		bool isFullscreen = false;
		bool isOk =
			JsonUtility::GetInt(json, "x", x) &&
			JsonUtility::GetInt(json, "y", y) &&
			JsonUtility::GetInt(json, "width", width) &&
			JsonUtility::GetInt(json, "height", height) &&
			JsonUtility::GetBool(json, "isMaximized", isMaximized) &&
			JsonUtility::GetBool(json, "isFullscreen", isFullscreen);
		if (!isOk || width <= 0 || height <= 0)
		{
			Log::Warning("Window: Invalid placement in {}", kPlacementPath);
			return false;
		}

		// 読み込んだ値を反映
		WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
		wp.rcNormalPosition = { x, y, x + width, y + height };
		wp.showCmd = isMaximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL;
		SetWindowPlacement(mHWnd, &wp);
		if (isFullscreen)
		{
			SetFullscreen(true);
		}

		return true;
	}

	void Window::SavePlacement()
	{
		WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
		if (mIsFullscreen)
		{
			// フルスクリーンの場合はフルスクリーン前の値
			wp = mSavedPlacement;
		}
		else if (!GetWindowPlacement(mHWnd, &wp))
		{
			return;
		}

		const RECT& rect = wp.rcNormalPosition;
		Json json;
		json["x"] = rect.left;
		json["y"] = rect.top;
		json["width"] = rect.right - rect.left;
		json["height"] = rect.bottom - rect.top;
		json["isMaximized"] = (wp.showCmd == SW_SHOWMAXIMIZED);
		json["isFullscreen"] = mIsFullscreen;
		JsonUtility::SaveFile(kPlacementPath, json);
	}
}
