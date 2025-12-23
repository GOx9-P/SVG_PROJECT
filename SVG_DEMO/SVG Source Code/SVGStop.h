#ifndef _SVGSTOP_H_
#define _SVGSTOP_H_

#include "Color.h"

class SVGStop {
private:
    float offset; // Vị trí từ 0.0 đến 1.0
    SVGColor stopColor;
    float stopOpacity;

public:
    SVGStop();
    SVGStop(float offset, const SVGColor& color, float opacity);

    float getOffset() const;
    void setOffset(float val);

    void parseAttributes(xml_node<>*);

    SVGColor getStopColor() const;
    void setStopColor(const SVGColor& val);

    float getStopOpacity() const;
    void setStopOpacity(float val);
};

#endif

