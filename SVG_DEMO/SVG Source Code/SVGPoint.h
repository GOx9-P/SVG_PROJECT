#pragma once
#include "Library.h"

class SVGPoint {
private:
	float x;
	float y;
public:
	float getX() const;
	void setX(float&);
	float getY() const;
	void setY(float&);
	SVGPoint();
	void translate(float dx, float dy);
	~SVGPoint();
};
