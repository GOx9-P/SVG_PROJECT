#pragma once
#include "Library.h"

class SVGPoint {
private:
	float x;
	float y;
public:
	SVGPoint();
	SVGPoint(float x = 0.0f, float y = 0.0f);
	void translate(float dx, float dy);
	~SVGPoint();
};
