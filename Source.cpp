#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");

#define WIDTH  512
#define HEIGHT  512

#define _USE_MATH_DEFINES
#include <math.h>

void DrawKoch(HDC hdc, const POINT* a, const POINT* b, int n)
{
	POINT e;
	const POINT c = { (2 * a->x + b->x) / 3,(2 * a->y + b->y) / 3 };
	const POINT d = { (a->x + 2 * b->x) / 3,(a->y + 2 * b->y) / 3 };
	const int x = b->x - a->x;
	const int y = -(b->y - a->y);
	const double distance = sqrt(x*x + y*y) / sqrt(3);
	if (x >= 0)
	{
		const double angle = atan((double)y / x) + M_PI / 6;
		e.x = a->x + (int)(distance*cos(angle));
		e.y = a->y - (int)(distance*sin(angle));
	}
	else
	{
		const double angle = atan((double)y / x) - M_PI / 6;
		e.x = b->x + (int)(distance*cos(angle));
		e.y = b->y - (int)(distance*sin(angle));
	}
	if (n <= 0)
	{
		MoveToEx(hdc, a->x, a->y, 0);
		LineTo(hdc, c.x, c.y);
		MoveToEx(hdc, c.x, c.y, 0);
		LineTo(hdc, e.x, e.y);
		MoveToEx(hdc, e.x, e.y, 0);
		LineTo(hdc, d.x, d.y);
		MoveToEx(hdc, d.x, d.y, 0);
		LineTo(hdc, b->x, b->y);
	}
	else
	{
		DrawKoch(hdc, a, &c, n - 1);
		DrawKoch(hdc, &c, &e, n - 1);
		DrawKoch(hdc, &e, &d, n - 1);
		DrawKoch(hdc, &d, b, n - 1);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	static HBITMAP hBitmap;
	static HDC hdcMem;
	switch (msg)
	{
	case WM_CREATE:
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("コッホ曲線を描画"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hButton, 10, 10, 256, 32, TRUE);
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rect;
			GetClientRect(hWnd, &rect);
			if (hdcMem)
			{
				BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
			}
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_APP:
		{
			if (hdcMem)
			{
				DeleteDC(hdcMem);
				hdcMem = NULL;
				DeleteObject(hBitmap);
				hBitmap = NULL;
			}
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			SendMessage(hWnd, WM_APP, 0, 0);
			const HDC hdc = GetDC(hWnd);
			hdcMem = CreateCompatibleDC(hdc);
			hBitmap = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);
			SelectObject(hdcMem, hBitmap);
			ReleaseDC(hWnd, hdc);

			PatBlt(hdcMem, 0, 0, WIDTH, HEIGHT, WHITENESS);

			POINT P = { 100,160 };
			POINT Q = { 400,160 };
			POINT R = { 250,420 };
			DrawKoch(hdcMem, &P, &Q, 7);
			DrawKoch(hdcMem, &Q, &R, 7);
			DrawKoch(hdcMem, &R, &P, 7);

			InvalidateRect(hWnd, 0, 0);
		}
		break;
	case WM_DESTROY:
		SendMessage(hWnd, WM_APP, 0, 0);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Window"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
