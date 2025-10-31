#pragma once
#include "Library.h"

class SVGPoint {
private:
	float x;
	float y;
public:
	SVGPoint();
	void translate(float dx, float dy);
	~SVGPoint();
};
