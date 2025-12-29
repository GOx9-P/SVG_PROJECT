//#include "stdafx.h"
//#include "Library.h"
//#include "SVGRoot.h"
//#include <windowsx.h>
//#include <locale>
//
//// Các biến lưu trạng thái Camera
//float g_Zoom = 1.0f;        // Độ phóng to
//float g_PanX = 0.0f;        // Dịch chuyển X
//float g_PanY = 0.0f;        // Dịch chuyển Y
//float g_Angle = 0.0f;       // Góc xoay
//float scaleTransform = 1.0f;
//float widthPicture = 0;
//bool isFlip = false;
//Bitmap* g_pCachedBitmap = NULL; // Ảnh đệm chứa nội dung SVG
//RectF   g_SvgBounds;
//bool g_HasInitCamera = false;
//
//// Các biến hỗ trợ kéo thả chuột
//bool g_IsDragging = false;
//int g_LastMouseX = 0;
//int g_LastMouseY = 0;
//
////Áp dụng biến đổi lên Graphics
//void ApplyTransform(Graphics* graphics, float w, float h) {
//
//    if (!graphics) return;
//
//    // --- BƯỚC 1: XÁC ĐỊNH TÂM ---
//    // Tìm tọa độ tâm của bức ảnh (hoặc tâm màn hình nơi ảnh đang nằm)
//    float centerX = w / 2.0f;
//    float centerY = h / 2.0f;
//
//    // --- BƯỚC 2: DỜI GỐC TỌA ĐỘ ĐẾN TÂM (PIVOT) ---
//    // Ta đưa gốc (0,0) của bút vẽ đến ngay giữa bức ảnh, cộng thêm độ lệch Pan (kéo thả)
//    graphics->TranslateTransform(centerX + g_PanX, centerY + g_PanY);
//
//    // --- BƯỚC 3: THỰC HIỆN BIẾN ĐỔI TẠI TÂM ---
//
//    // Xoay (Sẽ xoay quanh tâm vì gốc đang ở tâm)
//    graphics->RotateTransform(g_Angle);
//
//    // Zoom và Lật
//    // Kết hợp scaleTransform (lật) và g_Zoom (phóng to)
//    // Nếu scaleTransform là -1 -> scale X sẽ là -g_Zoom -> Lật & Phóng to
//    graphics->ScaleTransform(scaleTransform * g_Zoom, g_Zoom);
//   
//}
//
////Reset về mặc định (nếu cần)
//void ResetView() {
//    g_Zoom = 1.0f;
//    g_PanX = 0.0f;
//    g_PanY = 0.0f;
//    g_Angle = 0.0f;
//}
//
//SVGRoot svgRoot;
//
//void UpdateCachedBitmap() {
//    if (g_pCachedBitmap) { delete g_pCachedBitmap; g_pCachedBitmap = NULL; }
//
//    // 1. Lấy kích thước thật (Lúc này đã chính xác nhờ fix ở SVGElement)
//    g_SvgBounds = svgRoot.getBoundingBox();
//
//    // 2. Padding an toàn (Tránh bị cắt nét viền to)
//    float padding = 50.0f;
//
//    // Kiểm tra kích thước hợp lệ
//    if (g_SvgBounds.Width <= 0) g_SvgBounds.Width = 100;
//    if (g_SvgBounds.Height <= 0) g_SvgBounds.Height = 100;
//
//    // 3. Tạo Bitmap
//    int w = (int)ceil(g_SvgBounds.Width + 2 * padding);
//    int h = (int)ceil(g_SvgBounds.Height + 2 * padding);
//
//    g_pCachedBitmap = new Bitmap(w, h);
//    Graphics g(g_pCachedBitmap);
//    g.SetSmoothingMode(SmoothingModeAntiAlias);
//    g.SetInterpolationMode(InterpolationModeHighQualityBicubic); // Thêm cái này cho nét
//    g.Clear(Color::Transparent);
//
//    // 4. Dịch hình vào giữa Bitmap
//    // Logic: -X để đưa mép trái về 0, +Padding để tạo lề
//    g.TranslateTransform(-g_SvgBounds.X + padding, -g_SvgBounds.Y + padding);
//
//    // 5. Vẽ 1:1 (Bỏ qua ViewBox của Root để giữ nguyên tỉ lệ thật)
//    svgRoot.render(&g, 0, 0, true);
//}
//
//void InitCamera(float screenW, float screenH) {
//    //scaleTransform = 1.0f;
//    g_Zoom = 1.0f;
//
//    // Đảm bảo vị trí bắt đầu là góc trên-trái (0,0)
//    g_PanX = 0.0f;
//    g_PanY = 0.0f;
//
//    // Reset các biến xoay/lật
//    g_Angle = 0.0f;
//    scaleTransform = 1.0f;
//}
//
//// --- 2. OnPaint: Tính toán Pivot để xoay tại tâm nhưng vị trí vẫn đúng ---
//VOID OnPaint(HDC hdc)
//{
//    RECT rect;
//    GetClientRect(WindowFromDC(hdc), &rect);
//    float winWidth = (float)(rect.right - rect.left);
//    float winHeight = (float)(rect.bottom - rect.top);
//
//    if (winWidth == 0 || winHeight == 0) return;
//
//    // Kiểm tra cache
//    if (g_pCachedBitmap == NULL) {
//        UpdateCachedBitmap();
//        g_HasInitCamera = false;
//    }
//    if (!g_HasInitCamera && g_pCachedBitmap != NULL) {
//        InitCamera(winWidth, winHeight);
//        g_HasInitCamera = true;
//    }
//
//    Bitmap backBuffer((INT)winWidth, (INT)winHeight);
//    Graphics bufferGraphics(&backBuffer);
//    bufferGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
//    bufferGraphics.Clear(Color::White);
//
//    if (g_pCachedBitmap != NULL) {
//        GraphicsState state = bufferGraphics.Save();
//
//        // A. Lấy thông tin ViewBox (Hệ tọa độ chuẩn của file)
//        float vbX = svgRoot.getVbX();
//        float vbY = svgRoot.getVbY();
//        float vbW = svgRoot.getVbWidth();
//        float vbH = svgRoot.getVbHeight();
//
//        // --- SỬA LẠI ĐOẠN NÀY ---
//        // Fallback: Nếu không có ViewBox, ta lấy gốc là (0,0) để bảo toàn độ dịch chuyển (Transform)
//        if (vbW <= 0 || vbH <= 0) {
//            vbX = 0; // Luôn giữ gốc là 0
//            vbY = 0;
//
//            // ViewBox bao trùm hết hình vẽ (tính từ 0,0 đến mép phải dưới của hình)
//            if (g_SvgBounds.Width > 0 && g_SvgBounds.Height > 0) {
//                vbW = g_SvgBounds.X + g_SvgBounds.Width;
//                vbH = g_SvgBounds.Y + g_SvgBounds.Height;
//            }
//            else {
//                vbW = 100; vbH = 100;
//            }
//        }
//
//        // B. Tìm Tâm của Ảnh (Image Center) trong hệ tọa độ SVG (World Coords)
//        // Lưu ý: g_SvgBounds là khung bao sát mép hình vẽ thực tế
//        float cxWorld = g_SvgBounds.X + g_SvgBounds.Width / 2.0f;
//        float cyWorld = g_SvgBounds.Y + g_SvgBounds.Height / 2.0f;
//
//        // C. Tính điểm Pivot trên màn hình (Screen Coords)
//        // Đây là điểm mà TÂM ẢNH sẽ xuất hiện trên màn hình.
//        // Công thức: (Vị trí tâm ảnh so với gốc ViewBox) * Zoom + Pan
//        // Giải thích: Nếu Pan=0, và Tâm ảnh nằm lệch gốc ViewBox 1 đoạn d, 
//        // thì trên màn hình nó sẽ nằm ở d*Zoom (tức là đúng vị trí lệch, không bị dời vào giữa).
//        float pivotX = (cxWorld - vbX) * g_Zoom + g_PanX;
//        float pivotY = (cyWorld - vbY) * g_Zoom + g_PanY;
//
//        // D. Áp dụng biến đổi Matrix tại điểm Pivot
//        // 1. Dời gốc toạ độ đến Pivot
//        bufferGraphics.TranslateTransform(pivotX, pivotY);
//
//        // 2. Xoay (Lúc này gốc toạ độ đang ở Tâm ảnh, nên hình sẽ xoay tại chỗ)
//        bufferGraphics.RotateTransform(g_Angle);
//
//        // 3. Zoom & Lật
//        bufferGraphics.ScaleTransform(scaleTransform * g_Zoom, g_Zoom);
//
//        // E. Vẽ ảnh
//        // Vì gốc toạ độ đang nằm ngay Tâm Ảnh (do bước TranslateTransform),
//        // ta phải vẽ lùi lại 1/2 kích thước ảnh để ảnh nằm cân đối quanh tâm.
//        float imgW = (float)g_pCachedBitmap->GetWidth();
//        float imgH = (float)g_pCachedBitmap->GetHeight();
//
//        bufferGraphics.DrawImage(g_pCachedBitmap,
//            -imgW / 2.0f,
//            -imgH / 2.0f,
//            imgW, imgH);
//
//        bufferGraphics.Restore(state);
//    }
//
//    Graphics screenGraphics(hdc);
//    screenGraphics.DrawImage(&backBuffer, 0, 0);
//}
//
//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
//INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR lpCmdLine, INT iCmdShow) {
//    setlocale(LC_ALL, "C");
//
//    GdiplusStartupInput gdiplusStartupInput;
//    ULONG_PTR gdiplusToken;
//    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
//
//    // --- LOGIC XỬ LÝ DÒNG LỆNH ---
//    string fileName = "FireFox.svg"; // File mặc định
//
//    if (lpCmdLine && strlen(lpCmdLine) > 0) {
//        string cmdArg = lpCmdLine;
//        // Xử lý nếu đường dẫn file có dấu ngoặc kép (do chứa khoảng trắng)
//        if (cmdArg.length() >= 2 && cmdArg.front() == '\"' && cmdArg.back() == '\"') {
//            cmdArg = cmdArg.substr(1, cmdArg.length() - 2);
//        }
//        fileName = cmdArg;
//    }
//
//    try {
//        svgRoot.loadFromFile(fileName);
//    }
//    catch (const exception& e) {
//        string errorMsg = "Loi nap file: " + fileName + "\nChi tiet: " + e.what();
//        MessageBoxA(NULL, errorMsg.c_str(), "SVG Error", MB_OK | MB_ICONERROR);
//        GdiplusStartupInput gdiplusStartupInput; // Shutdown nhe truoc khi thoat
//        GdiplusShutdown(gdiplusToken);
//        return -1;
//    }
//
//    // Đăng ký lớp cửa sổ
//    WNDCLASS wndClass;
//    wndClass.style = CS_HREDRAW | CS_VREDRAW;
//    wndClass.lpfnWndProc = WndProc;
//    wndClass.cbClsExtra = 0;
//    wndClass.cbWndExtra = 0;
//    wndClass.hInstance = hInstance;
//    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
//    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
//    wndClass.lpszMenuName = NULL;
//    wndClass.lpszClassName = TEXT("GettingStarted");
//
//    RegisterClass(&wndClass);
//
//    HWND hWnd = CreateWindow(
//        TEXT("GettingStarted"),
//        (std::wstring(L"SVG Renderer - ") + (lpCmdLine && lpCmdLine[0] ? L"Custom File" : L"FireFox.svg")).c_str(),
//        WS_OVERLAPPEDWINDOW,
//        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//        NULL, NULL, hInstance, NULL);
//
//    ShowWindow(hWnd, iCmdShow);
//    UpdateWindow(hWnd);
//
//    MessageBox(hWnd,
//        L"Nhấn F1 để xem hướng dẫn sử dụng.",
//        L"Thông báo",
//        MB_OK | MB_ICONINFORMATION);
//
//    MSG msg;
//    while (GetMessage(&msg, NULL, 0, 0)) {
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//
//    SVGRoot::CleanupStaticResources();
//    GdiplusShutdown(gdiplusToken);
//    return msg.wParam;
//}
//
//
//
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    HDC          hdc;
//    PAINTSTRUCT  ps;
//
//    switch (message)
//    {
//        // 1. XỬ LÝ ZOOM (LĂN CHUỘT GIỮA)
//    case WM_MOUSEWHEEL:
//    {
//        short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
//        if (zDelta > 0) {
//            g_Zoom *= 1.1f;
//        }
//        else {
//            g_Zoom /= 1.1f;
//        }
//        InvalidateRect(hWnd, NULL, FALSE);
//        return 0;
//    }
//
//    // 2. XỬ LÝ KÉO THẢ (PAN)
//    case WM_LBUTTONDOWN:
//        g_IsDragging = true;
//        g_LastMouseX = GET_X_LPARAM(lParam);
//        g_LastMouseY = GET_Y_LPARAM(lParam);
//        SetCapture(hWnd);
//        return 0;
//
//    case WM_MOUSEMOVE:
//        if (g_IsDragging) {
//            int currentX = GET_X_LPARAM(lParam);
//            int currentY = GET_Y_LPARAM(lParam);
//
//            g_PanX += (currentX - g_LastMouseX);
//            g_PanY += (currentY - g_LastMouseY);
//
//            g_LastMouseX = currentX;
//            g_LastMouseY = currentY;
//
//            InvalidateRect(hWnd, NULL, FALSE);
//        }
//        return 0;
//
//    case WM_LBUTTONUP:
//        g_IsDragging = false;
//        ReleaseCapture();
//        return 0;
//
//        // 3. XỬ LÝ PHÍM TẮT (SHORTCUTS)
//    case WM_KEYDOWN:
//        switch (wParam)
//        {
//        case 'R': // Xoay phải
//            g_Angle += 10.0f;
//            InvalidateRect(hWnd, NULL, FALSE);
//            break;
//        case 'L': // Xoay trái
//            g_Angle -= 10.0f;
//            InvalidateRect(hWnd, NULL, FALSE);
//            break;
//        case 'F': // Lật ảnh (Flip)
//            if (scaleTransform == 1.0f)
//                scaleTransform = -1.0f;
//            else
//                scaleTransform = 1.0f;
//            InvalidateRect(hWnd, NULL, FALSE);
//            break;
//        case '0': // Reset view
//            ResetView();
//            InvalidateRect(hWnd, NULL, FALSE);
//            break;
//        case VK_F1: // Hiện hướng dẫn 
//            MessageBox(hWnd,
//                L"HƯỚNG DẪN SỬ DỤNG:\n\n"
//                L"1. Chuột trái: Nhấn và giữ để kéo thả ảnh (Pan)\n"
//                L"2. Cuộn chuột: Phóng to / Thu nhỏ (Zoom)\n"
//                L"3. Phím R: Xoay ảnh sang phải\n"
//                L"4. Phím L: Xoay ảnh sang trái\n"
//                L"5. Phím F: Lật ảnh (Flip)\n"
//                L"6. Phím 0: Khôi phục trạng thái xem mặc định\n"
//                L"7. Phím F1: Hiện bảng hướng dẫn này",
//                L"Help",
//                MB_OK | MB_ICONINFORMATION);
//            break;
//        }
//        return 0;
//
//        // 4. VẼ VÀ HỦY CỬA SỔ
//    case WM_PAINT:
//        hdc = BeginPaint(hWnd, &ps);
//        OnPaint(hdc);
//        EndPaint(hWnd, &ps);
//        return 0;
//
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//}


