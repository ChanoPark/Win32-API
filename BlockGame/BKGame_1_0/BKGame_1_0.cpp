#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <MMSystem.h>

#define WM_USER 0x0400
#define UM_GAMEOVER WM_USER+4

static int m_Patterns[5][5] = { {1,1,1,1,1,},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1,},{1,1,1,1,1} };

void InitBlocks(RECT blocks[][5], RECT& boound);
void DrawBlocks(HDC hdc, RECT blocks[][5]);
int CheckGameOver(RECT& rect, RECT& bar);
int HitTest(HDC hdc, RECT& ball, RECT block[][5]);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
POINT CenterPoint(RECT& r);

void DrawObject(HDC, RECT&, COLORREF, int, int ropCode = R2_XORPEN);
void DrawObject(HDC, RECT&, COLORREF, COLORREF, int, int ropCode = R2_XORPEN);
void DisplayCount(HDC, int, RECT&, COLORREF color = RGB(255, 255, 255));
int CheckStrikeX(RECT&, RECT&);
int CheckBallBoundX(RECT&, RECT&);
int CheckBallBoundY(RECT&, RECT&);
int HitTest(RECT&, RECT&);

HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = _T("BkGame 0.9 ROP");

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
		300, 100, 1000, 700,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	hWndMain = hWnd;

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

	static RECT Blocks[5][5];
	int hitResult_2;
	static int gameScore;

	static RECT textR;
	static RECT barR, clientR;
	static RECT ballR;
	static COLORREF barColor;
	static int alphaX;
	static int ballToggleX;
	static int ballToggleY;

	static POINT p;
	static POINT q;
	static BOOL dragFlag = FALSE;

	int vOffset;
	int hitPosition;
	static int hitNumCount;

	TCHAR str[100] = _T("");

	switch (iMessage) {
	case WM_CREATE:
		barColor = RGB(255, 0, 0);
		ballToggleX = 1;
		ballToggleY = 1;
		GetClientRect(hWnd, &clientR);
		InitBlocks(Blocks, clientR);

		p = CenterPoint(clientR);
		textR = clientR;
		textR.bottom = textR.top + 20;
		SetRect(&barR, p.x - 50, p.y - 15, p.x + 50, p.y + 15);
		SetRect(&ballR, p.x - 10, p.y - 10, p.x + 10, p.y + 10);
		OffsetRect(&barR, 0, 300);
		SetTimer(hWnd, 1, 10, NULL);
		hitNumCount = 0;
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		DrawBlocks(hdc, Blocks);

		DrawObject(hdc, barR, barColor, 0);
		DrawObject(hdc, ballR, RGB(255, 255, 255), RGB(255, 0, 255), 1);

		DisplayCount(hdc, gameScore, clientR);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			alphaX = -1;
			if (CheckStrikeX(barR, clientR) == 1)
				break;
			else
			{
				OffsetRect(&barR, 5 * alphaX, 0);
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		case VK_RIGHT:
			alphaX = 1;
			if (CheckStrikeX(barR, clientR) == 2)
				break;
			else
			{
				OffsetRect(&barR, 5 * alphaX, 0);
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		}
		return 0;
	case WM_LBUTTONDOWN:
		p.x = LOWORD(lParam);
		p.y = HIWORD(lParam);
		if (PtInRect(&barR, p))
			dragFlag = TRUE;
		return 0;
	case WM_MOUSEMOVE:
		hdc = GetDC(hWnd);
		if (dragFlag) {
			q.x = LOWORD(lParam);
			q.y = HIWORD(lParam);

			DrawObject(hdc, barR, barColor, 0);

			vOffset = q.x - p.x;

			OffsetRect(&barR, vOffset, 0);
			DrawObject(hdc, barR, barColor, 0);
			p = q;
		}
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_LBUTTONUP:
		if (dragFlag)
			dragFlag = FALSE;
		return 0;
	case WM_TIMER:
		hdc = GetDC(hWnd);

		if (CheckGameOver(ballR, barR))
			SendMessage(hWnd, UM_GAMEOVER, 0, 0);
		else {
			DrawObject(hdc, ballR, RGB(255, 255, 255), RGB(255, 0, 255), 1);

			hitNumCount = HitTest(ballR, barR);
			hitResult_2 = HitTest(hdc, ballR, Blocks);

			if (CheckBallBoundX(ballR, clientR) || hitResult_2 == 1)
				ballToggleX *= -1;
			if (CheckBallBoundY(ballR, clientR) || hitNumCount || hitResult_2 == 2)
				ballToggleY *= -1;

			OffsetRect(&ballR, 3 * ballToggleX, 3 * ballToggleY);

			if (hitNumCount)
				sndPlaySound(_T("LASER.wav"), SND_ASYNC);
	
			if (hitResult_2) {
				sndPlaySound(_T("BarHit.wav"), SND_ASYNC);

				gameScore++;
				InvalidateRect(hWnd, &textR, TRUE);
			}
			DrawObject(hdc, ballR, RGB(255, 255, 255), RGB(255, 0, 255), 1);
		}
		ReleaseDC(hWnd, hdc);
		return 0;
	case UM_GAMEOVER:
		KillTimer(hWnd, 1);
		_stprintf_s(str, _T("Hit Count:%3d\n\nDo you want to restart?"), gameScore);

		hitPosition = MessageBox(hWnd, str, _T("confirm"), MB_YESNO | MB_ICONQUESTION);

		if (hitPosition == IDNO)
			DestroyWindow(hWnd);
		else {
			SendMessage(hWnd, WM_CREATE, 0, 0);
			InvalidateRect(hWnd, NULL, TRUE);
		}
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

void DrawObject(HDC hdc, RECT& r, COLORREF color, int type, int ropCode) {
	DrawObject(hdc, r, color, color, type);
}

void DrawObject(HDC hdc, RECT& r, COLORREF penC, COLORREF brushC, int type, int ropCode) {
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	SetROP2(hdc, ropCode);
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

int CheckStrikeX(RECT& bar, RECT& client)
{
	if (bar.left <= client.left)
		return 1;
	else if (bar.right >= client.right)
		return 2;
	else
		return 0;
}

int CheckBallBoundX(RECT& ball, RECT& client) {
	if (ball.left <= client.left || ball.right >= client.right)
		return 1;
	else
		return 0;
}

int CheckBallBoundY(RECT& ball, RECT& client) {
	if (ball.top <= client.top || ball.bottom >= client.bottom)
		return 1;
	else
		return 0;
}

int HitTest(RECT& ball, RECT& bar) {
	RECT temp;
	int w, h;

	if (IntersectRect(&temp, &ball, &bar))
	{
		w = temp.right - temp.left;
		h = temp.top - temp.bottom;

		if (w < h)
			return 1;
		else
			return 2;
	}
	else
		return 0;
}

void DisplayCount(HDC hdc, int count, RECT& client, COLORREF color)
{
	TCHAR str[100] = _T("");
	_stprintf_s(str, _T("Hit Count: %6d "), count);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 255, 255));

	DrawText(hdc, str, _tcslen(str), &client, DT_SINGLELINE | DT_RIGHT | DT_TOP);
}

void InitBlocks(RECT block[][5], RECT& bound)
{
	RECT r, t;
	int w, h;

	r = bound; r.right /= 5;
	
	r.top = 20;
	r.bottom = r.top + 20;

	w = r.right - r.left;
	h = r.bottom - r.top;

	for (int i = 0; i < 5; i++) {
		t = r;
		OffsetRect(&r, 0, h);

		for (int j = 0; j < 5; j++)
		{
			block[i][j] = t;
			OffsetRect(&t, w, 0);
		}
	}
}

void DrawBlocks(HDC hdc, RECT blocks[][5])
{
	COLORREF color = 255;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (m_Patterns[i][j])
				DrawObject(hdc, blocks[i][j], RGB(0, 0, 0), RGB(0, color, color), 0);
		}
		color -= 25;
	}
}

int CheckGameOver(RECT& ball, RECT& bar)
{
	if (ball.bottom > bar.top + 10)
	{
		sndPlaySound(_T("hf_inc2.wav"), SND_ASYNC);
		return 1;
	}
	else
		return 0;
}

int HitTest(HDC hdc, RECT& ball, RECT blocks[][5])
{
	int result;
	COLORREF color = 255;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (m_Patterns[i][j]) {
				result = HitTest(ball, blocks[i][j]);

				if (result)
				{
					m_Patterns[i][j] = 0;
					DrawObject(hdc, blocks[i][j], RGB(0, 0, 0), RGB(0, color, color), 0);
					return result;
				}
			}
		}
		color -= 25;
	}
	return 0;
}