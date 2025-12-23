#ifndef _SVGLINEARGRADIENT_H_
#define _SVGLINEARGRADIENT_H_

#include "SVGGradient.h"

class SVGLinearGradient : public SVGGradient {
private:
    float x1, y1, x2, y2;

public:
    SVGLinearGradient();

    void parseAttributes(xml_node<>* node) override;

    // Hàm quan trọng: Tạo Brush GDI+ dựa trên bounding box của hình cần vẽ
    Gdiplus::Brush* createBrush(const Gdiplus::RectF& boundingBox) override;
};

#endif