#include "stdafx.h"
#include "Library.h"
#include "SVGRoot.h"
#include <windowsx.h>
#include <locale>
#include <string>

// --- ĐỊNH DANH MENU ---
#define IDM_FILE_EXIT   1001
#define IDM_HELP_GUIDE  1201

// Các biến lưu trạng thái Camera
float g_Zoom = 1.0f;
float g_PanX = 0.0f;
float g_PanY = 0.0f;
float g_Angle = 0.0f;
float scaleTransform = 1.0f;
float widthPicture = 0;
bool isFlip = false;
Bitmap* g_pCachedBitmap = NULL;
RectF   g_SvgBounds;
bool g_HasInitCamera = false;

// Các biến hỗ trợ kéo thả chuột
bool g_IsDragging = false;
int g_LastMouseX = 0;
int g_LastMouseY = 0;

// --- HÀM HIỂN THỊ HƯỚNG DẪN TRỰC TIẾP ---
void ShowHelpGuide(HWND hWnd) {
    MessageBox(hWnd,
        L"HƯỚNG DẪN SỬ DỤNG:\n\n"
        L"1. Chuột trái : Kéo thả (Pan)\n"
        L"2. Cuộn chuột : Zoom (Phóng to/Thu nhỏ)\n"
        L"3. Phím R : Xoay phải 10 độ\n"
        L"4. Phím L : Xoay trái 10 độ\n"
        L"5. Phím F : Lật ảnh (Flip)\n"
        L"6. Phím 0 : Reset về mặc định\n",
        L"Help",
        MB_OK | MB_ICONINFORMATION);
}

