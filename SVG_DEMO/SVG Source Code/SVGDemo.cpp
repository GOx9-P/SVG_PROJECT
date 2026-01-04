#include "stdafx.h"
#include "Library.h"
#include "SVGRoot.h"
#include <windowsx.h>
#include <locale>
#include <string>
#include <commdlg.h>
#include <math.h>

#define IDM_FILE_OPEN    1002
#define IDM_FILE_EXIT    1001
#define IDM_HELP_GUIDE   1201
#define IDM_FILE_SAVE_AS 1003
#define TIMER_ID_INTERACTION_DONE 999 

float g_Zoom = 1.0f;
float g_PanX = 0.0f;
float g_PanY = 0.0f;
float g_Angle = 0.0f;
float scaleTransform = 1.0f;

bool g_HasInitCamera = false;
Bitmap* g_pCachedBitmap = NULL;
bool g_IsInteracting = false;
bool g_IsDragging = false;
int g_LastMouseX = 0;
int g_LastMouseY = 0;

SVGRoot svgRoot;

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

void ShowHelpGuide(HWND hWnd) {
    MessageBox(hWnd,
        L"HƯỚNG DẪN SỬ DỤNG:\n\n"
        L"1. Chuột trái : Kéo thả (Pan)\n"
        L"2. Cuộn chuột : Zoom (Phóng to/Thu nhỏ)\n"
        L"3. Phím R : Xoay phải\n"
        L"4. Phím L : Xoay trái\n"
        L"5. Phím F : Lật ảnh (Flip)\n"
        L"6. Phím 0 : Reset về mặc định\n",
        L"Help", MB_OK | MB_ICONINFORMATION);
}

void ResetView() {
    g_Zoom = 1.0f;
    g_PanX = 0.0f;
    g_PanY = 0.0f;
    g_Angle = 0.0f;
    scaleTransform = 1.0f;
}

void InitCamera(float screenW, float screenH) {
    ResetView();
}

void UpdateCachedBitmap() {
    // Chỉ dùng để update trạng thái, không render bitmap tĩnh để tránh lỗi khi xoay
    if (g_pCachedBitmap) { delete g_pCachedBitmap; g_pCachedBitmap = NULL; }
}

VOID OnPaint(HDC hdc) {
    RECT rect;
    GetClientRect(WindowFromDC(hdc), &rect);
    float winWidth = (float)(rect.right - rect.left);
    float winHeight = (float)(rect.bottom - rect.top);
    if (winWidth == 0 || winHeight == 0) return;

    if (!g_HasInitCamera) {
        UpdateCachedBitmap();
        InitCamera(winWidth, winHeight);
        g_HasInitCamera = true;
    }

    Bitmap backBuffer((INT)winWidth, (INT)winHeight);
    Graphics bufferGraphics(&backBuffer);

    if (g_IsInteracting) {
        bufferGraphics.SetSmoothingMode(SmoothingModeHighSpeed);
        bufferGraphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    }
    else {
        bufferGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
        bufferGraphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    }

    bufferGraphics.Clear(Color::White);
    GraphicsState state = bufferGraphics.Save();

    // --- LOGIC: VIEWBOX + OBJECT CENTER INTERACTION (NO CLIP, NO FIT) ---

    // 1. Lấy thông tin ViewBox
    float vbX = svgRoot.getVbX();
    float vbY = svgRoot.getVbY();
    float vbW = svgRoot.getVbWidth();
    float vbH = svgRoot.getVbHeight();

    // Fallback nếu không có ViewBox thì dùng BoundingBox
    if (vbW <= 0 || vbH <= 0) {
        RectF bb = svgRoot.getBoundingBox();
        vbX = bb.X; vbY = bb.Y; vbW = bb.Width; vbH = bb.Height;
    }
    if (vbW <= 0) vbW = 100; if (vbH <= 0) vbH = 100;

    // 2. CẤU HÌNH HIỂN THỊ: TẮT FIT TO SCREEN
    // Đặt scale = 1.0 (kích thước thật) và tx, ty = 0 (vẽ từ góc trên trái)
    float fitScale = 1.0f;
    float tx = 0.0f;
    float ty = 0.0f;

    // 3. Lấy tâm vật thể (BoundingBox) để xoay
    RectF bb = svgRoot.getBoundingBox();
    float objCX = bb.X + bb.Width / 2.0f;
    float objCY = bb.Y + bb.Height / 2.0f;

    // 4. Áp dụng chuỗi biến đổi ma trận
    // D. Đưa về màn hình (chỉ cộng Pan, không scale fit, không center fit)
    bufferGraphics.TranslateTransform(tx + g_PanX, ty + g_PanY);

    // C. Scale fit (Ở đây là 1.0 - giữ nguyên kích thước)
    bufferGraphics.ScaleTransform(fitScale, fitScale);

    // B. Dời ViewBox về gốc (để khớp với tọa độ vẽ)
    bufferGraphics.TranslateTransform(-vbX, -vbY);

    // A. Tương tác người dùng (Xoay/Zoom quanh TÂM VẬT THỂ)
    bufferGraphics.TranslateTransform(objCX, objCY);        // Dời tâm vật thể về vị trí cũ
    bufferGraphics.RotateTransform(g_Angle);                // Xoay
    bufferGraphics.ScaleTransform(scaleTransform * g_Zoom, g_Zoom); // Zoom
    bufferGraphics.TranslateTransform(-objCX, -objCY);      // Dời tâm vật thể về gốc để xoay

    // 5. QUAN TRỌNG: KHÔNG SET CLIP
    // Đã xóa lệnh SetClip để phần thừa ngoài ViewBox vẫn hiện ra

    // 6. Vẽ (Ignore ViewBox nội tại vì ta đã xử lý ma trận ở trên)
    svgRoot.render(&bufferGraphics, (int)winWidth, (int)winHeight, true);

    bufferGraphics.Restore(state);
    Graphics screenGraphics(hdc);
    screenGraphics.DrawImage(&backBuffer, 0, 0);
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0; UINT size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;
    Gdiplus::ImageCodecInfo* pImageCodecInfo = new Gdiplus::ImageCodecInfo[size];
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT i = 0; i < num; ++i) {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[i].Clsid;
            delete[] pImageCodecInfo; return i;
        }
    }
    delete[] pImageCodecInfo; return -1;
}

