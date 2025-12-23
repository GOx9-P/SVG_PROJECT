//#include "stdafx.h"
//#include "SVGLinearGradient.h"
//#include <vector>
//#include <algorithm>
//
//SVGLinearGradient::SVGLinearGradient() {
//    // Giá trị mặc định theo chuẩn SVG
//    x1 = 0.0f; // 0%
//    y1 = 0.0f; // 0%
//    x2 = 1.0f; // 100%
//    y2 = 0.0f; // 0%
//}
//
//void SVGLinearGradient::parseAttributes(xml_node<>* node) {
//    SVGGradient::parseAttributes(node); // Parse stops và ID
//
//    auto parseVal = [&](xml_attribute<>* attr, float defVal) {
//        if (!attr) return defVal;
//        string val = attr->value();
//        if (val.back() == '%') {
//            val.pop_back();
//            return (float)atof(val.c_str()) / 100.0f;
//        }
//        return (float)atof(val.c_str());
//        };
//
//    x1 = parseVal(node->first_attribute("x1"), 0.0f);
//    y1 = parseVal(node->first_attribute("y1"), 0.0f);
//    x2 = parseVal(node->first_attribute("x2"), 1.0f);
//    y2 = parseVal(node->first_attribute("y2"), 0.0f);
//}
//
//Gdiplus::LinearGradientBrush* SVGLinearGradient::createBrush(const Gdiplus::RectF& bounds) {
//    if (stops.empty()) return nullptr;
//
//    // 1. Tính toán điểm bắt đầu và kết thúc
//    Gdiplus::PointF startPoint, endPoint;
//
//    if (gradientUnits == "userSpaceOnUse") {
//        startPoint.X = x1;
//        startPoint.Y = y1;
//        endPoint.X = x2;
//        endPoint.Y = y2;
//    }
//    else { // objectBoundingBox (Mặc định)
//        startPoint.X = bounds.X + x1 * bounds.Width;
//        startPoint.Y = bounds.Y + y1 * bounds.Height;
//        endPoint.X = bounds.X + x2 * bounds.Width;
//        endPoint.Y = bounds.Y + y2 * bounds.Height;
//    }
//
//    // 2. Tạo Brush tạm
//    Gdiplus::LinearGradientBrush* brush = new Gdiplus::LinearGradientBrush(
//        startPoint, endPoint,
//        Gdiplus::Color(255, 0, 0, 0),
//        Gdiplus::Color(255, 255, 255, 255)
//    );
//
//    // 3. Chuẩn bị màu trộn (Interpolation Colors)
//    int count = (int)stops.size();
//    std::vector<Gdiplus::Color> gdiColors(count);
//    std::vector<REAL> blendPositions(count);
//
//    for (int i = 0; i < count; i++) {
//        SVGColor c = stops[i].getStopColor();
//        BYTE alpha = (BYTE)(stops[i].getStopOpacity() * c.getA());
//
//        gdiColors[i] = Gdiplus::Color(alpha, c.getR(), c.getG(), c.getB());
//        blendPositions[i] = (REAL)stops[i].getOffset();
//    }
//
//    // Fix lỗi GDI+: blendPositions bắt buộc phải có 0.0 và 1.0
//    if (blendPositions.front() > 0.001f) {
//        blendPositions.insert(blendPositions.begin(), 0.0f);
//        gdiColors.insert(gdiColors.begin(), gdiColors.front());
//        count++;
//    }
//    if (blendPositions.back() < 0.999f) {
//        blendPositions.push_back(1.0f);
//        gdiColors.push_back(gdiColors.back());
//        count++;
//    }
//
//    brush->SetInterpolationColors(gdiColors.data(), blendPositions.data(), count);
//
//    // 4. Áp dụng Transform
//    if (gradientTransform) {
//        brush->MultiplyTransform(gradientTransform);
//    }
//
//    // 5. Spread Method
//    if (spreadMethod == "reflect") brush->SetWrapMode(Gdiplus::WrapModeTileFlipXY);
//    else if (spreadMethod == "repeat") brush->SetWrapMode(Gdiplus::WrapModeTile);
//    else brush->SetWrapMode(Gdiplus::WrapModeClamp);
//
//    return brush;
//}

