#include "stdafx.h"
#include "Library.h"
#include "SVGRoot.h"
#include <windowsx.h>
#include <locale>
#include <string>

#define IDM_FILE_EXIT   1001
#define IDM_HELP_GUIDE  1201

float g_Zoom = 1.0f;
float g_PanX = 0.0f;
float g_PanY = 0.0f;
float g_Angle = 0.0f;
float scaleTransform = 1.0f;
bool g_HasInitCamera = false;
RectF g_SvgBounds;

bool g_IsDragging = false;
int g_LastMouseX = 0;
int g_LastMouseY = 0;

void ShowHelpGuide(HWND hWnd) {
    MessageBox(hWnd,
        L"HƯỚNG DẪN SỬ DỤNG:\n\n"
        L"1. Chuột trái : Kéo thả (Pan)\n"
        L"2. Cuộn chuột : Zoom (Phóng to/Thu nhỏ)\n"
        L"3. Phím R : Xoay phải\n"
        L"4. Phím L : Xoay trái\n"
        L"5. Phím F : Lật ảnh (Flip)\n"
        L"6. Phím 0 : Reset về mặc định\n",
        L"Help",
        MB_OK | MB_ICONINFORMATION);
}

void ResetView() {
    g_Zoom = 1.0f;
    g_PanX = 0.0f;
    g_PanY = 0.0f;
    g_Angle = 0.0f;
    scaleTransform = 1.0f;
}

SVGRoot svgRoot;

void InitCamera(float screenW, float screenH) {
    g_Zoom = 1.0f;
    g_PanX = 0.0f;
    g_PanY = 0.0f;
    g_Angle = 0.0f;
    scaleTransform = 1.0f;
}

VOID OnPaint(HDC hdc) {
    RECT rect;
    GetClientRect(WindowFromDC(hdc), &rect);
    float winWidth = (float)(rect.right - rect.left);
    float winHeight = (float)(rect.bottom - rect.top);

    if (winWidth == 0 || winHeight == 0) return;

    if (!g_HasInitCamera) {
        g_SvgBounds = svgRoot.getBoundingBox();
        InitCamera(winWidth, winHeight);
        g_HasInitCamera = true;
    }

    Bitmap backBuffer((INT)winWidth, (INT)winHeight);
    Graphics bufferGraphics(&backBuffer);

    bufferGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
    bufferGraphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    bufferGraphics.SetCompositingQuality(CompositingQualityHighQuality);
    bufferGraphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    bufferGraphics.Clear(Color::White);

    GraphicsState state = bufferGraphics.Save();

    float vbX = svgRoot.getVbX();
    float vbY = svgRoot.getVbY();
    float vbW = svgRoot.getVbWidth();
    float vbH = svgRoot.getVbHeight();

    if (vbW <= 0 || vbH <= 0) {
        vbX = 0; vbY = 0;
        if (g_SvgBounds.Width > 0 && g_SvgBounds.Height > 0) {
            vbW = g_SvgBounds.X + g_SvgBounds.Width;
            vbH = g_SvgBounds.Y + g_SvgBounds.Height;
        }
        else { vbW = 100; vbH = 100; }
    }

    float cxWorld = g_SvgBounds.X + g_SvgBounds.Width / 2.0f;
    float cyWorld = g_SvgBounds.Y + g_SvgBounds.Height / 2.0f;

    float pivotX = (cxWorld - vbX) * g_Zoom + g_PanX;
    float pivotY = (cyWorld - vbY) * g_Zoom + g_PanY;

    bufferGraphics.TranslateTransform(pivotX, pivotY);
    bufferGraphics.RotateTransform(g_Angle);
    bufferGraphics.ScaleTransform(scaleTransform * g_Zoom, g_Zoom);

    bufferGraphics.TranslateTransform(-cxWorld, -cyWorld);

    svgRoot.render(&bufferGraphics, 0, 0, true);

    bufferGraphics.Restore(state);

    Graphics screenGraphics(hdc);
    screenGraphics.DrawImage(&backBuffer, 0, 0);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR lpCmdLine, INT iCmdShow) {
    setlocale(LC_ALL, "C");
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    std::string fileName = "FireFox.svg";
    if (lpCmdLine && strlen(lpCmdLine) > 0) {
        std::string cmdArg = lpCmdLine;
        if (cmdArg.length() >= 2 && cmdArg.front() == '\"' && cmdArg.back() == '\"') {
            cmdArg = cmdArg.substr(1, cmdArg.length() - 2);
        }
        fileName = cmdArg;
    }

    try { svgRoot.loadFromFile(fileName); }
    catch (const std::exception& e) {
        std::string errorMsg = "Loi nap file: " + fileName + "\nChi tiet: " + e.what();
        MessageBoxA(NULL, errorMsg.c_str(), "SVG Error", MB_OK | MB_ICONERROR);
        GdiplusShutdown(gdiplusToken);
        return -1;
    }

    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();

    AppendMenu(hMenu, MF_STRING, IDM_HELP_GUIDE, L"&Help");

    WNDCLASS wndClass;
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = TEXT("GettingStarted");
    RegisterClass(&wndClass);

    HWND hWnd = CreateWindow(
        TEXT("GettingStarted"),
        (std::wstring(L"SVG Renderer - ") + (lpCmdLine && lpCmdLine[0] ? L"Custom File" : L"FireFox.svg")).c_str(),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, hMenu, hInstance, NULL);

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    SVGRoot::CleanupStaticResources();
    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;
    switch (message) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_HELP_GUIDE:
            ShowHelpGuide(hWnd);
            break;
        case IDM_FILE_EXIT:
            PostQuitMessage(0);
            break;
        }
        return 0;
    case WM_MOUSEWHEEL: {
        short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (zDelta > 0) g_Zoom *= 1.1f; else g_Zoom /= 1.1f;
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;
    }
    case WM_LBUTTONDOWN:
        g_IsDragging = true;
        g_LastMouseX = GET_X_LPARAM(lParam);
        g_LastMouseY = GET_Y_LPARAM(lParam);
        SetCapture(hWnd);
        return 0;
    case WM_MOUSEMOVE:
        if (g_IsDragging) {
            int currentX = GET_X_LPARAM(lParam);
            int currentY = GET_Y_LPARAM(lParam);
            g_PanX += (currentX - g_LastMouseX);
            g_PanY += (currentY - g_LastMouseY);
            g_LastMouseX = currentX;
            g_LastMouseY = currentY;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        return 0;
    case WM_LBUTTONUP:
        g_IsDragging = false;
        ReleaseCapture();
        return 0;
    case WM_KEYDOWN:
        switch (wParam) {
        case 'R': g_Angle += 10.0f; break;
        case 'L': g_Angle -= 10.0f; break;
        case 'F': scaleTransform = (scaleTransform == 1.0f) ? -1.0f : 1.0f; break;
        case '0': ResetView(); break;
        case VK_F1: ShowHelpGuide(hWnd); break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}