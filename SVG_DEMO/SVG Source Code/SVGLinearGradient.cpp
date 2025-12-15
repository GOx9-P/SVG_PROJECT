#include "stdafx.h"
#include "SVGLinearGradient.h"
#include <vector>
#include <algorithm>

SVGLinearGradient::SVGLinearGradient() {
    // Giá trị mặc định theo chuẩn SVG
    x1 = 0.0f; // 0%
    y1 = 0.0f; // 0%
    x2 = 1.0f; // 100%
    y2 = 0.0f; // 0%
}

void SVGLinearGradient::parseAttributes(xml_node<>* node) {
    SVGGradient::parseAttributes(node); // Parse stops và ID

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

Gdiplus::LinearGradientBrush* SVGLinearGradient::createBrush(const Gdiplus::RectF& bounds) {
    if (stops.empty()) return nullptr;

    // 1. Tính toán điểm bắt đầu và kết thúc
    Gdiplus::PointF startPoint, endPoint;

    if (gradientUnits == "userSpaceOnUse") {
        startPoint.X = x1;
        startPoint.Y = y1;
        endPoint.X = x2;
        endPoint.Y = y2;
    }
    else { // objectBoundingBox (Mặc định)
        startPoint.X = bounds.X + x1 * bounds.Width;
        startPoint.Y = bounds.Y + y1 * bounds.Height;
        endPoint.X = bounds.X + x2 * bounds.Width;
        endPoint.Y = bounds.Y + y2 * bounds.Height;
    }

    // 2. Tạo Brush tạm
    Gdiplus::LinearGradientBrush* brush = new Gdiplus::LinearGradientBrush(
        startPoint, endPoint,
        Gdiplus::Color(255, 0, 0, 0),
        Gdiplus::Color(255, 255, 255, 255)
    );

    // 3. Chuẩn bị màu trộn (Interpolation Colors)
    int count = (int)stops.size();
    std::vector<Gdiplus::Color> gdiColors(count);
    std::vector<REAL> blendPositions(count);

    for (int i = 0; i < count; i++) {
        SVGColor c = stops[i].getStopColor();
        BYTE alpha = (BYTE)(stops[i].getStopOpacity() * c.getA());

        gdiColors[i] = Gdiplus::Color(alpha, c.getR(), c.getG(), c.getB());
        blendPositions[i] = (REAL)stops[i].getOffset();
    }

    // Fix lỗi GDI+: blendPositions bắt buộc phải có 0.0 và 1.0
    if (blendPositions.front() > 0.001f) {
        blendPositions.insert(blendPositions.begin(), 0.0f);
        gdiColors.insert(gdiColors.begin(), gdiColors.front());
        count++;
    }
    if (blendPositions.back() < 0.999f) {
        blendPositions.push_back(1.0f);
        gdiColors.push_back(gdiColors.back());
        count++;
    }

    brush->SetInterpolationColors(gdiColors.data(), blendPositions.data(), count);

    // 4. Áp dụng Transform
    if (gradientTransform) {
        brush->MultiplyTransform(gradientTransform);
    }

    // 5. Spread Method
    if (spreadMethod == "reflect") brush->SetWrapMode(Gdiplus::WrapModeTileFlipXY);
    else if (spreadMethod == "repeat") brush->SetWrapMode(Gdiplus::WrapModeTile);
    else brush->SetWrapMode(Gdiplus::WrapModeClamp);

    return brush;
}