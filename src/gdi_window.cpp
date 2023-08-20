#include "gdi_window.h"

LRESULT gdi_window::gdi_wnd_proc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{
			CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
			gdi_window* self = (gdi_window*)cs->lpCreateParams;
			SetProp(wnd, "gdi_window", self);
		}
		break;
		case WM_PAINT:
		{
			gdi_window* self = (gdi_window*)GetProp(wnd, "gdi_window");
			if (!self->input.empty())
			{
				HDC dc = GetDC(wnd);
				self->paint(dc);
				ReleaseDC(wnd, dc);
			}
			return DefWindowProc(wnd, message, wParam, lParam);
		}break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			gdi_window* self = (gdi_window*)GetProp(wnd, "gdi_window");
			self->stop = true;
			return 0;
		} break;
	}
	return DefWindowProc(wnd, message, wParam, lParam);
}

void gdi_window::start_window()
{
	stop = false;
	this->windowThread=std::thread{ [&]() { main_loop(); } };
}



void gdi_window::main_loop()
{
	//init_gdiplus?

	this->wnd=CreateWindowEx(NULL,
		_wc.wc.lpszClassName, "Pixels",	WS_OVERLAPPEDWINDOW,
		0,0,
		width,height,
		NULL,NULL,
		_wc.wc.hInstance,
		(LPVOID)this //userparam
	);
	::ShowWindow(wnd, SW_SHOW);

	while (!stop)
	{
		MSG msg;
		while (PeekMessage(&msg, this->wnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			stop = true;
		}
	}


}

void gdi_window::paint(HDC dc)
{
	int x = 0;
	int y = 0;
	for (const Pixel& p : pixels)
	{
		if (x > width) { x = 0; ++y; }
		if (y > height) { y = 0; }
		::SetPixel(dc, x, y, p.color);
		++x;
	}
}

void gdi_window::trigger_update()
{
	PostMessage(this->wnd, WM_PAINT, 0, 0); //<-oder eigene WM_USER+xy-Message?
}

void gdi_window::wait_for_close()
{
	this->windowThread.join();
}

void gdi_window::close_window()
{
	PostMessage(this->wnd, WM_DESTROY, 0, 0);
	stop = true;
	wait_for_close();
}