// Áp dụng biến đổi lên Graphics
void ApplyTransform(Graphics* graphics, float w, float h) {
    if (!graphics) return;
    float centerX = w / 2.0f;
    float centerY = h / 2.0f;
    graphics->TranslateTransform(centerX + g_PanX, centerY + g_PanY);
    graphics->RotateTransform(g_Angle);
    graphics->ScaleTransform(scaleTransform * g_Zoom, g_Zoom);
}

// Reset về mặc định
void ResetView() {
    g_Zoom = 1.0f;
    g_PanX = 0.0f;
    g_PanY = 0.0f;
    g_Angle = 0.0f;
    scaleTransform = 1.0f;
}

SVGRoot svgRoot;

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

    if (g_pCachedBitmap == NULL) {
        UpdateCachedBitmap();
        g_HasInitCamera = false;
    }
    if (!g_HasInitCamera && g_pCachedBitmap != NULL) {
        InitCamera(winWidth, winHeight);
        g_HasInitCamera = true;
    }

    Bitmap backBuffer((INT)winWidth, (INT)winHeight);
    Graphics bufferGraphics(&backBuffer);
    bufferGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
    bufferGraphics.Clear(Color::White);

    if (g_pCachedBitmap != NULL) {
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

        float imgW = (float)g_pCachedBitmap->GetWidth();
        float imgH = (float)g_pCachedBitmap->GetHeight();
        bufferGraphics.DrawImage(g_pCachedBitmap, -imgW / 2.0f, -imgH / 2.0f, imgW, imgH);
        bufferGraphics.Restore(state);
    }
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
    wndClass.cbClsExtra = 0; wndClass.cbWndExtra = 0;
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
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, hMenu, hInstance, NULL);

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }

    SVGRoot::CleanupStaticResources();
    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch (message)
    {
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

    case WM_MOUSEWHEEL:
    {
        short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (zDelta > 0) g_Zoom *= 1.1f; else g_Zoom /= 1.1f;
        InvalidateRect(hWnd, NULL, FALSE); return 0;
    }
    case WM_LBUTTONDOWN:
        g_IsDragging = true;
        g_LastMouseX = GET_X_LPARAM(lParam);
        g_LastMouseY = GET_Y_LPARAM(lParam);
        SetCapture(hWnd); return 0;
    case WM_MOUSEMOVE:
        if (g_IsDragging) {
            int currentX = GET_X_LPARAM(lParam);
            int currentY = GET_Y_LPARAM(lParam);
            g_PanX += (currentX - g_LastMouseX);
            g_PanY += (currentY - g_LastMouseY);
            g_LastMouseX = currentX; g_LastMouseY = currentY;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        return 0;
    case WM_LBUTTONUP:
        g_IsDragging = false; ReleaseCapture(); return 0;
    case WM_KEYDOWN:
        switch (wParam) {
        case 'R': g_Angle += 10.0f; InvalidateRect(hWnd, NULL, FALSE); break;
        case 'L': g_Angle -= 10.0f; InvalidateRect(hWnd, NULL, FALSE); break;
        case 'F': scaleTransform = (scaleTransform == 1.0f) ? -1.0f : 1.0f; InvalidateRect(hWnd, NULL, FALSE); break;
        case '0': ResetView(); InvalidateRect(hWnd, NULL, FALSE); break;
        case VK_F1: ShowHelpGuide(hWnd); break;
        }
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps); OnPaint(hdc); EndPaint(hWnd, &ps); return 0;
    case WM_DESTROY:
        PostQuitMessage(0); return 0;
    default: return DefWindowProc(hWnd, message, wParam, lParam);
    }
}