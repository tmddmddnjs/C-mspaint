#include <windows.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void PaintTheBlock(HWND, COLORREF, int);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Mouse");
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
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
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;// | CS_DBLCLKS;
	RegisterClass(&WndClass);
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}
TCHAR* Items[] = { TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"),TEXT("5"),TEXT("10") };
HWND r1, r2, r3;
HWND hCombo, hRed, hGreen, hBlue, hsRed, hsGreen, hsBlue, hPaint;

typedef struct _Straight_Shape // 직선
{
	COLORREF iColor_Pen;
	int iWidth;
	POINT p[2];
}Straight_Shape;
typedef struct _Line_Shape // 곡선
{
	COLORREF iColor_Pen;
	int iCount_Point;
	int iWidth;
	POINT p[1000];
}Line_Shape;
typedef struct _Circle_Shape //원형
{
	COLORREF iColor_Brush;
	COLORREF iColor_Pen;
	int iWidth;
	POINT p[2];
}Circle__Shape;
typedef struct _Rectangle_Shape //사각형
{
	COLORREF iColor_Brush;
	COLORREF iColor_Pen;
	int iWidth;
	POINT p[2];
}Rectangle_Shape;
typedef struct _Brush_Shape {
	COLORREF iColor_Brush;
	int iCount_Point;
	POINT pt;
}Brush_Shape;
typedef struct _Random_Brush {
	COLORREF iColor_Brush;
	int iCount_Point;
	POINT pt;
}Random_Brsuh;
typedef struct _Delete_Circle //지우개
{
	POINT p[1000];
	int iCount_Point;
	int iWidth;
}Delete_Circle;
Line_Shape lines[1000];
Straight_Shape straights[1000];
Rectangle_Shape rectangles[1000];
Circle__Shape circles[1000];
Brush_Shape brush[1000];
Random_Brsuh Rbrush[1000];
Delete_Circle deletes[1000];
int iLineCount, iStraightCount, iRectangleCount, iCircleCount, iBrushCount, iRBrushCount, iDeleteCount,
iAllCount_End;
int iAllShape[1000]; // WM_PAINT위한 배열
COLORREF iCurrentColor_Brush = RGB(255, 255, 255); // 기본색상 브러쉬 하양
COLORREF iCurrentColor_Pen = RGB(0, 0, 0); // 기본색상 펜 검정
int iCurrentWidth = 1; // 기본 선두께
int iCurrentShape = 2; // 기본 모양(곡선)
HCURSOR e20, e40, e60, cur; // 커서
int mchk; // 지우개 형태 체크

#include "resource.h"
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) //다일로그 함수
{
	static int Red, Green, Blue;
	static COLORREF iColor; static int iWidth;
	static COLORREF iColor2;
	int TempPos; TCHAR str[128];
	switch (iMessage) {
	case WM_INITDIALOG:
		iWidth = iCurrentWidth;
		hPaint = GetDlgItem(hDlg, IDC_PAINT);
		hCombo = GetDlgItem(hDlg, IDC_COMBO1);
		hRed = GetDlgItem(hDlg, IDC_SCROLLBAR1);
		hGreen = GetDlgItem(hDlg, IDC_SCROLLBAR2);
		hBlue = GetDlgItem(hDlg, IDC_SCROLLBAR3);
		//hsRed = GetDlgItem(hDlg, IDC_STATIC1);//Test OK
		hsGreen = GetDlgItem(hDlg, IDC_STATIC2);
		hsBlue = GetDlgItem(hDlg, IDC_STATIC3);
		for (int i = 0; i < 6; i++) {
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)Items[i]);
		}
		//SendMessage(hCombo, CB_SETCURSEL, iWidth - 1, 0);
		SetDlgItemInt(hDlg, IDC_COMBO1, iCurrentWidth, FALSE);
		Red = GetRValue(iCurrentColor_Pen);
		Green = GetGValue(iCurrentColor_Pen);
		Blue = GetBValue(iCurrentColor_Pen);
		SetScrollRange(hRed, SB_CTL, 0, 255, TRUE);
		SetScrollPos(hRed, SB_CTL, Red, TRUE);
		SetScrollRange(hGreen, SB_CTL, 0, 255, TRUE);
		SetScrollPos(hGreen, SB_CTL, Green, TRUE);
		SetScrollRange(hBlue, SB_CTL, 0, 255, TRUE);
		SetScrollPos(hBlue, SB_CTL, Blue, TRUE);
		wsprintf(str, "R:%d", Red);
		SetDlgItemText(hDlg, IDC_STATIC1, str);
		wsprintf(str, "G:%d", Green);
		SetWindowText(hsGreen, str);
		wsprintf(str, "B:%d", Blue);
		SetWindowText(hsBlue, str);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			iCurrentWidth = iWidth;
			iCurrentColor_Pen = iColor;
			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case IDC_COMBO1:
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE:
				iWidth = SendMessage(hCombo, CB_GETCURSEL, 0, 0) + 1;
				PaintTheBlock(hPaint, iColor, iWidth);
				InvalidateRect(hDlg, NULL, TRUE);
				break;
			case CBN_EDITCHANGE:
				iWidth = GetDlgItemInt(hDlg, IDC_COMBO1, NULL, FALSE);
				PaintTheBlock(hPaint, iColor, iWidth);
				InvalidateRect(hDlg, NULL, TRUE);
				break;
			}
			return TRUE;
		}
		return TRUE;
	case WM_HSCROLL:
		/* if ((HWND)lParam == hRed) temp = r;
		if ((HWND)lParam == hGreen) temp = g;
		if ((HWND)lParam == hBlue) temp = b;*/
		TempPos = GetScrollPos((HWND)lParam, SB_CTL);
		switch (LOWORD(wParam)) {
		case SB_LINELEFT:
			TempPos = max(0, TempPos - 1);
			break;
		case SB_LINERIGHT:
			TempPos = min(255, TempPos + 1);
			break;
		case SB_PAGELEFT:
			TempPos = max(0, TempPos - 10);
			break;
		case SB_PAGERIGHT:
			TempPos = min(255, TempPos + 10);
			break;
		case SB_THUMBTRACK:
			TempPos = HIWORD(wParam);
			break;
		}
		if ((HWND)lParam == hRed) Red = TempPos;
		if ((HWND)lParam == hGreen) Green = TempPos;
		if ((HWND)lParam == hBlue) Blue = TempPos;
		SetScrollPos((HWND)lParam, SB_CTL, TempPos, TRUE);
		wsprintf(str, "R:%d", Red);
		SetDlgItemText(hDlg, IDC_STATIC1, str);
		wsprintf(str, "G:%d", Green);
		SetWindowText(hsGreen, str);
		wsprintf(str, "B:%d", Blue);
		SetWindowText(hsBlue, str);
		iColor = RGB(Red, Green, Blue);
		iCurrentColor_Pen = iColor;
		PaintTheBlock(hPaint, iColor, iWidth);
		iCurrentColor_Brush = iColor; // 임시 색상 ( 후에 브러쉬 전용 인터페이스 구현바람 )
		//InvalidateRect(hDlg, NULL, TRUE);
			return TRUE;
	case WM_PAINT:
		PaintTheBlock(hPaint, iColor, iWidth);
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN MyPen;
	HBRUSH MyBrush;
	RECT rect;
	static int x, y;
	static int oldx, oldy;
	int straight_count, line_count, circle_count, rectangle_count, brush_count, Rbrush_count, delete_count;
	switch (iMessage) {
	case WM_CREATE:
		iCurrentShape = 0;
		cur = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_CURSOR1));
		e20 = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_ERASER20));
		e40 = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_ERASER40));
		e60 = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_ERASER60));
		return 0;
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_DIALOG:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc);
			break;
		case ID_40008: //직선
			iCurrentShape = 1; mchk = 0; break;
		case ID_40009: //곡선
			iCurrentShape = 2; mchk = 0; break;
		case ID_40010: //타원
			iCurrentShape = 3; mchk = 0; break;
		case ID_40011: //네모
			iCurrentShape = 4; mchk = 0; break;
		case ID_40012: //브러쉬
			iCurrentShape = 6; mchk = 0; break;
		case ID_40020:
			iCurrentShape = 7; mchk = 0; break;
		case ID_40013: //지우개
			iCurrentShape = 5; mchk = 1; break;
		case ID_40019: //채우기
			hdc = GetDC(hWnd);
			MyBrush = CreateSolidBrush(iCurrentColor_Brush);
			MyBrush = (HBRUSH)SelectObject(hdc, MyBrush);
			ExtFloodFill(hdc, x, y, RGB(0, 0, 0), FLOODFILLBORDER);
			DeleteObject(SelectObject(hdc, MyBrush));
			ReleaseDC(hWnd, hdc);
			break;
		case ID_40014: //지우개 사이즈
			iCurrentShape = 10; mchk = 1; break;
		case ID_40015: //지우개 사이즈
			iCurrentShape = 20; mchk = 2;  break;
		case ID_40016: //지우개 사이즈
			iCurrentShape = 30; mchk = 3;  break;
		case ID_40017: //전체 지우기
				hdc = GetDC(hWnd);
				GetClientRect(hWnd, &rect);
				FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
				ReleaseDC(hWnd, hdc);
				straights[0] = straights[iStraightCount];
				lines[0] = lines[iLineCount];
				circles[0] = circles[iCircleCount];
				rectangles[0] = rectangles[iRectangleCount];
				brush[0] = brush[iBrushCount];
				Rbrush[0] = Rbrush[iRBrushCount];
				deletes[0] = deletes[iDeleteCount];
				iAllShape[0] = iAllShape[iAllCount_End];
				iRectangleCount = 0;
				iLineCount = 0;
				iCircleCount = 0;
				iDeleteCount = 0;
				iAllCount_End = 0;
				break;
		}
		return 0;
	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		oldx = x;
		oldy = y;
		if (iCurrentShape == 1) { //직선 그리기
			straights[iStraightCount].p[0].x = x;
			straights[iStraightCount].p[0].y = y;
			straights[iStraightCount].iWidth = iCurrentWidth;
			straights[iStraightCount].iColor_Pen = iCurrentColor_Pen;
		}
		else if (iCurrentShape == 2) { //곡선
			lines[iLineCount].iCount_Point = 0;
			lines[iLineCount].p[lines[iLineCount].iCount_Point].x = x;
			lines[iLineCount].p[lines[iLineCount].iCount_Point].y = y;
			lines[iLineCount].iWidth = iCurrentWidth;
			lines[iLineCount].iColor_Pen = iCurrentColor_Pen;
		}
		else if (iCurrentShape == 3) { //원형
			circles[iCircleCount].p[0].x = x;
			circles[iCircleCount].p[0].y = y;
			circles[iCircleCount].iWidth = iCurrentWidth;
			circles[iCircleCount].iColor_Pen = iCurrentColor_Pen;
			circles[iCircleCount].iColor_Brush = iCurrentColor_Brush;
		}
		else if (iCurrentShape == 4) { //네모
			rectangles[iRectangleCount].p[0].x = x;
			rectangles[iRectangleCount].p[0].y = y;
			rectangles[iRectangleCount].iWidth = iCurrentWidth;
			rectangles[iRectangleCount].iColor_Pen = iCurrentColor_Pen;
			rectangles[iRectangleCount].iColor_Brush = iCurrentColor_Brush;
		}
		else if (iCurrentShape == 6) { //브러쉬
			brush[iBrushCount].pt.x = x;
			brush[iBrushCount].pt.y = y;
			brush[iBrushCount].iColor_Brush = iCurrentColor_Brush;
		}
		else if (iCurrentShape == 7) {
			Rbrush[iRBrushCount].pt.x = x;
			Rbrush[iRBrushCount].pt.y = y;
			Rbrush[iRBrushCount].iColor_Brush = iCurrentColor_Brush;
		}
		else if (iCurrentShape == 10 || iCurrentShape == 20 || iCurrentShape == 30) {
			deletes[iDeleteCount].iCount_Point = 0;
			deletes[iDeleteCount].p[deletes[iDeleteCount].iCount_Point].x = x;
			deletes[iDeleteCount].p[deletes[iDeleteCount].iCount_Point].y = y;
			deletes[iDeleteCount].iWidth = iCurrentShape * 2;
		}
		else {
			lines[iLineCount].iCount_Point = 0;
			lines[iLineCount].p[lines[iLineCount].iCount_Point].x = x;
			lines[iLineCount].p[lines[iLineCount].iCount_Point].y = y;
			lines[iLineCount].iWidth = iCurrentWidth;
			lines[iLineCount].iColor_Pen = iCurrentColor_Pen;
		}
		return 0;
	case WM_SETCURSOR:
		switch (mchk) {
		case 0:
			SetCursor(cur);
			return TRUE;
		case 1:
			SetCursor(e20);
			return TRUE;
		case 2:
			SetCursor(e40);
			return TRUE;
		case 3:
			SetCursor(e60);
			return TRUE;
		}
		break;
	case WM_MOUSEMOVE:
		hdc = GetDC(hWnd);
		if (wParam & MK_LBUTTON) {
			if (iCurrentShape == 1) { //직선 그리기
				SetROP2(hdc, R2_NOTXORPEN);
				MyPen = CreatePen(PS_SOLID, iCurrentWidth, iCurrentColor_Pen);
				MyPen = (HPEN)SelectObject(hdc, MyPen);
				MoveToEx(hdc, oldx, oldy, NULL); // 이전 궤적 지운다
				LineTo(hdc, x, y);
				x = LOWORD(lParam);
				y = HIWORD(lParam);
				straights[iStraightCount].p[1].x = x;
				straights[iStraightCount].p[1].y = y;
				MoveToEx(hdc, oldx, oldy, NULL); // 새로운 궤적 그린다
				LineTo(hdc, x, y);
				DeleteObject(SelectObject(hdc, MyPen));
				//InvalidateRect(hWnd, NULL, FALSE);
			}
			else if (iCurrentShape == 2) { //곡선
				MyPen = CreatePen(PS_SOLID, iCurrentWidth, iCurrentColor_Pen);
				MyPen = (HPEN)SelectObject(hdc, MyPen);
				MoveToEx(hdc, x, y, NULL);
				x = LOWORD(lParam);
				y = HIWORD(lParam);
				lines[iLineCount].iCount_Point++;
				lines[iLineCount].p[lines[iLineCount].iCount_Point].x = x;
				lines[iLineCount].p[lines[iLineCount].iCount_Point].y = y;
				LineTo(hdc, x, y);
				DeleteObject(SelectObject(hdc, MyPen));
			}
			else if (iCurrentShape == 3) { //원형
				SetROP2(hdc, R2_NOTXORPEN);
				MyPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
				MyPen = (HPEN)SelectObject(hdc, MyPen);
				Ellipse(hdc, oldx, oldy, x, y);
				x = LOWORD(lParam);
				y = HIWORD(lParam);
				circles[iCircleCount].p[1].x = x;
				circles[iCircleCount].p[1].y = y;
				Ellipse(hdc, oldx, oldy, x, y);
				DeleteObject(SelectObject(hdc, MyPen));
			}
			else if (iCurrentShape == 4) { //네모
				SetROP2(hdc, R2_NOTXORPEN);
				MyPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
				MyPen = (HPEN)SelectObject(hdc, MyPen);
				Rectangle(hdc, oldx, oldy, x, y);
				x = LOWORD(lParam);
				y = HIWORD(lParam);
				rectangles[iRectangleCount].p[1].x = x;
				rectangles[iRectangleCount].p[1].y = y;
				Rectangle(hdc, oldx, oldy, x, y);
				DeleteObject(SelectObject(hdc, MyPen));
			}
			else if (iCurrentShape == 6) { //brush
				MyBrush = CreateSolidBrush(iCurrentColor_Brush);
				MyBrush = (HBRUSH)SelectObject(hdc, MyBrush);
				SelectObject(hdc, GetStockObject(iCurrentColor_Brush));
				brush[iBrushCount].pt.x = x = LOWORD(lParam);
				brush[iBrushCount].pt.y = y = HIWORD(lParam);
				brush[iBrushCount].iColor_Brush = iCurrentColor_Brush;
				iBrushCount++;
				Ellipse(hdc, x, y, x + 20, y + 20);
				Sleep(15);
				SelectObject(hdc, MyBrush);
				DeleteObject(MyBrush); 
			}
			else if (iCurrentShape == 7) { //랜덤 브러쉬
				MyBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
				MyBrush = (HBRUSH)SelectObject(hdc, MyBrush);
				SelectObject(hdc, GetStockObject(iCurrentColor_Brush));
				Rbrush[iRBrushCount].pt.x = x = LOWORD(lParam);
				Rbrush[iRBrushCount].pt.y = y = HIWORD(lParam);
				Rbrush[iRBrushCount].iColor_Brush = iCurrentColor_Brush;
				iRBrushCount++;
				Ellipse(hdc, x, y, x + 20, y + 20);
				Sleep(15);
				SelectObject(hdc, MyBrush);
				DeleteObject(MyBrush);
			}
			else if (iCurrentShape == 10 || iCurrentShape == 20 || iCurrentShape == 30) {
				if (iCurrentShape == 10) {
					MyPen = CreatePen(PS_SOLID, 10, RGB(255, 255, 255));
					MyPen = (HPEN)SelectObject(hdc, MyPen);
					SelectObject(hdc, GetStockObject(NULL_PEN));
					Ellipse(hdc, x, y, x + 10, y + 10);
				}
				else if (iCurrentShape == 20) {
					MyPen = CreatePen(PS_SOLID, 20, RGB(255, 255, 255));
					MyPen = (HPEN)SelectObject(hdc, MyPen);
					SelectObject(hdc, GetStockObject(NULL_PEN));
					Ellipse(hdc, x, y, x + 20, y + 20);
				}
				else if (iCurrentShape == 30) {
					MyPen = CreatePen(PS_SOLID, 30, RGB(255, 255, 255));
					MyPen = (HPEN)SelectObject(hdc, MyPen);
					SelectObject(hdc, GetStockObject(NULL_PEN));
					Ellipse(hdc, x, y, x + 30, y + 30);
				}
				x = LOWORD(lParam);
				y = HIWORD(lParam);
				SelectObject(hdc, MyPen);
				DeleteObject(MyPen);
				ReleaseDC(hWnd, hdc);
			}
			else {
				MyPen = CreatePen(PS_SOLID, iCurrentWidth, iCurrentColor_Pen);
				MyPen = (HPEN)SelectObject(hdc, MyPen);
				MoveToEx(hdc, x, y, NULL);
				x = LOWORD(lParam);
				y = HIWORD(lParam);
				lines[iLineCount].iCount_Point++;
				lines[iLineCount].p[lines[iLineCount].iCount_Point].x = x;
				lines[iLineCount].p[lines[iLineCount].iCount_Point].y = y;
				LineTo(hdc, x, y);
				DeleteObject(SelectObject(hdc, MyPen));
			}
		}
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_LBUTTONUP:
		hdc = GetDC(hWnd);
		if (iCurrentShape == 1) { // 직선
			iStraightCount++;
		}
		else if (iCurrentShape == 2) { // 곡선
			iLineCount++;
		}
		else if (iCurrentShape == 3) { // 원형
			InvalidateRect(hWnd, NULL, TRUE);
			iCircleCount++;
		}
		else if (iCurrentShape == 4) { // 네모
			InvalidateRect(hWnd, NULL, TRUE);
			iRectangleCount++;
		}
		else if (iCurrentShape == 6) {
			iBrushCount++;
		}
		else if (iCurrentShape == 7) {
			iRBrushCount++;
		}
		else if (iCurrentShape == 10 || iCurrentShape == 20 || iCurrentShape == 30) {
			iDeleteCount++;
		}
		else {
			iLineCount++;
		}
		iAllShape[iAllCount_End] = iCurrentShape; // WM_PAINT위한 동작순서 초기화
		iAllCount_End++; // WM_PAINT위한 동작순서 초기화
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_LBUTTONDBLCLK:
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		straight_count = 0;
		line_count = 0;
		circle_count = 0;
		rectangle_count = 0;
		brush_count = 0;
		Rbrush_count = 0;
		delete_count = 0;
		for (int i = 0; i < iAllCount_End; i++) //곡선이네
		{
			if (iAllShape[i] == 1)
			{
				MyPen = CreatePen(PS_SOLID, straights[straight_count].iWidth,
					straights[straight_count].iColor_Pen);
				MyPen = (HPEN)SelectObject(hdc, MyPen);
				MoveToEx(hdc, straights[straight_count].p[0].x,
					straights[straight_count].p[0].y, NULL); // 새로운 궤적 그린다
				LineTo(hdc, straights[straight_count].p[1].x,
					straights[straight_count].p[1].y);
				straight_count++;
				DeleteObject(SelectObject(hdc, MyPen));
			}
			else if (iAllShape[i] == 2)
			{
				MyPen = CreatePen(PS_SOLID, lines[line_count].iWidth,
					lines[line_count].iColor_Pen);
				MyPen = (HPEN)SelectObject(hdc, MyPen);
				for (int j = 0; j < lines[line_count].iCount_Point - 1; j++)
				{
					MoveToEx(hdc, lines[line_count].p[j].x,
						lines[line_count].p[j].y, NULL);
					LineTo(hdc, lines[line_count].p[j + 1].x,
						lines[line_count].p[j + 1].y);
				}
				line_count++;
				DeleteObject(SelectObject(hdc, MyPen));
			}
			else if (iAllShape[i] == 3)
			{
				MyPen = CreatePen(PS_SOLID, circles[circle_count].iWidth,
					circles[circle_count].iColor_Pen);
				MyBrush =
					CreateSolidBrush(circles[circle_count].iColor_Brush);
				MyPen = (HPEN)SelectObject(hdc, MyPen);
				MyBrush = (HBRUSH)SelectObject(hdc, MyBrush);
				Ellipse(hdc, circles[circle_count].p[0].x,
					circles[circle_count].p[0].y,
					circles[circle_count].p[1].x,
					circles[circle_count].p[1].y);
				circle_count++;
				DeleteObject(SelectObject(hdc, MyPen));
				DeleteObject(SelectObject(hdc, MyBrush));
			}
			else if (iAllShape[i] == 4)
			{
				MyPen = CreatePen(PS_SOLID,
					rectangles[rectangle_count].iWidth, rectangles[rectangle_count].iColor_Pen);
				MyBrush =
					CreateSolidBrush(rectangles[rectangle_count].iColor_Brush);
				MyPen = (HPEN)SelectObject(hdc, MyPen);
				MyBrush = (HBRUSH)SelectObject(hdc, MyBrush);
				Rectangle(hdc, rectangles[rectangle_count].p[0].x,
					rectangles[rectangle_count].p[0].y,
					rectangles[rectangle_count].p[1].x,
					rectangles[rectangle_count].p[1].y);
				rectangle_count++;
				DeleteObject(SelectObject(hdc, MyPen));
				DeleteObject(SelectObject(hdc, MyBrush));
			}
			else if (iAllShape[i] == 6) {
				MyBrush = CreateSolidBrush(brush[brush_count].iColor_Brush);
				MyBrush = (HBRUSH)SelectObject(hdc, MyBrush);
				for (int j = 0; j < iBrushCount - 1; j++) {
					Ellipse(hdc, brush[j].pt.x,
						brush[j].pt.y,
						brush[j].pt.x + 20,
						brush[j].pt.y + 20);
				}
				iBrushCount++;
				DeleteObject(SelectObject(hdc, MyBrush));
			}
			else if (iAllShape[i] == 7) {
				MyBrush = CreateSolidBrush(Rbrush[Rbrush_count].iColor_Brush);
				MyBrush = (HBRUSH)SelectObject(hdc, MyBrush);
				for (int j = 0; j < iRBrushCount - 1; j++) {
					Ellipse(hdc, Rbrush[j].pt.x,
						Rbrush[j].pt.y,
						Rbrush[j].pt.x + 20,
						Rbrush[j].pt.y + 20);
				}
				iRBrushCount++;
				DeleteObject(SelectObject(hdc, MyBrush));
			}
			else if (iAllShape[i] == 10 || iAllShape[i] == 20 || iAllShape[i] == 30)
			{
				MyPen = CreatePen(PS_SOLID, iCurrentShape * 20, RGB(255, 255,
					255));
				MyPen = (HPEN)SelectObject(hdc, MyPen);
				for (int j = 0; j < deletes[delete_count].iCount_Point - 1;
					j++)
				{
					MyPen = CreatePen(PS_SOLID, deletes[delete_count].iWidth,
						RGB(255, 255, 255));
					MyPen = (HPEN)SelectObject(hdc, MyPen);
					MoveToEx(hdc, deletes[delete_count].p[j].x,
						deletes[delete_count].p[j].y, NULL);
					LineTo(hdc, deletes[delete_count].p[j + 1].x,
						deletes[delete_count].p[j + 1].y);
				}
				delete_count++;
				DeleteObject(SelectObject(hdc, MyPen));
			}
			else {
				MyPen = CreatePen(PS_SOLID, lines[line_count].iWidth,
					lines[line_count].iColor_Pen);
				MyPen = (HPEN)SelectObject(hdc, MyPen);
				for (int j = 0; j < lines[line_count].iCount_Point - 1; j++)
				{
					MoveToEx(hdc, lines[line_count].p[j].x,
						lines[line_count].p[j].y, NULL);
					LineTo(hdc, lines[line_count].p[j + 1].x,
						lines[line_count].p[j + 1].y);
				}
				line_count++;
				DeleteObject(SelectObject(hdc, MyPen));
			}
		}
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
void PaintTheBlock(HWND hWnd, COLORREF color, int width)
{
	HDC hdc;
	HPEN hPen;
	RECT rect;
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
	hdc = GetDC(hWnd);
	GetClientRect(hWnd, &rect);
	hPen = CreatePen(PS_SOLID, width, color);
	hPen = (HPEN)SelectObject(hdc, hPen);
	MoveToEx(hdc, rect.left, rect.bottom / 2, NULL);
	LineTo(hdc, rect.right, rect.bottom / 2);
	DeleteObject(SelectObject(hdc, hPen));
	ReleaseDC(hWnd, hdc);
}
