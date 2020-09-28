#include <windows.h>
#include <iostream>

#define SPEED 10
#define BOUNCE 15

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HBITMAP hBmp;
HBITMAP hMask;

int bmpX = 0;
int bmpY = 0;

RECT rct;

void MoveBmp(int accX, int accY) {
    BITMAP bitmap;
    GetObject(hBmp, sizeof(bitmap), &bitmap);
    int width = bitmap.bmWidth;
    int height = bitmap.bmHeight;
    if ((bmpX + width + accX * SPEED) > rct.right) {
        bmpX -= BOUNCE;
        return;
    }
    if ((bmpX + accX * SPEED) < 0) {
        bmpX += BOUNCE;
        return;
    }
    if ((bmpY + height + accY * SPEED) > rct.bottom) {
        bmpY -= BOUNCE;
        return;
    }
    if ((bmpY - accY * SPEED) < 0) {
        bmpY += BOUNCE;
        return;
    }
    bmpX += accX * SPEED;
    bmpY += accY * SPEED;
}

void DrawWindow(HDC hdc) {
    BITMAP bitmap;
    HDC hdcMem;
    HGDIOBJ oldBitmap;
    SetBkMode(hdc, TRANSPARENT);
    hdcMem = CreateCompatibleDC(hdc);
    oldBitmap = SelectObject(hdcMem, hMask);
    SelectObject(hdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(hdc, RGB(255, 255, 255));
    Rectangle(hdc, 0, 0, rct.right, rct.bottom);
    GetObject(hBmp, sizeof(bitmap), &bitmap);
    SelectObject(hdcMem, hMask);
    BitBlt(hdc, bmpX, bmpY, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCAND);
    SelectObject(hdcMem, hBmp);
    BitBlt(hdc, bmpX, bmpY, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCPAINT);
    SelectObject(hdcMem, oldBitmap);
    DeleteDC(hdcMem);
}



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex; HWND hWnd; MSG msg;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_DBLCLKS;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "Sprite";
    wcex.hIconSm = wcex.hIcon;
    RegisterClassEx(&wcex);
    hWnd = CreateWindow("Sprite", "Sprite",
                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
                        CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
            hBmp = (HBITMAP) LoadImage(NULL, "..\\resource\\dragon.bmp",
                                       IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            hMask = (HBITMAP) LoadImage(NULL, "..\\resource\\dragon-mask.bmp",
                                        IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            break;
        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hDc;
            hDc = BeginPaint(hWnd, &ps);
            DrawWindow(hDc);
            EndPaint(hWnd, &ps);
            return 0;
        case WM_MOUSEWHEEL:
            int key;
            key = GET_KEYSTATE_WPARAM(wParam);
            int delta;
            delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (delta > 0) {
                if (key == MK_SHIFT) {
                    MoveBmp(1, 0);
                } else {
                    MoveBmp(0, -1);
                }
            } else {
                if (key == MK_SHIFT) {
                    MoveBmp(-1, 0);
                } else {
                    MoveBmp(0, 1);
                }
            }
            InvalidateRect(hWnd, NULL, TRUE);
            return 0;
        case WM_SIZE:
            MoveBmp(0, 0);
            GetClientRect(hWnd, &rct);
            InvalidateRect(hWnd, NULL, TRUE);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}