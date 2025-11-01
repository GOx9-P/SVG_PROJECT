#pragma once

#include "Library.h"
#include "Color.h"
class SVGStroke {
private:
    SVGColor color;
    float width;
public:
    SVGStroke();
    SVGStroke(const Color& c, float w);
    Color& getColor();
    float getWidth();
    void setColor(Color& c);
    void setWidth(float w);
    bool isVisible();
};