void SaveSVGImage(HWND hWnd, SVGRoot* root) {
    if (!root) return;
    OPENFILENAME ofn; wchar_t szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn)); ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd; ofn.lpstrFile = szFile; ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"PNG Image (*.png)\0*.png\0JPEG Image (*.jpg)\0*.jpg\0All Files (*.*)\0*.*\0";
    ofn.lpstrDefExt = L"png";
    ofn.nFilterIndex = 1; ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE) {
        RECT rect; GetClientRect(hWnd, &rect);
        int w = rect.right - rect.left; int h = rect.bottom - rect.top;
        Bitmap* bitmap = new Bitmap(w, h, PixelFormat32bppARGB);
        Graphics* g = Graphics::FromImage(bitmap);

        g->SetSmoothingMode(SmoothingModeAntiAlias);
        g->SetInterpolationMode(InterpolationModeHighQualityBicubic);

        if (wcsstr(szFile, L".jpg")) g->Clear(Color::White);
        else g->Clear(Color::Transparent);

        // --- Áp dụng Logic Ma trận y hệt OnPaint ---
        float vbX = root->getVbX();
        float vbY = root->getVbY();
        float vbW = root->getVbWidth();
        float vbH = root->getVbHeight();

        if (vbW <= 0 || vbH <= 0) {
            RectF bb = root->getBoundingBox();
            vbX = bb.X; vbY = bb.Y; vbW = bb.Width; vbH = bb.Height;
        }
        if (vbW <= 0) vbW = 100; if (vbH <= 0) vbH = 100;

        float fitScale = 1.0f;
        float tx = 0.0f;
        float ty = 0.0f;

        RectF bb = root->getBoundingBox();
        float objCX = bb.X + bb.Width / 2.0f;
        float objCY = bb.Y + bb.Height / 2.0f;

        g->TranslateTransform(tx + g_PanX, ty + g_PanY);
        g->ScaleTransform(fitScale, fitScale);
        g->TranslateTransform(-vbX, -vbY);

        g->TranslateTransform(objCX, objCY);
        g->RotateTransform(g_Angle);
        g->ScaleTransform(scaleTransform * g_Zoom, g_Zoom);
        g->TranslateTransform(-objCX, -objCY);

        // Đã xóa SetClip ở đây luôn

        root->render(g, w, h, true);

        CLSID clsid;
        GetEncoderClsid(wcsstr(szFile, L".jpg") ? L"image/jpeg" : L"image/png", &clsid);
        bitmap->Save(szFile, &clsid, NULL);

        delete g; delete bitmap;
        MessageBox(hWnd, L"Saved!", L"Info", MB_OK);
    }
}

bool OpenFileDialog(HWND hWnd, std::string& outFileName) {
    OPENFILENAME ofn; wchar_t szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn)); ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd; ofn.lpstrFile = szFile; ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"SVG Files (*.svg)\0*.svg\0All Files (*.*)\0*.*\0";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileName(&ofn)) { outFileName = WStringToString(ofn.lpstrFile); return true; }
    return false;
}

