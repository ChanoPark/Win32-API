#include <windows.h>
#include <TCHAR.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
POINT CenterPoint(RECT r);
int CheckStrikeX(RECT r, RECT bound);
void DrawObject(HDC, RECT, COLORREF, int);
void DrawObject(HDC, RECT, COLORREF, COLORREF, int);


HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = _T("AutoMove");

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance
	, _In_ LPSTR lpszCmdParam, _In_ int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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
	static RECT barR, clientR;
	static int alphaX;
	static COLORREF barColor;
	POINT p;

	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hWnd, &clientR);
		p = CenterPoint(clientR);
		SetRect(&barR, p.x - 50, p.y - 15, p.x + 50, p.y + 15);
		OffsetRect(&barR, 0, 200);
		alphaX = 1;
		barColor = RGB(0, 255, 0);
		SetTimer(hWnd, 1, 10, NULL);
		return 0;
	case WM_TIMER:
		if (CheckStrikeX(barR, clientR))
			alphaX *= -1;

		OffsetRect(&barR, 3 * alphaX, 0);
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawObject(hdc, barR, barColor, 0);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

POINT CenterPoint(RECT r) {
	POINT p;
	p.x = (r.left + r.right) / 2;
	p.y = (r.top + r.bottom) / 2;
	return p;
}

int CheckStrikeX(RECT r, RECT bound) {
	if (r.left <= bound.left || r.right >= bound.right)
		return 1;
	else
		return 0;
}

void DrawObject(HDC hdc, RECT r, COLORREF color, int type) {
	DrawObject(hdc, r, color, color, type);
}

void DrawObject(HDC hdc, RECT r, COLORREF penC, COLORREF brushC, int type) {
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
	SelectObject(hdc, hOldPen); Select(hdc, hOldBrush);
	DeleteObject(hPen); DeleteObject(hBrush);
}