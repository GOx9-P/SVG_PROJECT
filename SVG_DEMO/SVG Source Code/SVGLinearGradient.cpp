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

    Gdiplus::LinearGradientBrush* brush = new Gdiplus::LinearGradientBrush(
        startPoint, endPoint,
        Gdiplus::Color::Black, Gdiplus::Color::White
    );

   
    if (gradientTransform) {
       
        brush->MultiplyTransform(gradientTransform);
    }


    struct StopData { REAL pos; Gdiplus::Color col; };
    std::vector<StopData> sortedStops;

    for (const auto& stop : stops) {
        SVGColor c = stop.getStopColor();
        BYTE a = (BYTE)(stop.getStopOpacity() * c.getA());
        REAL pos = stop.getOffset();

       
        if (pos < 0.0f) pos = 0.0f;
        if (pos > 1.0f) pos = 1.0f;

        sortedStops.push_back({ pos, Gdiplus::Color(a, c.getR(), c.getG(), c.getB()) });
    }

    std::sort(sortedStops.begin(), sortedStops.end(),
        [](const StopData& a, const StopData& b) { return a.pos < b.pos; });

   
    if (!sortedStops.empty()) {
      
        if (sortedStops.front().pos > 0.001f) {
            sortedStops.insert(sortedStops.begin(), { 0.0f, sortedStops.front().col });
        }
        else {
           
            sortedStops.front().pos = 0.0f;
        }

       
        if (sortedStops.back().pos < 0.999f) {
            sortedStops.push_back({ 1.0f, sortedStops.back().col });
        }
        else {
            sortedStops.back().pos = 1.0f;
        }
    }

  
    int count = (int)sortedStops.size();
    if (count >= 2) {
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
    brush->SetWrapMode(Gdiplus::WrapModeTileFlipXY);

    return brush;
}