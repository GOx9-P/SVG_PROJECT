#ifndef _SVGRADIALGRADIENT_H_
#define _SVGRADIALGRADIENT_H_

#include "SVGGradient.h"

class SVGRadialGradient : public SVGGradient {
private:
    float cx, cy, r, fx, fy;

public:
    SVGRadialGradient();
    void parseAttributes(xml_node<>* node) override;

    // Override hàm tạo brush: Sử dụng PathGradientBrush để mô phỏng Radial Gradient
    Gdiplus::Brush* createBrush(const Gdiplus::RectF& boundingBox) override;
};

#endif