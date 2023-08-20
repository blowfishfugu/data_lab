#pragma once
#include "AsyncQueue.h"
#include <tuple>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <Windows.h>


struct Pixel
{
	COLORREF color{};
	__int64 brightness{};
};

using coord = std::tuple<int, int>;
using PixelFunc = std::function<void(Pixel&)>;
struct RectData
{
	PixelFunc command{}; //on/off/toggle
	coord topLeft{};
	coord bottomRight{};
};

struct window_class
{
	WNDCLASSEX wc;
	window_class(WNDPROC defaultProc)
	{
		static bool called_once = false;
		if (!called_once)
		{
			called_once = true;
			ZeroMemory(&wc, sizeof(wc));
			wc.cbSize = sizeof(wc);
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = defaultProc;
			wc.hInstance = GetModuleHandle(NULL);
			wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
			wc.lpszClassName = "gdi_window";
			RegisterClassEx(&wc);
		}
	}
	
};

struct gdi_window
{
	AsyncQueue<RectData>& input;
	std::thread windowThread;
	std::atomic_bool stop{ true };
	HWND wnd{};
	int width{};
	int height{};
	std::vector<Pixel> pixels;
	static LRESULT CALLBACK gdi_wnd_proc(HWND, UINT, WPARAM, LPARAM);
	window_class _wc{ gdi_wnd_proc };
	void main_loop();//eigentliche erstellung, danach dispatching
	void paint(HDC dc);

	gdi_window( AsyncQueue<RectData>& in, int w, int h ):
		input(in),width(w),height(h)
	{
		pixels.resize(width * height);
	}
	
	void start_window(); //hiernach in eigenem thread, WM_CREATE+msgqueue in eigenem loop
	void trigger_update(); //input updated, von aussen callbar, "swap-buffer"
	void wait_for_close(); //wait for WM_QUIT
	void close_window(); //Send WM_DESTROY
};


