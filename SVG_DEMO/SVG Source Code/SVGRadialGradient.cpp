#include "stdafx.h"
#include "SVGRadialGradient.h"
#include "SVGStop.h"
#include <algorithm>
#include <vector>
#include <cmath>
#include <sstream>
#include <string>

using namespace std;

// Helper: Parse số an toàn (xử lý dấu phẩy/chấm)
static float parseFloat(const string& str) {
    if (str.empty()) return 0.0f;
    string s = str;
    for (char& c : s) if (c == ',') c = '.';
    stringstream ss(s);
    ss.imbue(std::locale::classic());
    float val = 0;
    ss >> val;
    return val;
}

// Helper: Parse Transform (Hỗ trợ matrix, translate, scale, rotate, skew)
static void applyTransformString(Gdiplus::Matrix* matrix, const string& transStr) {
    if (transStr.empty()) return;
    matrix->Reset();
    string s = transStr;
    // Chuẩn hóa chuỗi: thay thế ký tự lạ bằng khoảng trắng
    for (size_t i = 0; i < s.length(); ++i) {
        char c = s[i];
        if (c == ',' || c == '(' || c == ')') s[i] = ' ';
        // Xử lý trường hợp số âm dính liền (vd: 0-10 -> 0 -10)
        else if (c == '-' && i > 0 && (isdigit(s[i - 1]) || s[i - 1] == '.')) {
            s.insert(i, " "); i++;
        }
    }
    stringstream ss(s);
    ss.imbue(std::locale::classic());
    string token;
    while (ss >> token) {
        if (token == "matrix") {
            float m[6]; for (int i = 0; i < 6; ++i) ss >> m[i];
            Gdiplus::Matrix temp(m[0], m[1], m[2], m[3], m[4], m[5]);
            matrix->Multiply(&temp, Gdiplus::MatrixOrderAppend);
        }
        else if (token == "translate") {
            float tx, ty = 0; ss >> tx; if (!(ss >> ty)) ty = 0;
            matrix->Translate(tx, ty, Gdiplus::MatrixOrderAppend);
        }
        else if (token == "scale") {
            float sx, sy; ss >> sx; if (!(ss >> sy)) sy = sx;
            matrix->Scale(sx, sy, Gdiplus::MatrixOrderAppend);
        }
        else if (token == "rotate") {
            float angle, cx = 0, cy = 0; ss >> angle;
            streampos oldPos = ss.tellg();
            if (ss >> cx >> cy) {
                matrix->Translate(cx, cy, Gdiplus::MatrixOrderAppend); // Fix thứ tự rotate tâm
                matrix->Rotate(angle, Gdiplus::MatrixOrderAppend);
                matrix->Translate(-cx, -cy, Gdiplus::MatrixOrderAppend);
            }
            else {
                ss.clear(); ss.seekg(oldPos);
                matrix->Rotate(angle, Gdiplus::MatrixOrderAppend);
            }
        }
        // skewX, skewY nếu cần...
    }
}

static float distSq(const Gdiplus::PointF& p1, const Gdiplus::PointF& p2) {
    return (p1.X - p2.X) * (p1.X - p2.X) + (p1.Y - p2.Y) * (p1.Y - p2.Y);
}

SVGRadialGradient::SVGRadialGradient() {
    cx = 0.5f; cy = 0.5f; r = 0.5f; fx = 0.5f; fy = 0.5f;
}

void SVGRadialGradient::parseAttributes(xml_node<>* node) {
    SVGGradient::parseAttributes(node);

    // Parse lại stops con nếu SVGGradient chưa làm đúng
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
        // Trim spaces
        size_t first = val.find_first_not_of(" \t\r\n");
        if (string::npos != first) val = val.substr(first, val.find_last_not_of(" \t\r\n") - first + 1);

        float f = 0;
        if (!val.empty() && val.back() == '%') {
            val.pop_back();
            f = parseFloat(val) / 100.0f;
        }
        else {
            f = parseFloat(val);
        }
        return f;
        };

    cx = parseVal(node->first_attribute("cx"), 0.5f);
    cy = parseVal(node->first_attribute("cy"), 0.5f);
    r = parseVal(node->first_attribute("r"), 0.5f);
    fx = parseVal(node->first_attribute("fx"), cx);
    fy = parseVal(node->first_attribute("fy"), cy);

    if (xml_attribute<>* attr = node->first_attribute("gradientTransform")) {
        if (!this->gradientTransform) this->gradientTransform = new Gdiplus::Matrix();
        applyTransformString(this->gradientTransform, attr->value());
    }
}

