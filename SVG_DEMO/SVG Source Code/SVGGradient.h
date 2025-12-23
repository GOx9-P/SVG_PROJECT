#ifndef _SVGGRADIENT_H_
#define _SVGGRADIENT_H_

#include "SVGElement.h"
#include "SVGStop.h"
#include <vector>

class SVGGradient : public SVGElement {
protected:
    std::vector<SVGStop> stops;
    string gradientUnits; // "userSpaceOnUse" hoặc "objectBoundingBox"
    string spreadMethod;  // "pad", "reflect", "repeat"
    Gdiplus::Matrix* gradientTransform;

public:
    SVGGradient();
    virtual ~SVGGradient();

    void addStop(const SVGStop& stop);
    std::vector<SVGStop> getStops() const;

    // Helper: Parse chuỗi offset (ví dụ "50%" -> 0.5)
    float parseOffset(const string& val);

    void parseAttributes(xml_node<>* node) override;

    // Hàm draw rỗng vì Gradient không tự vẽ mình
    void draw(Graphics* graphics) override {}
    Gdiplus::RectF getBoundingBox() override;

    virtual Gdiplus::Brush* createBrush(const Gdiplus::RectF& boundingBox) = 0;
};

#endif