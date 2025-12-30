#include "stdafx.h"
#include "Library.h"
#include "SVGRoot.h"
#include <windowsx.h>
#include <locale>
#include <string>
#include <commdlg.h>

#define IDM_FILE_OPEN   1002
#define IDM_FILE_EXIT   1001
#define IDM_HELP_GUIDE  1201
#define IDM_FILE_SAVE_AS 1003

float g_Zoom = 1.0f;
float g_PanX = 0.0f;
float g_PanY = 0.0f;
float g_Angle = 0.0f;
float scaleTransform = 1.0f;
bool g_HasInitCamera = false;
RectF g_SvgBounds;
Bitmap* g_pCachedBitmap = NULL;

bool g_IsDragging = false;
int g_LastMouseX = 0;
int g_LastMouseY = 0;

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

void UpdateCachedBitmap() {
    if (g_pCachedBitmap) { delete g_pCachedBitmap; g_pCachedBitmap = NULL; }
    g_SvgBounds = svgRoot.getBoundingBox();

    float padding = 50.0f;
    if (g_SvgBounds.Width <= 0) g_SvgBounds.Width = 100;
    if (g_SvgBounds.Height <= 0) g_SvgBounds.Height = 100;

    int w = (int)ceil(g_SvgBounds.Width + 2 * padding);
    int h = (int)ceil(g_SvgBounds.Height + 2 * padding);

    g_pCachedBitmap = new Bitmap(w, h);
    Graphics g(g_pCachedBitmap);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.Clear(Color::Transparent);
    g.TranslateTransform(-g_SvgBounds.X + padding, -g_SvgBounds.Y + padding);
    svgRoot.render(&g, 0, 0, true);
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
    // [FIX] Đổi sang Bilinear để tránh lỗi mất màu khi zoom nhỏ
    bufferGraphics.SetInterpolationMode(Gdiplus::InterpolationModeBilinear);

    // 3. Tăng chất lượng hòa trộn màu (Alpha Blending cho lông cáo và quả cầu)
    bufferGraphics.SetCompositingQuality(Gdiplus::CompositingQualityDefault);

    // [FIX] Đổi sang Default hoặc None để tránh lỗi lệch pixel/thu nhỏ nền khi zoom out
    bufferGraphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeDefault);
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


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;
    // Lay tat ca cac loai du lieu maf GDI+ ho tro
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) {
        return -1;
    }
    Gdiplus::ImageCodecInfo* pImageCodecInfo = new Gdiplus::ImageCodecInfo[size];
    if (pImageCodecInfo == nullptr) {
        return -1;
    }
    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT i = 0; i < num; ++i) {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[i].Clsid;
			delete[] pImageCodecInfo;
			return i;   
        }
    }
	delete[] pImageCodecInfo;
    return -1;
}

