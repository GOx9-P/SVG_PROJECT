#include "stdafx.h"
#include "Library.h"
#include "SVGRoot.h"
#include <windowsx.h>
#include <locale>

// Các biến lưu trạng thái Camera
float g_Zoom = 1.0f;        // Độ phóng to
float g_PanX = 0.0f;        // Dịch chuyển X
float g_PanY = 0.0f;        // Dịch chuyển Y
float g_Angle = 0.0f;       // Góc xoay
float scaleTransform = 1.0f;
float widthPicture = 0;
bool isFlip = false;
Bitmap* g_pCachedBitmap = NULL; // Ảnh đệm chứa nội dung SVG
RectF   g_SvgBounds;
bool g_HasInitCamera = false;

// Các biến hỗ trợ kéo thả chuột
bool g_IsDragging = false;
int g_LastMouseX = 0;
int g_LastMouseY = 0;

//Áp dụng biến đổi lên Graphics
void ApplyTransform(Graphics* graphics, float w, float h) {

    if (!graphics) return;

    // --- BƯỚC 1: XÁC ĐỊNH TÂM ---
    // Tìm tọa độ tâm của bức ảnh (hoặc tâm màn hình nơi ảnh đang nằm)
    float centerX = w / 2.0f;
    float centerY = h / 2.0f;

    // --- BƯỚC 2: DỜI GỐC TỌA ĐỘ ĐẾN TÂM (PIVOT) ---
    // Ta đưa gốc (0,0) của bút vẽ đến ngay giữa bức ảnh, cộng thêm độ lệch Pan (kéo thả)
    graphics->TranslateTransform(centerX + g_PanX, centerY + g_PanY);

    // --- BƯỚC 3: THỰC HIỆN BIẾN ĐỔI TẠI TÂM ---

    // Xoay (Sẽ xoay quanh tâm vì gốc đang ở tâm)
    graphics->RotateTransform(g_Angle);

    // Zoom và Lật
    // Kết hợp scaleTransform (lật) và g_Zoom (phóng to)
    // Nếu scaleTransform là -1 -> scale X sẽ là -g_Zoom -> Lật & Phóng to
    graphics->ScaleTransform(scaleTransform * g_Zoom, g_Zoom);
   
}

//Reset về mặc định (nếu cần)
void ResetView() {
    g_Zoom = 1.0f;
    g_PanX = 0.0f;
    g_PanY = 0.0f;
    g_Angle = 0.0f;
}

SVGRoot svgRoot;


