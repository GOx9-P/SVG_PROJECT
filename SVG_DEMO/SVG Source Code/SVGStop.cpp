#include "stdafx.h"
#include "SVGStop.h"

SVGStop::SVGStop() : offset(0.0f), stopOpacity(1.0f) {
    stopColor = SVGColor(0, 0, 0, 255);
}

SVGStop::SVGStop(float offset, const SVGColor& color, float opacity) {
    this->offset = offset;
    this->stopColor = color;
    this->stopOpacity = opacity;
}

float SVGStop::getOffset() const { return offset; }
void SVGStop::setOffset(float val) { offset = val; }

SVGColor SVGStop::getStopColor() const { return stopColor; }
void SVGStop::setStopColor(const SVGColor& val) { stopColor = val; }

float SVGStop::getStopOpacity() const { return stopOpacity; }
void SVGStop::setStopOpacity(float val) { stopOpacity = val; }