//#include "stdafx.h"
//#include "SVGLinearGradient.h"
//#include <vector>
//#include <algorithm>
//
//SVGLinearGradient::SVGLinearGradient() {
//    // Giá trị mặc định theo chuẩn SVG
//    x1 = 0.0f; // 0%
//    y1 = 0.0f; // 0%
//    x2 = 1.0f; // 100%
//    y2 = 0.0f; // 0%
//}
//
//void SVGLinearGradient::parseAttributes(xml_node<>* node) {
//    SVGGradient::parseAttributes(node); // Parse stops và ID
//
//    auto parseVal = [&](xml_attribute<>* attr, float defVal) {
//        if (!attr) return defVal;
//        string val = attr->value();
//        if (val.back() == '%') {
//            val.pop_back();
//            return (float)atof(val.c_str()) / 100.0f;
//        }
//        return (float)atof(val.c_str());
//        };
//
//    x1 = parseVal(node->first_attribute("x1"), 0.0f);
//    y1 = parseVal(node->first_attribute("y1"), 0.0f);
//    x2 = parseVal(node->first_attribute("x2"), 1.0f);
//    y2 = parseVal(node->first_attribute("y2"), 0.0f);
//}
//
//Gdiplus::Brush* SVGLinearGradient::createBrush(const Gdiplus::RectF& bounds){
//    if (stops.empty()) return nullptr;
//
//    // 1. Tính toán điểm bắt đầu và kết thúc
//    Gdiplus::PointF startPoint, endPoint;
//
//    if (gradientUnits == "userSpaceOnUse") {
//        startPoint.X = x1;
//        startPoint.Y = y1;
//        endPoint.X = x2;
//        endPoint.Y = y2;
//    }
//    else { // objectBoundingBox (Mặc định)
//        startPoint.X = bounds.X + x1 * bounds.Width;
//        startPoint.Y = bounds.Y + y1 * bounds.Height;
//        endPoint.X = bounds.X + x2 * bounds.Width;
//        endPoint.Y = bounds.Y + y2 * bounds.Height;
//    }
//
//    // 2. Tạo Brush tạm
//    Gdiplus::LinearGradientBrush* brush = new Gdiplus::LinearGradientBrush(
//        startPoint, endPoint,
//        Gdiplus::Color(255, 0, 0, 0),
//        Gdiplus::Color(255, 255, 255, 255)
//    );
//
//    // 3. Chuẩn bị màu trộn (Interpolation Colors)
//    int count = (int)stops.size();
//    std::vector<Gdiplus::Color> gdiColors(count);
//    std::vector<REAL> blendPositions(count);
//
//    for (int i = 0; i < count; i++) {
//        SVGColor c = stops[i].getStopColor();
//        BYTE alpha = (BYTE)(stops[i].getStopOpacity() * c.getA());
//
//        gdiColors[i] = Gdiplus::Color(alpha, c.getR(), c.getG(), c.getB());
//        blendPositions[i] = (REAL)stops[i].getOffset();
//    }
//
//    // Fix lỗi GDI+: blendPositions bắt buộc phải có 0.0 và 1.0
//    if (blendPositions.front() > 0.001f) {
//        blendPositions.insert(blendPositions.begin(), 0.0f);
//        gdiColors.insert(gdiColors.begin(), gdiColors.front());
//        count++;
//    }
//    if (blendPositions.back() < 0.999f) {
//        blendPositions.push_back(1.0f);
//        gdiColors.push_back(gdiColors.back());
//        count++;
//    }
//
//    brush->SetInterpolationColors(gdiColors.data(), blendPositions.data(), count);
//
//    // 4. Áp dụng Transform
//    if (gradientTransform) {
//        brush->MultiplyTransform(gradientTransform);
//    }
//
//    // 5. Spread Method
//    if (spreadMethod == "reflect") brush->SetWrapMode(Gdiplus::WrapModeTileFlipXY);
//    else if (spreadMethod == "repeat") brush->SetWrapMode(Gdiplus::WrapModeTile);
//    else brush->SetWrapMode(Gdiplus::WrapModeClamp);
//
//    return brush;
//}

#include "stdafx.h"
#include "SVGLinearGradient.h"
#include "SVGStop.h"
#include <vector>
#include <algorithm>

SVGLinearGradient::SVGLinearGradient() {
    x1 = 0.0f; y1 = 0.0f;
    x2 = 1.0f; y2 = 0.0f;
}

void SVGLinearGradient::parseAttributes(xml_node<>* node) {
    SVGGradient::parseAttributes(node);

    // Re-parse stops con nếu cần (đảm bảo tính đồng bộ)
    if (node->first_node("stop")) {
        stops.clear();
        for (xml_node<>* child = node->first_node("stop"); child; child = child->next_sibling("stop")) {
            SVGStop stop;
            stop.parseAttributes(child);
            stops.push_back(stop);
        }
    }

    auto parseVal = [&](xml_attribute<>* attr, float defVal) {
        if (!attr) return defVal;
        string val = attr->value();
        if (val.back() == '%') {
            val.pop_back();
            return (float)atof(val.c_str()) / 100.0f;
        }
        return (float)atof(val.c_str());
        };

    x1 = parseVal(node->first_attribute("x1"), 0.0f);
    y1 = parseVal(node->first_attribute("y1"), 0.0f);
    x2 = parseVal(node->first_attribute("x2"), 1.0f);
    y2 = parseVal(node->first_attribute("y2"), 0.0f);
}