//void UpdateCachedBitmap() {
//    // 1. Dọn dẹp bộ nhớ cũ
//    if (g_pCachedBitmap) {
//        delete g_pCachedBitmap;
//        g_pCachedBitmap = NULL;
//    }
//
//    // 2. Lấy kích thước thật của hình (Yêu cầu bạn đã làm Bước 1 & 2 ở bài trước)
//    g_SvgBounds = svgRoot.getBoundingBox();
//
//    // Kiểm tra nếu hình rỗng hoặc lỗi
//    if (g_SvgBounds.Width <= 0) g_SvgBounds.Width = 100; // Giá trị mặc định chống crash
//    if (g_SvgBounds.Height <= 0) g_SvgBounds.Height = 100;
//
//    // 3. Tạo Bitmap đệm vừa khít hình
//    // (Cộng thêm 2 pixel để tránh mất nét ở viền do làm tròn số)
//    int w = (int)ceil(g_SvgBounds.Width) + 2;
//    int h = (int)ceil(g_SvgBounds.Height) + 2;
//
//    g_pCachedBitmap = new Bitmap(w, h);
//
//    // 4. Vẽ SVG lên Bitmap đệm
//    Graphics g(g_pCachedBitmap);
//    g.SetSmoothingMode(SmoothingModeAntiAlias);
//    g.Clear(Color::Transparent); // Nền trong suốt quan trọng!
//
//    // Dời hình về góc (0,0) của Bitmap
//    // Ví dụ: Hình vẽ bắt đầu tại (100, 100), ta phải lùi (-100, -100) để nó chui vào Bitmap
//   /* g.TranslateTransform(-g_SvgBounds.X, -g_SvgBounds.Y);
//
//    svgRoot.render(&g,w,h);*/
//    g.TranslateTransform(-g_SvgBounds.X + g_CachePadding, -g_SvgBounds.Y + g_CachePadding);
//
//    // Gọi render với cờ true để VÔ HIỆU HÓA tính toán ViewBox
//    // Vì ta đã tự tính toán bounding box và translate thủ công rồi.
//    svgRoot.render(&g, 0, 0, true);
//}
void UpdateCachedBitmap() {
    if (g_pCachedBitmap) { delete g_pCachedBitmap; g_pCachedBitmap = NULL; }

    // 1. Lấy kích thước thật (Lúc này đã chính xác nhờ fix ở SVGElement)
    g_SvgBounds = svgRoot.getBoundingBox();

    // 2. Padding an toàn (Tránh bị cắt nét viền to)
    float padding = 50.0f;

    // Kiểm tra kích thước hợp lệ
    if (g_SvgBounds.Width <= 0) g_SvgBounds.Width = 100;
    if (g_SvgBounds.Height <= 0) g_SvgBounds.Height = 100;

    // 3. Tạo Bitmap
    int w = (int)ceil(g_SvgBounds.Width + 2 * padding);
    int h = (int)ceil(g_SvgBounds.Height + 2 * padding);

    g_pCachedBitmap = new Bitmap(w, h);
    Graphics g(g_pCachedBitmap);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic); // Thêm cái này cho nét
    g.Clear(Color::Transparent);

    // 4. Dịch hình vào giữa Bitmap
    // Logic: -X để đưa mép trái về 0, +Padding để tạo lề
    g.TranslateTransform(-g_SvgBounds.X + padding, -g_SvgBounds.Y + padding);

    // 5. Vẽ 1:1 (Bỏ qua ViewBox của Root để giữ nguyên tỉ lệ thật)
    svgRoot.render(&g, 0, 0, true);
}

