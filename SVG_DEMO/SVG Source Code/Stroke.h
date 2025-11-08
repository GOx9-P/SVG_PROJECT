#ifndef _STROKE_H_
#define _STROKE_H_

#include "Library.h"
#include "Color.h"
class SVGStroke {
private:
    SVGColor color;
    float width;
public:
    SVGStroke();
    SVGStroke(const SVGColor& c, float w);
    SVGColor& getColor();
    float getWidth();
    void setColor(SVGColor& c);
    void setWidth(float w);
    bool isVisible();
};

#endif // !1


