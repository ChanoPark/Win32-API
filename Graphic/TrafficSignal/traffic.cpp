#include <windows.h>
#include <tchar.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
POINT CenterPoint(RECT& r);
void DrawObject(HDC, RECT&, COLORREF, int);
void DrawObject(HDC, RECT&, COLORREF, COLORREF, int);

HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = _T("Traffic Signal");

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
		450, 200, 125, 360,
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

	static RECT lampR[3];

	static COLORREF lampBackCo[3] = { RGB(100, 0, 0), RGB(100, 100, 0), RGB(0, 100, 0) };
	static COLORREF lampForeCo[3] = { RGB(255, 0, 0), RGB(255, 255, 0), RGB(0, 255, 0) };

	static int lampFlag;
	int i;
	int offset;
	RECT clientR;

	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hWnd, &clientR);
		InflateRect(&clientR, -10, -10);
		clientR.bottom /= 3;
		offset = clientR.bottom - clientR.top + 10;

		for (i = 0; i < 3; i++) {
			lampR[i] = clientR;
			OffsetRect(&clientR, 0, offset);
		}

		lampFlag = 0;
		SetTimer(hWnd, 1, 1000, NULL);
		return 0;
	case WM_TIMER:
		lampFlag--;
		if (lampFlag < 0)
			lampFlag = 2;

		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		for (i = 0; i < 3; i++) {
			DrawObject(hdc, lampR[i], lampBackCo[i], 1);
		}

		DrawObject(hdc, lampR[lampFlag], lampForeCo[lampFlag], 1);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
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
//temp 문자열 길이 구하기
// num = _tcslen(temp);


// 문자열에 쓰기 1
// #include <stdio.h> 파일이 필요함
// _stprintf_s(temp, _T("Cown Down: %2d"), Count);


// 문자열에 쓰기 2
// _tcscpy_s(temp, _T("호환성 지원"));