//void InitCamera(float screenW, float screenH) {
//    // Nếu file không có ViewBox, ta dùng BoundingBox để thay thế
//    float targetX = svgRoot.getVbX();
//    float targetY = svgRoot.getVbY();
//    float targetW = svgRoot.getVbWidth();
//    float targetH = svgRoot.getVbHeight();
//
//    // Fallback nếu không có viewBox
//    if (targetW <= 0 || targetH <= 0) {
//        targetX = g_SvgBounds.X;
//        targetY = g_SvgBounds.Y;
//        targetW = g_SvgBounds.Width;
//        targetH = g_SvgBounds.Height;
//    }
//
//    if (targetW > 0 && targetH > 0) {
//        // 1. Tính Zoom để vừa khít màn hình (Fit View)
//        float scaleX = screenW / targetW;
//        float scaleY = screenH / targetH;
//        g_Zoom = min(scaleX, scaleY); // Chọn tỉ lệ nhỏ hơn để hình nằm trọn trong màn hình
//
//        // Giới hạn zoom không quá bé hoặc quá to
//        if (g_Zoom == 0) g_Zoom = 1.0f;
//
//        // 2. Tính Pan để căn giữa màn hình
//        // Công thức: Đưa điểm giữa ViewBox về điểm giữa Màn hình
//
//        // Hiện tại hình trong Cache đang nằm ở toạ độ: (Bounds.X - Bounds.X + Padding) = Padding
//        // Tức là gốc (0,0) của nội dung đang nằm lệch đi một đoạn.
//
//        // Khoảng cách từ BoundingBox đến ViewBox (khoảng trắng thừa)
//        float offsetX = targetX - g_SvgBounds.X;
//        float offsetY = targetY - g_SvgBounds.Y;
//
//        // Tính kích thước hiển thị thực tế
//        float displayW = targetW * g_Zoom;
//        float displayH = targetH * g_Zoom;
//
//        // Căn giữa: (Màn hình - Hình) / 2
//        float centerX = (screenW - displayW) / 2.0f;
//        float centerY = (screenH - displayH) / 2.0f;
//
//        // Gán ngược lại vào biến Pan toàn cục
//        // Logic: Pan = Vị trí mong muốn - (Vị trí thực tế * Zoom)
//        // Vị trí thực tế trong Bitmap Cache là (Padding - offsetX, Padding - offsetY)
//
//        // Tuy nhiên, do hàm ApplyTransform của chúng ta đang dùng Pivot tại tâm ảnh Bitmap
//        // nên ta tính toán đơn giản hơn:
//
//        // Reset về 0 để tính cho dễ
//        g_PanX = 0; g_PanY = 0;
//
//        // Ta muốn ViewBox nằm giữa màn hình.
//        // Hiện tại OnPaint vẽ tâm Bitmap vào giữa màn hình.
//        // Ta chỉ cần dịch một đoạn bằng độ lệch tâm giữa ViewBox và BoundingBox
//
//        float bmpCenterX = g_SvgBounds.Width / 2.0f;
//        float bmpCenterY = g_SvgBounds.Height / 2.0f;
//
//        float viewBoxCenterX = (targetX - g_SvgBounds.X) + targetW / 2.0f;
//        float viewBoxCenterY = (targetY - g_SvgBounds.Y) + targetH / 2.0f;
//
//        g_PanX = (bmpCenterX - viewBoxCenterX) * g_Zoom; // Dịch ngược lại để khớp tâm
//        g_PanY = (bmpCenterY - viewBoxCenterY) * g_Zoom;
//    }
//}
//
//VOID OnPaint(HDC hdc)
//{
//   // Ref: https://docs.microsoft.com/en-us/windows/desktop/gdiplus/-gdiplus-getting-started-use
//   /*Graphics graphics(hdc);
//   Pen      pen(Color(255, 0, 0, 255));
//   graphics.DrawLine(&pen, 0, 0, 200, 100);*/
//
//    /*Graphics graphics(hdc);
//    ApplyTransform(&graphics);
//    svgRoot.render(&graphics);*/
//
//   
//    RECT rect;
//    GetClientRect(WindowFromDC(hdc), &rect);
//    float winWidth = (float)(rect.right - rect.left);
//    float winHeight = (float)(rect.bottom - rect.top);
//
//    if (winWidth == 0 || winHeight == 0) return;
//
//    // --- 1. KIỂM TRA & TẠO CACHE ---
//    // Nếu chưa có ảnh đệm (lần đầu chạy hoặc mới load file), tạo ngay
//    if (g_pCachedBitmap == NULL) {
//        UpdateCachedBitmap();
//        g_HasInitCamera = false;
//    }
//    if (!g_HasInitCamera && g_pCachedBitmap != NULL) {
//        InitCamera(winWidth, winHeight);
//        g_HasInitCamera = true;
//    }
//
//    // --- 2. TẠO DOUBLE BUFFER (Màn hình đệm) ---
//    Bitmap backBuffer((INT)winWidth, (INT)winHeight);
//    Graphics bufferGraphics(&backBuffer);
//
//    bufferGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
//    bufferGraphics.Clear(Color::White); // Xóa nền trắng
//
//    if (g_pCachedBitmap != NULL) {
//        GraphicsState state = bufferGraphics.Save(); // Lưu trạng thái gốc
//
//        // Lấy kích thước thật của ảnh cache
//        float imgW = (float)g_pCachedBitmap->GetWidth();
//        float imgH = (float)g_pCachedBitmap->GetHeight();
//        ApplyTransform(&bufferGraphics, imgW, imgH);
//
//        // --- 5. VẼ ẢNH ---
//        // Vì gốc tọa độ đang nằm ở TÂM ẢNH, ta phải vẽ lùi lại 1/2 kích thước 
//        // để hình ảnh hiển thị cân đối quanh tâm đó.
//        // Kết quả: Góc trái trên của ảnh sẽ nằm đúng vị trí (0,0) nếu chưa Pan.
//        bufferGraphics.DrawImage(g_pCachedBitmap,
//            -imgW / 2.0f,
//            -imgH / 2.0f,
//            imgW, imgH);
//
//        bufferGraphics.Restore(state); // Khôi phục trạng thái
//    }
//    // --- 6. XUẤT RA MÀN HÌNH ---
//    Graphics screenGraphics(hdc);
//    screenGraphics.DrawImage(&backBuffer, 0, 0);
//}








