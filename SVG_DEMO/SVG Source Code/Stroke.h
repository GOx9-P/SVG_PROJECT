#ifndef _STROKE_H_
#define _STROKE_H_

#include "Library.h"
#include "Color.h"
class SVGStroke {
private:
    SVGColor color;
    float width;
    LineCap lineCap;   
    LineJoin lineJoin;
public:
    SVGStroke();
    SVGStroke(const SVGColor& c, float w);
    SVGColor& getColor();
    float getWidth();
    LineCap getLineCap() const;
    LineJoin getLineJoin() const;
    void setColor(SVGColor& c);
    void setWidth(float w);
    void setLineCap(const string& type);
    void setLineJoin(const string& type);
    
    bool isVisible();
};

#endif // !1