void TriggerInteraction(HWND hWnd) {
    g_IsInteracting = true;
    SetTimer(hWnd, TIMER_ID_INTERACTION_DONE, 60, NULL);
    InvalidateRect(hWnd, NULL, FALSE);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR lpCmdLine, INT iCmdShow) {
    setlocale(LC_ALL, "C");
    GdiplusStartupInput gdiplusStartupInput; ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    std::string fileName = "FireFox.svg";
    if (lpCmdLine && strlen(lpCmdLine) > 0) {
        std::string cmdArg = lpCmdLine;
        if (cmdArg.length() >= 2 && cmdArg.front() == '\"') cmdArg = cmdArg.substr(1, cmdArg.length() - 2);
        fileName = cmdArg;
    }

    try { svgRoot.loadFromFile(fileName); }
    catch (...) {}

    UpdateCachedBitmap();

    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"&File");
    AppendMenu(hMenu, MF_STRING, IDM_HELP_GUIDE, L"&Help");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_OPEN, L"&Open... (Ctrl+O)");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE_AS, L"&Save As... (Ctrl+S)");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_EXIT, L"E&xit");

    WNDCLASS wndClass = { 0 };
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszClassName = TEXT("SVGClass");
    RegisterClass(&wndClass);

    HWND hWnd = CreateWindow(TEXT("SVGClass"), L"SVG Viewer (Adaptive Mode)", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, hMenu, hInstance, NULL);

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }

    if (g_pCachedBitmap) delete g_pCachedBitmap;
    SVGRoot::CleanupStaticResources();
    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_FILE_OPEN: {
            std::string newFile;
            if (OpenFileDialog(hWnd, newFile)) {
                try {
                    svgRoot.Clear();
                    SVGRoot::CleanupStaticResources();
                    svgRoot.loadFromFile(newFile);
                    UpdateCachedBitmap();
                    ResetView();
                    InvalidateRect(hWnd, NULL, FALSE);
                    SetWindowText(hWnd, (L"SVG Viewer - " + std::wstring(newFile.begin(), newFile.end())).c_str());
                }
                catch (const std::exception& e) { MessageBoxA(hWnd, e.what(), "Error", MB_OK); }
            }
            break;
        }
        case IDM_HELP_GUIDE: ShowHelpGuide(hWnd); break;
        case IDM_FILE_SAVE_AS: SaveSVGImage(hWnd, &svgRoot); break;
        case IDM_FILE_EXIT: PostQuitMessage(0); break;
        }
        return 0;

    case WM_TIMER:
        if (wParam == TIMER_ID_INTERACTION_DONE) {
            KillTimer(hWnd, TIMER_ID_INTERACTION_DONE);
            g_IsInteracting = false;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        return 0;

    case WM_MOUSEWHEEL:
        if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) g_Zoom *= 1.1f; else g_Zoom /= 1.1f;
        TriggerInteraction(hWnd);
        return 0;

    case WM_LBUTTONDOWN:
        g_IsDragging = true;
        g_LastMouseX = GET_X_LPARAM(lParam);
        g_LastMouseY = GET_Y_LPARAM(lParam);
        SetCapture(hWnd);
        TriggerInteraction(hWnd);
        return 0;

    case WM_MOUSEMOVE:
        if (g_IsDragging) {
            g_PanX += (GET_X_LPARAM(lParam) - g_LastMouseX);
            g_PanY += (GET_Y_LPARAM(lParam) - g_LastMouseY);
            g_LastMouseX = GET_X_LPARAM(lParam); g_LastMouseY = GET_Y_LPARAM(lParam);
            TriggerInteraction(hWnd);
        } return 0;

    case WM_LBUTTONUP:
        g_IsDragging = false;
        ReleaseCapture();
        return 0;

    case WM_KEYDOWN:
        switch (wParam) {
        case 'R': g_Angle += 10.0f; TriggerInteraction(hWnd); break;
        case 'L': g_Angle -= 10.0f; TriggerInteraction(hWnd); break;
        case 'F': scaleTransform *= -1.0f; TriggerInteraction(hWnd); break;
        case '0': ResetView(); TriggerInteraction(hWnd); break;
        case 'O': if (GetKeyState(VK_CONTROL) < 0) SendMessage(hWnd, WM_COMMAND, IDM_FILE_OPEN, 0); break;
        case 'S': if (GetKeyState(VK_CONTROL) < 0) SendMessage(hWnd, WM_COMMAND, IDM_FILE_SAVE_AS, 0); break;
        }
        return 0;

    case WM_PAINT: { PAINTSTRUCT ps; BeginPaint(hWnd, &ps); OnPaint(ps.hdc); EndPaint(hWnd, &ps); return 0; }
    case WM_DESTROY: PostQuitMessage(0); return 0;
    default: return DefWindowProc(hWnd, message, wParam, lParam);
    }
}