// Thay thế toàn bộ hàm SaveSVGImage cũ bằng hàm này
void SaveSVGImage(HWND hWnd, SVGRoot* root) {
    if (!root) {
        MessageBox(hWnd, L"Chưa có dữ liệu SVG để lưu!", L"Lỗi", MB_OK | MB_ICONERROR);
        return;
    }

    OPENFILENAME ofn;
    wchar_t szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"PNG Image (*.png)\0*.png\0JPEG Image (*.jpg)\0*.jpg\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrDefExt = L"png";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE) {
        // [QUAN TRỌNG 1] Lấy kích thước hiện tại của cửa sổ View
        // Để đảm bảo ảnh lưu ra có tỉ lệ và vị trí (độ lệch) y hệt màn hình
        RECT rect;
        GetClientRect(hWnd, &rect);
        int winWidth = rect.right - rect.left;
        int winHeight = rect.bottom - rect.top;

        // Tạo Bitmap bằng kích thước cửa sổ
        Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(winWidth, winHeight, PixelFormat32bppARGB);
        Gdiplus::Graphics* graphics = Gdiplus::Graphics::FromImage(bitmap);

        graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

        bool isJPG = (wcsstr(szFile, L".jpg") != NULL || wcsstr(szFile, L".jpeg") != NULL);
        if (isJPG) graphics->Clear(Gdiplus::Color::White);
        else graphics->Clear(Gdiplus::Color::Transparent);

        // [QUAN TRỌNG 2] SAO CHÉP LOGIC CAMERA TỪ ONPAINT
        // Tính toán ViewBox và các thông số y hệt như lúc hiển thị
        float vbX = root->getVbX();
        float vbY = root->getVbY();
        float vbW = root->getVbWidth();
        float vbH = root->getVbHeight();

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

        // Tính toán điểm neo dựa trên độ lệch (Pan) và Zoom hiện tại
        float pivotX = (cxWorld - vbX) * g_Zoom + g_PanX;
        float pivotY = (cyWorld - vbY) * g_Zoom + g_PanY;

        // Áp dụng chuỗi biến đổi y hệt OnPaint
        graphics->TranslateTransform(pivotX, pivotY);           // 1. Dời đến vị trí lệch
        graphics->RotateTransform(g_Angle);                     // 2. Xoay
        graphics->ScaleTransform(scaleTransform * g_Zoom, g_Zoom); // 3. Zoom & Flip (Lật)
        graphics->TranslateTransform(-cxWorld, -cyWorld);       // 4. Dời về gốc để xoay/scale

        // [QUAN TRỌNG 3] Vẽ với chế độ ignoreViewBox = true
        // Để hình vẽ tuân theo các phép biến đổi Transform ở trên
        root->render(graphics, 0, 0, true);

        // Lưu file
        CLSID encoderClsid;
        if (isJPG) GetEncoderClsid(L"image/jpeg", &encoderClsid);
        else GetEncoderClsid(L"image/png", &encoderClsid);

        Gdiplus::Status stat = bitmap->Save(szFile, &encoderClsid, NULL);

        if (stat == Gdiplus::Ok) MessageBox(hWnd, L"Lưu ảnh thành công!", L"Thông báo", MB_OK | MB_ICONINFORMATION);
        else MessageBox(hWnd, L"Lưu ảnh thất bại!", L"Lỗi", MB_OK | MB_ICONERROR);

        delete graphics;
        delete bitmap;
    }
}

bool OpenFileDialog(HWND hWnd, std::string& outFileName) {
    OPENFILENAME ofn;       
    wchar_t szFile[260];    

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0'; 
    ofn.nMaxFile = sizeof(szFile);

    ofn.lpstrFilter = L"SVG Files (*.svg)\0*.svg\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        outFileName = WStringToString(ofn.lpstrFile);
        return true;
    }
    return false;
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

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"&File");
    AppendMenu(hMenu, MF_STRING, IDM_HELP_GUIDE, L"&Help");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_OPEN, L"&Open... (Ctrl+O)");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE_AS, L"&Save As... (Ctrl+S)");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_EXIT, L"E&xit");

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
        switch (LOWORD(wParam))
        {
        case IDM_FILE_OPEN:
        {
            std::string newFile;
            if (OpenFileDialog(hWnd, newFile)) {
                try {
                    svgRoot.Clear();
                    SVGRoot::CleanupStaticResources();
                    svgRoot.loadFromFile(newFile);
                    ResetView();
                    g_HasInitCamera = false;
                    UpdateCachedBitmap();
                    InvalidateRect(hWnd, NULL, FALSE);
                    std::wstring title = L"SVG Viewer - " + std::wstring(newFile.begin(), newFile.end());
                    SetWindowText(hWnd, title.c_str());
                }
                catch (const std::exception& e) {
                    MessageBoxA(hWnd, e.what(), "Error Loading File", MB_OK | MB_ICONERROR);
                }
            }
            break;
        }
        case IDM_HELP_GUIDE:
            ShowHelpGuide(hWnd);
            break;
        case IDM_FILE_SAVE_AS:
            SaveSVGImage(hWnd, &svgRoot);
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
        case 'O':
            if (GetKeyState(VK_CONTROL) < 0) {
                SendMessage(hWnd, WM_COMMAND, IDM_FILE_OPEN, 0);
            }
            break;
        case 'S':
            if (GetKeyState(VK_CONTROL) < 0) {
                SendMessage(hWnd, WM_COMMAND, IDM_FILE_SAVE_AS, 0);
            }
            break;
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