Gdiplus::Brush* SVGRadialGradient::createBrush(const Gdiplus::RectF& boundingBox) {
    if (stops.empty()) return new Gdiplus::SolidBrush(Gdiplus::Color::Black);

    REAL rawCx = (REAL)cx; REAL rawCy = (REAL)cy; REAL rawR = (REAL)r;
    REAL rawFx = (REAL)fx; REAL rawFy = (REAL)fy;

    // 1. Focal Point Clamping (Tránh lỗi GDI+ khi fx nằm ngoài r)
    REAL dx = rawFx - rawCx; REAL dy = rawFy - rawCy;
    REAL dist = sqrt(dx * dx + dy * dy);
    if (rawR > 0 && dist >= rawR * 0.99f) {
        REAL scale = (rawR * 0.99f) / dist;
        rawFx = rawCx + dx * scale; rawFy = rawCy + dy * scale;
    }

    // 2. Tính Matrix biến đổi (Transform)
    Gdiplus::Matrix finalMatrix;
    if (gradientTransform) finalMatrix.Multiply(gradientTransform);

    // Áp dụng ObjectBoundingBox: Scale từ đơn vị (0-1) ra kích thước thật
    if (gradientUnits != "userSpaceOnUse") {
        finalMatrix.Scale(boundingBox.Width, boundingBox.Height, Gdiplus::MatrixOrderAppend);
        finalMatrix.Translate(boundingBox.X, boundingBox.Y, Gdiplus::MatrixOrderAppend);
    }

    // 3. Dynamic Expansion (Tự động mở rộng Brush nếu R chưa đủ phủ kín)
    // Với gradient #d (r=140%), rawR đã lớn, nhưng ta vẫn cần kiểm tra.
    REAL expansionScale = 1.0f;
    Gdiplus::Matrix inverseMatrix;
    inverseMatrix.Multiply(&finalMatrix);

    if (inverseMatrix.Invert() == Gdiplus::Status::Ok) {
        // Lấy 4 góc của BoundingBox
        Gdiplus::PointF corners[4] = {
            Gdiplus::PointF(boundingBox.X, boundingBox.Y),
            Gdiplus::PointF(boundingBox.X + boundingBox.Width, boundingBox.Y),
            Gdiplus::PointF(boundingBox.X, boundingBox.Y + boundingBox.Height),
            Gdiplus::PointF(boundingBox.X + boundingBox.Width, boundingBox.Y + boundingBox.Height)
        };
        inverseMatrix.TransformPoints(corners, 4); // Chuyển góc về không gian đơn vị

        float maxDistSq = 0.0f;
        Gdiplus::PointF centerP(rawCx, rawCy);
        for (int i = 0; i < 4; i++) {
            float d = distSq(centerP, corners[i]);
            if (d > maxDistSq) maxDistSq = d;
        }

        // Nếu khoảng cách xa nhất lớn hơn bán kính r, cần mở rộng brush
        // padding 1.01f để tránh viền răng cưa
        if (rawR > 0 && sqrt(maxDistSq) > rawR) {
            expansionScale = (sqrt(maxDistSq) / rawR) * 1.01f;
        }
    }
    else {
        expansionScale = 2.0f; // Fallback an toàn
    }

    // 4. Tạo PathGradientBrush
    Gdiplus::GraphicsPath path;
    REAL expandedR = rawR * expansionScale;

    // Tạo hình tròn bao quanh gradient (đã mở rộng)
    path.AddEllipse(rawCx - expandedR, rawCy - expandedR, 2 * expandedR, 2 * expandedR);
    path.Transform(&finalMatrix); // Áp dụng scale/rotate ra không gian màn hình

    Gdiplus::PathGradientBrush* pthGrBrush = new Gdiplus::PathGradientBrush(&path);

    // Cực kỳ quan trọng cho các gradient như #d (cy âm)
    // TileFlipXY giúp mép gradient tiếp tục lan màu ra ngoài vùng brush thay vì bị cắt
    pthGrBrush->SetWrapMode(Gdiplus::WrapModeTileFlipXY);
    pthGrBrush->SetGammaCorrection(TRUE);

    Gdiplus::PointF focalPoint(rawFx, rawFy);
    finalMatrix.TransformPoints(&focalPoint, 1);
    pthGrBrush->SetCenterPoint(focalPoint);

    // 5. Mapping Màu (SVG offset -> GDI+ position)
    struct StopData { REAL pos; Gdiplus::Color col; };
    std::vector<StopData> sortedStops;

    for (const auto& stop : stops) {
        SVGColor c = stop.getStopColor();
        BYTE a = (BYTE)(stop.getStopOpacity() * c.getA());

        // Map: SVG(0=Tâm, 1=Bìa) -> GDI(1=Tâm, 0=Bìa)
        // Cần chia cho expansionScale vì brush đã bị phóng to
        REAL gdiPos = 1.0f - (stop.getOffset() / expansionScale);

        if (gdiPos > 1.0f) gdiPos = 1.0f;
        if (gdiPos < 0.0f) gdiPos = 0.0f;

        sortedStops.push_back({ gdiPos, Gdiplus::Color(a, c.getR(), c.getG(), c.getB()) });
    }

    // Sắp xếp stops (GDI tính từ bìa 0.0 vào tâm 1.0)
    std::sort(sortedStops.begin(), sortedStops.end(), [](const StopData& a, const StopData& b) { return a.pos < b.pos; });

    // 6. Padding (Lấp đầy màu) - Fix quan trọng cho #d
    // Gradient #d chỉ có stop từ 0.13 đến 0.53. 
    // Ta cần lấp đầy từ 0.0->0.47 (màu cuối) và 0.87->1.0 (màu đầu)
    if (!sortedStops.empty()) {
        // Lấp đầy viền ngoài (nếu stop cuối cùng của SVG không chạm tới bán kính mở rộng)
        if (sortedStops.front().pos > 0.0f) {
            sortedStops.insert(sortedStops.begin(), { 0.0f, sortedStops.front().col });
        }
        // Lấp đầy tâm (nếu stop đầu tiên của SVG không phải là 0)
        if (sortedStops.back().pos < 1.0f) {
            sortedStops.push_back({ 1.0f, sortedStops.back().col });
        }
    }

    std::vector<Gdiplus::Color> finalColors;
    std::vector<REAL> finalPositions;
    for (const auto& s : sortedStops) {
        finalColors.push_back(s.col);
        finalPositions.push_back(s.pos);
    }

    if (!finalColors.empty()) {
        pthGrBrush->SetInterpolationColors(finalColors.data(), finalPositions.data(), (int)finalColors.size());
    }

    return pthGrBrush;
}