Gdiplus::Brush* SVGLinearGradient::createBrush(const Gdiplus::RectF& boundingBox) {
    if (stops.empty()) return new Gdiplus::SolidBrush(Gdiplus::Color::Black);

    // 1. Tính toán điểm Start/End theo BoundingBox
    Gdiplus::PointF startPoint, endPoint;

    if (gradientUnits == "userSpaceOnUse") {
        startPoint = Gdiplus::PointF(x1, y1);
        endPoint = Gdiplus::PointF(x2, y2);
    }
    else {
        // objectBoundingBox
        startPoint = Gdiplus::PointF(
            boundingBox.X + x1 * boundingBox.Width,
            boundingBox.Y + y1 * boundingBox.Height
        );
        endPoint = Gdiplus::PointF(
            boundingBox.X + x2 * boundingBox.Width,
            boundingBox.Y + y2 * boundingBox.Height
        );
    }

    // 2. Tạo LinearGradientBrush cơ bản
    // Lưu ý: Màu trong constructor chỉ là dummy, sẽ bị ghi đè bởi SetInterpolationColors
    Gdiplus::LinearGradientBrush* brush = new Gdiplus::LinearGradientBrush(
        startPoint, endPoint,
        Gdiplus::Color::Black, Gdiplus::Color::White
    );

    // 3. Áp dụng Transform (Nếu có)
    if (gradientTransform) {
        // LinearGradientBrush hỗ trợ transform trực tiếp
        brush->MultiplyTransform(gradientTransform);
    }

    // 4. Xử lý Stops và Padding (FIX CHO TH2)
    // Gradient #l: stops từ 0.17 -> 0.6.
    // Cần điền đầy từ 0.0 -> 0.17 bằng màu đầu, và 0.6 -> 1.0 bằng màu cuối.

    struct StopData { REAL pos; Gdiplus::Color col; };
    std::vector<StopData> sortedStops;

    for (const auto& stop : stops) {
        SVGColor c = stop.getStopColor();
        BYTE a = (BYTE)(stop.getStopOpacity() * c.getA());
        REAL pos = stop.getOffset();

        // Clamp offset trong khoảng 0-1
        if (pos < 0.0f) pos = 0.0f;
        if (pos > 1.0f) pos = 1.0f;

        sortedStops.push_back({ pos, Gdiplus::Color(a, c.getR(), c.getG(), c.getB()) });
    }

    // Sort stops tăng dần theo offset
    std::sort(sortedStops.begin(), sortedStops.end(),
        [](const StopData& a, const StopData& b) { return a.pos < b.pos; });

    // --- PADDING LOGIC ---
    if (!sortedStops.empty()) {
        // Pad Start: Nếu stop đầu tiên > 0, chêm thêm stop tại 0 với cùng màu
        if (sortedStops.front().pos > 0.001f) {
            sortedStops.insert(sortedStops.begin(), { 0.0f, sortedStops.front().col });
        }
        else {
            // Đảm bảo stop đầu tiên chính xác là 0.0f (tránh lỗi float nhỏ)
            sortedStops.front().pos = 0.0f;
        }

        // Pad End: Nếu stop cuối cùng < 1, chêm thêm stop tại 1 với cùng màu
        if (sortedStops.back().pos < 0.999f) {
            sortedStops.push_back({ 1.0f, sortedStops.back().col });
        }
        else {
            sortedStops.back().pos = 1.0f;
        }
    }

    // 5. Convert sang mảng GDI+
    int count = (int)sortedStops.size();
    if (count >= 2) { // GDI+ yêu cầu tối thiểu 2 màu
        Gdiplus::Color* colors = new Gdiplus::Color[count];
        REAL* positions = new REAL[count];

        for (int i = 0; i < count; i++) {
            colors[i] = sortedStops[i].col;
            positions[i] = sortedStops[i].pos;
        }

        brush->SetInterpolationColors(colors, positions, count);

        delete[] colors;
        delete[] positions;
    }

    // 6. Wrap Mode (Quan trọng cho TH2)
    // SVG mặc định là "pad" (kéo dài màu biên).
    // GDI+ TileFlipXY kết hợp với Padding ở bước 4 sẽ giả lập Pad tốt nhất.
    // Nếu dùng Tile thường, nó sẽ lặp lại từ đầu (0.0) ngay sau khi hết (1.0).
    brush->SetWrapMode(Gdiplus::WrapModeTileFlipXY);

    return brush;
}