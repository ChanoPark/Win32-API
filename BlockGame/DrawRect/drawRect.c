#include <windows.h>
#include <TCHAR.h>
#include <stdio.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
POINT CenterPoint(RECT);

HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = _T("Draw Rect");

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance
	, _In_ LPSTR lpszCmdParam, _In_ int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		400, 200, 500, 800,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	hWndMain = hWnd; // hWnd 정보도 전역변수에 저장!	

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage,
	WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	POINT p;

	static RECT barR, clientR;
	static int alphaX;
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;

	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hWnd, &clientR);
		p = CenterPoint(clientR);

		SetRect(&barR, p.x - 50, p.y - 15, p.x + 50, p.y + 15);
		OffsetRect(&barR, 0, 300);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			alphaX = -1;
			OffsetRect(&barR, alphaX * 2, 0);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_RIGHT:
			alphaX = 1;
			OffsetRect(&barR, alphaX * 2, 0);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
		hOldPen = (HPEN)SelectObject(hdc, hPen);
		hBrush = CreateSolidBrush(RGB(0, 255, 0));
		hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		Rectangle(hdc, barR.left, barR.top, barR.right, barR.bottom);

		SelectObject(hdc, hOldPen);
		SelectObject(hdc, hOldBrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

POINT CenterPoint(RECT r)
{
	POINT p;
	p.x = (r.left + r.right) / 2;
	p.y = (r.top + r.bottom) / 2;
	return p;
}