void InitCamera(float screenW, float screenH) {
    //float targetW = svgRoot.getVbWidth();
    //float targetH = svgRoot.getVbHeight();

    //// Fallback nếu không có ViewBox
    //if (targetW <= 0 || targetH <= 0) {
    //    targetW = g_SvgBounds.Width;
    //    targetH = g_SvgBounds.Height;
    //}

    //if (targetW > 0 && targetH > 0) {
    //    float scaleX = screenW / targetW;
    //    float scaleY = screenH / targetH;

    //    // Fit hình vào màn hình
    //    g_Zoom = min(scaleX, scaleY);
    //    g_Zoom *= 0.95f; // Thu nhỏ xíu để chừa lề

    //    if (g_Zoom == 0) g_Zoom = 1.0f;
    //}

    //// QUAN TRỌNG: Đặt Pan = 0.
    //// Kết hợp với logic OnPaint bên dưới, nó sẽ neo góc ViewBox vào góc màn hình (0,0).
    //g_PanX = 0.0f;
    //g_PanY = 0.0f;

    //// Reset các biến biến đổi
    //g_Angle = 0.0f;
    //scaleTransform = 1.0f;
    g_Zoom = 1.0f;

    // Đảm bảo vị trí bắt đầu là góc trên-trái (0,0)
    g_PanX = 0.0f;
    g_PanY = 0.0f;

    // Reset các biến xoay/lật
    g_Angle = 0.0f;
    scaleTransform = 1.0f;
}

// --- 2. OnPaint: Tính toán Pivot để xoay tại tâm nhưng vị trí vẫn đúng ---
VOID OnPaint(HDC hdc)
{
    RECT rect;
    GetClientRect(WindowFromDC(hdc), &rect);
    float winWidth = (float)(rect.right - rect.left);
    float winHeight = (float)(rect.bottom - rect.top);

    if (winWidth == 0 || winHeight == 0) return;

    // Kiểm tra cache
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

        // A. Lấy thông tin ViewBox (Hệ tọa độ chuẩn của file)
        float vbX = svgRoot.getVbX();
        float vbY = svgRoot.getVbY();
        float vbW = svgRoot.getVbWidth();
        float vbH = svgRoot.getVbHeight();

        // --- SỬA LẠI ĐOẠN NÀY ---
        // Fallback: Nếu không có ViewBox, ta lấy gốc là (0,0) để bảo toàn độ dịch chuyển (Transform)
        if (vbW <= 0 || vbH <= 0) {
            vbX = 0; // Luôn giữ gốc là 0
            vbY = 0;

            // ViewBox bao trùm hết hình vẽ (tính từ 0,0 đến mép phải dưới của hình)
            if (g_SvgBounds.Width > 0 && g_SvgBounds.Height > 0) {
                vbW = g_SvgBounds.X + g_SvgBounds.Width;
                vbH = g_SvgBounds.Y + g_SvgBounds.Height;
            }
            else {
                vbW = 100; vbH = 100;
            }
        }

        // B. Tìm Tâm của Ảnh (Image Center) trong hệ tọa độ SVG (World Coords)
        // Lưu ý: g_SvgBounds là khung bao sát mép hình vẽ thực tế
        float cxWorld = g_SvgBounds.X + g_SvgBounds.Width / 2.0f;
        float cyWorld = g_SvgBounds.Y + g_SvgBounds.Height / 2.0f;

        // C. Tính điểm Pivot trên màn hình (Screen Coords)
        // Đây là điểm mà TÂM ẢNH sẽ xuất hiện trên màn hình.
        // Công thức: (Vị trí tâm ảnh so với gốc ViewBox) * Zoom + Pan
        // Giải thích: Nếu Pan=0, và Tâm ảnh nằm lệch gốc ViewBox 1 đoạn d, 
        // thì trên màn hình nó sẽ nằm ở d*Zoom (tức là đúng vị trí lệch, không bị dời vào giữa).
        float pivotX = (cxWorld - vbX) * g_Zoom + g_PanX;
        float pivotY = (cyWorld - vbY) * g_Zoom + g_PanY;

        // D. Áp dụng biến đổi Matrix tại điểm Pivot
        // 1. Dời gốc toạ độ đến Pivot
        bufferGraphics.TranslateTransform(pivotX, pivotY);

        // 2. Xoay (Lúc này gốc toạ độ đang ở Tâm ảnh, nên hình sẽ xoay tại chỗ)
        bufferGraphics.RotateTransform(g_Angle);

        // 3. Zoom & Lật
        bufferGraphics.ScaleTransform(scaleTransform * g_Zoom, g_Zoom);

        // E. Vẽ ảnh
        // Vì gốc toạ độ đang nằm ngay Tâm Ảnh (do bước TranslateTransform),
        // ta phải vẽ lùi lại 1/2 kích thước ảnh để ảnh nằm cân đối quanh tâm.
        float imgW = (float)g_pCachedBitmap->GetWidth();
        float imgH = (float)g_pCachedBitmap->GetHeight();

        bufferGraphics.DrawImage(g_pCachedBitmap,
            -imgW / 2.0f,
            -imgH / 2.0f,
            imgW, imgH);

        bufferGraphics.Restore(state);
    }

    Graphics screenGraphics(hdc);
    screenGraphics.DrawImage(&backBuffer, 0, 0);
}








LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{

    setlocale(LC_ALL, "C");

   HWND                hWnd;
   MSG                 msg;
   WNDCLASS            wndClass;
   GdiplusStartupInput gdiplusStartupInput;
   ULONG_PTR           gdiplusToken;



   //// Read XML
   //xml_document<> doc;
   //xml_node<> *rootNode;
   //// Read the xml file into a vector
   //ifstream file("sample.svg");
   //vector<char> buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
   //buffer.push_back('\0');
   //// Parse the buffer using the xml file parsing library into doc 
   //doc.parse<0>(&buffer[0]);

   //rootNode = doc.first_node();
   //xml_node<> *node = rootNode->first_node();

   //while (node != NULL) {
	  // char *nodeName = node->name();
	  // xml_attribute<> *firstAttribute = node->first_attribute();
	  // char *attributeName = firstAttribute->name();
	  // char *attributeValue = firstAttribute->value();
	  // xml_attribute<> *secondAttribute = firstAttribute->next_attribute();
	  // // Set breakpoint here to view value
	  // // Ref: http://rapidxml.sourceforge.net/manual.html
	  // node = node->next_sibling();
   //}

   // Initialize GDI+.
   GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

   try {
       svgRoot.loadFromFile("svg-14.svg");
       if (g_pCachedBitmap) { delete g_pCachedBitmap; g_pCachedBitmap = NULL; }
       g_HasInitCamera = false;
       ResetView();
   }
   catch (const exception& e) {
       MessageBoxA(NULL, e.what(), "Error!!!", MB_OK | MB_ICONERROR);
       GdiplusShutdown(gdiplusToken);
       return -1;
   }
   
   wndClass.style          = CS_HREDRAW | CS_VREDRAW;
   wndClass.lpfnWndProc    = WndProc;
   wndClass.cbClsExtra     = 0;
   wndClass.cbWndExtra     = 0;
   wndClass.hInstance      = hInstance;
   wndClass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
   wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
   wndClass.lpszMenuName   = NULL;
   wndClass.lpszClassName  = TEXT("GettingStarted");
   
   RegisterClass(&wndClass);
   
   hWnd = CreateWindow(
      TEXT("GettingStarted"),   // window class name
      TEXT("SVG Demo"),  // window caption
      WS_OVERLAPPEDWINDOW,      // window style
      CW_USEDEFAULT,            // initial x position
      CW_USEDEFAULT,            // initial y position
      CW_USEDEFAULT,            // initial x size
      CW_USEDEFAULT,            // initial y size
      NULL,                     // parent window handle
      NULL,                     // window menu handle
      hInstance,                // program instance handle
      NULL);                    // creation parameters
      
   ShowWindow(hWnd, iCmdShow);
   UpdateWindow(hWnd);
   
   while(GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   
   SVGRoot::CleanupStaticResources();

   GdiplusShutdown(gdiplusToken);
   return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
   WPARAM wParam, LPARAM lParam)
{
   HDC          hdc;
   PAINTSTRUCT  ps;
   
   switch(message)
   {
    //XỬ LÝ ZOOM (LĂN CHUỘT GIỮA)
   case WM_MOUSEWHEEL:
   {
       short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
       // Lăn lên -> Phóng to, Lăn xuống -> Thu nhỏ
       if (zDelta > 0) {
           g_Zoom *= 1.1f;
       }
       else {
           g_Zoom /= 1.1f;
       }
       //Yêu cầu vẽ lại màn hình ngay lập tức
       InvalidateRect(hWnd, NULL, FALSE);
       return 0;
   }
   //XỬ LÝ KÉO THẢ
   case WM_LBUTTONDOWN: // Nhấn chuột trái
       g_IsDragging = true;
       g_LastMouseX = GET_X_LPARAM(lParam); // Lưu vị trí bắt đầu
       g_LastMouseY = GET_Y_LPARAM(lParam);
       SetCapture(hWnd); // Bắt chuột không cho chạy ra ngoài
       return 0;

   case WM_MOUSEMOVE: // Di chuyển chuột
       if (g_IsDragging) {
           int currentX = GET_X_LPARAM(lParam);
           int currentY = GET_Y_LPARAM(lParam);

           // Tính khoảng cách đã di chuyển
           int dx = currentX - g_LastMouseX;
           int dy = currentY - g_LastMouseY;

           // Cộng dồn vào vị trí Pan toàn cục
           g_PanX += dx;
           g_PanY += dy;

           // Cập nhật vị trí chuột cũ
           g_LastMouseX = currentX;
           g_LastMouseY = currentY;

           // Vẽ lại
           InvalidateRect(hWnd, NULL, FALSE);
       }
       return 0;

   case WM_LBUTTONUP: // Nhả chuột trái
       g_IsDragging = false;
       ReleaseCapture();
       return 0;

       //XỬ LÝ XOAY ////// THEM FLIP
   case WM_KEYDOWN:
       switch (wParam) {
       case 'R': // Nhấn R để xoay phải
           g_Angle += 10.0f;
           InvalidateRect(hWnd, NULL, FALSE);
           break;
       case 'L': // Nhấn L để xoay trái
           g_Angle -= 10.0f;
           InvalidateRect(hWnd, NULL, FALSE);
           break;
       case 'F':
           if(scaleTransform==1.0f)
           scaleTransform = -1.0f;
           else
           {
               scaleTransform = 1.0f;
           }
           InvalidateRect(hWnd, NULL, FALSE); 
           break;
       case '0': // Nhấn 0 để Reset
           ResetView();
           InvalidateRect(hWnd, NULL, FALSE);
           break;
       }
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
} // WndProc

//#include "stdafx.h"
//#include "Library.h"
//#include "SVGRoot.h"
//#include <windowsx.h>
//#include <locale>
//#include <string>
//
//using namespace std;
//using namespace Gdiplus;
//
//// --- Biến lưu trạng thái Camera ---
//float g_Zoom = 1.0f;        // Độ phóng to
//float g_PanX = 0.0f;        // Dịch chuyển X
//float g_PanY = 0.0f;        // Dịch chuyển Y
//float g_Angle = 0.0f;       // Góc xoay
//
//// --- Biến hỗ trợ kéo thả chuột ---
//bool g_IsDragging = false;
//int g_LastMouseX = 0;
//int g_LastMouseY = 0;
//
//// Áp dụng biến đổi lên Graphics theo thứ tự: Dịch chuyển -> Xoay -> Phóng to
//void ApplyTransform(Graphics* graphics) {
//    if (!graphics) return;
//    graphics->TranslateTransform(g_PanX, g_PanY);
//    graphics->RotateTransform(g_Angle);
//    graphics->ScaleTransform(g_Zoom, g_Zoom);
//}
//
//// Reset về trạng thái mặc định
//void ResetView() {
//    g_Zoom = 1.0f;
//    g_PanX = 0.0f;
//    g_PanY = 0.0f;
//    g_Angle = 0.0f;
//}
//
//SVGRoot svgRoot;
//
//const float k_SupersampleScale = 2.0f;
//
//VOID OnPaint(HDC hdc) {
//    //
//    RECT rect;
//    GetClientRect(WindowFromDC(hdc), &rect);
//    int logicalWidth = rect.right - rect.left;
//    int logicalHeight = rect.bottom - rect.top;
//
//    if (logicalWidth == 0 || logicalHeight == 0) return;
//
//    // 2. TẠO BUFFER LỚN HƠN (Supersampling)
//    // Kích thước vật lý của buffer sẽ nhân với hệ số k_SupersampleScale
//    int physicalWidth = (int)(logicalWidth * k_SupersampleScale);
//    int physicalHeight = (int)(logicalHeight * k_SupersampleScale);
//
//    Bitmap buffer(physicalWidth, physicalHeight);
//    Graphics bufferGraphics(&buffer);
//
//    // Cài đặt chất lượng vẽ cao nhất cho buffer
//    bufferGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
//    bufferGraphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
//    bufferGraphics.SetPixelOffsetMode(PixelOffsetModeHighQuality); // Quan trọng để map pixel chuẩn
//    bufferGraphics.Clear(Color::White);
//
//    // 3. ÁP DỤNG SCALE SUPERSAMPLING
//    // Phóng to toàn bộ hệ tọa độ vẽ lên k lần.
//    // Việc này phải làm ĐẦU TIÊN để mọi lệnh vẽ sau đó (bao gồm cả Pan/Zoom của người dùng)
//    // đều được nhân lên theo tỷ lệ này.
//    bufferGraphics.ScaleTransform(k_SupersampleScale, k_SupersampleScale);
//
//    // 4. Áp dụng các biến đổi View (Pan, Zoom, Rotate) của người dùng
//    // Lưu ý: Hàm này sẽ cộng dồn vào matrix đã scale ở bước 3
//    ApplyTransform(&bufferGraphics);
//
//    // 5. Vẽ SVG
//    // Ta vẫn truyền logicalWidth/Height vì hệ tọa độ Graphics đã được Scale rồi.
//    // SVG sẽ "nghĩ" là nó đang vẽ trên màn hình bình thường, nhưng thực tế là đang vẽ lên buffer to.
//    svgRoot.render(&bufferGraphics, logicalWidth, logicalHeight);
//
//    // 6. DOWN-SAMPLING (Vẽ thu nhỏ ra màn hình)
//    Graphics screenGraphics(hdc);
//
//    // Cực kỳ quan trọng: Chế độ nội suy khi thu nhỏ ảnh
//    // HighQualityBicubic giúp trộn màu pixel mượt mà nhất khi thu nhỏ từ buffer to xuống màn hình nhỏ.
//    screenGraphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
//
//    // Vẽ buffer (to) vào khung hình màn hình (nhỏ)
//    screenGraphics.DrawImage(&buffer, 0, 0, logicalWidth, logicalHeight);
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
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
//    HDC hdc;
//    PAINTSTRUCT ps;
//
//    switch (message) {
//    case WM_MOUSEWHEEL: {
//        short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
//        if (zDelta > 0) g_Zoom *= 1.1f;
//        else g_Zoom /= 1.1f;
//        InvalidateRect(hWnd, NULL, FALSE);
//        return 0;
//    }
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
//            g_PanX += (currentX - g_LastMouseX);
//            g_PanY += (currentY - g_LastMouseY);
//            g_LastMouseX = currentX;
//            g_LastMouseY = currentY;
//            InvalidateRect(hWnd, NULL, FALSE);
//        }
//        return 0;
//
//    case WM_LBUTTONUP:
//        g_IsDragging = false;
//        ReleaseCapture();
//        return 0;
//
//    case WM_KEYDOWN:
//        switch (wParam) {
//        case 'R': g_Angle += 10.0f; break;
//        case 'L': g_Angle -= 10.0f; break;
//        case '0': ResetView(); break;
//        }
//        InvalidateRect(hWnd, NULL, FALSE);
//        return 0;
//
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