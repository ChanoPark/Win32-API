#include <windows.h>
#include <tchar.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
POINT CenterPoint(RECT& r);
void DrawObject(HDC, RECT&, COLORREF, int);
void DrawObject(HDC, RECT&, COLORREF, COLORREF, int);

HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = _T("DragCircle");

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
		400, 300, 600, 500,
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

	static RECT ballR;
	static COLORREF ballCol;
	static BOOL onFlag;

	static POINT p;
	POINT q;
	RECT clientR;

	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hWnd, &clientR);
		p = CenterPoint(clientR);

		SetRect(&ballR, p.x - 15, p.y - 15, p.x + 15, p.y + 15);
		ballCol = RGB(255, 0, 0);
		onFlag = FALSE;
		return 0;
	case WM_LBUTTONDOWN:
		p.x = LOWORD(lParam);
		p.y = HIWORD(lParam);

		if (PtInRect(&ballR, p)) {
			onFlag = TRUE;
			ballCol = RGB(255, 255, 0);
		}
		return 0;
	case WM_MOUSEMOVE:
		if (onFlag) {
			q.x = LOWORD(lParam);
			q.y = HIWORD(lParam);
			p = q;
			InvalidateRect(hWnd, NULL, TRUE);
		}
	case WM_LBUTTONUP:
		if (onFlag) {
			onFlag = FALSE;
			ballCol = RGB(255, 0, 0);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		SetRect(&ballR, p.x - 15, p.y - 15, p.x + 15, p.y + 15);
		DrawObject(hdc, ballR, ballCol, 1);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
POINT CenterPoint(RECT& r) {
	POINT p;
	p.x = (r.left + r.right) / 2;
	p.y = (r.top + r.bottom) / 2;
	return p;
}

void DrawObject(HDC hdc, RECT& r, COLORREF color, int type) {
	DrawObject(hdc, r, color, color, type);
}

void DrawObject(HDC hdc, RECT& r, COLORREF penC, COLORREF brushC, int type) {
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	hPen = CreatePen(PS_SOLID, 1, penC);
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	hBrush = CreateSolidBrush(brushC);
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	switch (type) {
	case 0:
		Rectangle(hdc, r.left, r.top, r.right, r.bottom);
		break;
	case 1:
		Ellipse(hdc, r.left, r.top, r.right, r.bottom);
		break;
	}

	SelectObject(hdc, hOldPen); SelectObject(hdc, hOldBrush);
	DeleteObject(hPen); DeleteObject(hBrush);
}