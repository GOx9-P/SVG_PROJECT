#ifndef _SVGPOINT_H_
#define _SVGPOINT_H_

#include "Library.h"

class SVGPoint {
private:
	float x;
	float y;
public:
	float getX() const;
	void setX(const float&);
	float getY() const;
	void setY(const float&);
	SVGPoint();
	void translate(const float, const float);
	~SVGPoint();
};

#endif // !_SVGPOINT_H_

